import argparse
import json
import os
import re
import shutil
from subprocess import Popen, PIPE
import subprocess
import sys

def get_guids(cuda_devices_executable):
    cuda_device_output = Popen(cuda_devices_executable, shell=True, bufsize=1, stdout=PIPE).communicate()[0].splitlines()
    first_string = cuda_device_output[0].decode('utf-8')
    if 'Couldn\'t obtain CUDA devices info' in first_string:
        print(first_string)
        sys.exit(1)
    guids = list(line.decode('utf-8').split(' ')[-2] for line in cuda_device_output)
    return guids


def device_check(device_detect_executable, guids):

    def get_device_json(device_detect_output, vendor):

        match_list = re.findall(r'"{}":\s\[.*?\]'.format(vendor), device_detect_output, flags=re.DOTALL)

        if len(match_list) == 0 and len(guids) > 0:
            print('No {} devices'.format(vendor))
            print(device_detect_output)
            sys.exit(1)

        assert len(match_list) == 1, match_list

        device_json = match_list[0]
        device_json = json.loads(device_json[device_json.find('[') :])

        return device_json

    targets_dir = os.path.join(os.path.dirname(__file__),
                               'Targets',
                               'FrameProcessing',
                               'FrameProcessingData')

    for target_folder in os.listdir(targets_dir):

        target_folder_full = os.path.join(targets_dir, target_folder)

        if os.path.isfile(target_folder_full):
            continue

        assert os.path.isdir(target_folder_full), target_folder_full

        target_task_config_file = os.path.join(target_folder_full, 'taskConfig.json')
        assert os.path.isfile(target_task_config_file), target_task_config_file

        with open(target_task_config_file, 'r') as target_task_config_file:
            task_config = json.load(target_task_config_file)

        module = task_config['module']
        task = task_config['task']

        print('Checking devices for target {}'.format(target_folder))

        device_detect_output = subprocess.run([device_detect_executable, module, task], stdout=PIPE).stdout
        device_detect_output = device_detect_output.decode('utf-8')

        if target_folder.startswith('Caffe2'):

            device_json = get_device_json(device_detect_output, 'NVIDIA')

            guids_set = set(guids)

            for device in device_json:
                device_guid = device['device']
                if device_guid not in guids_set:
                    print('Unexpected GUID {}'.format(device_guid))
                    sys.exit(1)
                print(device_guid)
                guids_set.discard(device_guid)

            if guids_set:
                print('guids_set not empty: {}'.format(guids_set))
                sys.exit(1)

        else:

            assert target_folder.startswith('OpenVINO')

            if 'device.txt' not in os.listdir(target_folder_full):
                continue

            device_json = get_device_json(device_detect_output, 'INTEL')

            with open(os.path.join(target_folder_full, 'device.txt')) as f:
                plugins_set = set(line.strip() for line in f.readlines())

            for device in device_json:
                plugin = device['device']
                if plugin not in plugins_set:
                    print('Unexpected device {}'.format(plugin))
                    sys.exit(1)
                print(plugin)
                plugins_set.discard(plugin)

            if plugins_set:
                print('plugins_set not empty: {}'.format(plugins_set))
                sys.exit(1)

        print('Target {} device check passed'.format(target_folder))

    print('Device check successfull')


def frame_processing_check(frame_processing_executable, guids):

    if os.path.exists('logs'):
        shutil.rmtree('logs')
    os.mkdir('logs')

    tmp_task_config_file = 'task_config.json'
    if os.path.isfile(tmp_task_config_file):
        os.remove(tmp_task_config_file)

    targets_dir = os.path.join(os.path.dirname(__file__),
                           'Targets',
                           'FrameProcessing',
                           'FrameProcessingData')

    for target_folder in os.listdir(targets_dir):

        target_folder_full = os.path.join(targets_dir, target_folder)

        if os.path.isfile(target_folder_full):
            continue

        assert os.path.isdir(target_folder_full), target_folder_full

        if not any([os.path.isfile(os.path.join(target_folder_full, 'reference.txt')),
                   os.path.isfile(os.path.join(target_folder_full, 'reference_cpu.txt')),
                   os.path.isfile(os.path.join(target_folder_full, 'reference_gpu.txt'))]):
            print('No references found for target {}. Skipping.'.format(target_folder))
            continue

        print('Running target {}'.format(target_folder))

        target_task_config_file = os.path.join(target_folder_full, 'taskConfig.json')
        assert os.path.isfile(target_task_config_file), target_task_config_file

        target_demo_config_file = os.path.join(target_folder_full, 'demoConfig.json')
        assert os.path.isfile(target_demo_config_file), target_demo_config_file

        if target_folder.startswith('Caffe2'):
            devices = list(guids)
        else:
            openvino_devices_file_path = os.path.join(target_folder_full, 'device.txt')
            if os.path.isfile(openvino_devices_file_path):
                with open(openvino_devices_file_path) as f:
                    devices = [line.strip() for line in f.readlines()]
            else:
                devices = ['CPU']

        modes = ['sync']
        if os.path.isfile(os.path.join(target_folder_full, 'run_async.txt')):
            modes += ['async']

        for device_name in devices:
            for mode in modes:
                reference_file = os.path.join(target_folder_full, 'reference.txt')

                with open(target_task_config_file) as f:
                    task_config = json.load(f)

                task_config['device'] = device_name

                if mode == 'async':
                    task_config['nireq'] = 2
                else:
                    assert mode == 'sync'
                    if task_config.get('nireq', 0) > 0:
                        del task_config['nireq']

                with open(tmp_task_config_file, 'w') as f:
                    json.dump(task_config, f)

                subprocess.run([frame_processing_executable,
                                tmp_task_config_file,
                                target_demo_config_file])

                new_filename = os.path.join('logs', '{}_{}_{}.txt'.format(target_folder, device_name, mode))
                shutil.copy('log.txt', new_filename)

                return_code = subprocess.run(
                    'python {}\\fc.py --reference {} --output {}'.format(
                        os.path.abspath(os.path.dirname(__file__)),
                        reference_file,
                        new_filename)
                    ).returncode

                if return_code != 0:
                    print('Got return code {} on fc for target {}'.format(return_code, target_folder))
                    sys.exit(return_code)

                print('Target {} check with device {} and mode {} passed'.format(target_folder, device_name, mode))

        print('Target {} check passed'.format(target_folder))

    print('Frame processing check successfull')


if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('--CudaDevices', help='path to CudaDevices.exe', type=str, required=True)
    parser.add_argument('--DeviceDetect', help='path to DeviceDetect.exe', type=str, required=True)
    parser.add_argument('--FrameProcessing', help='path to FrameProcessing.exe', type=str, required=True)

    args = parser.parse_args()

    guids = get_guids(args.CudaDevices)
    device_check(args.DeviceDetect, guids)
    frame_processing_check(args.FrameProcessing, guids)
