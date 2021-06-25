import os
import sys

all_files = []
rootdir = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'NNClient/NNClient')
for root, subdirs, files in os.walk(rootdir):
    for file in files:
        full_file_path = os.path.join(root, file)
        nnclient_start_idx = full_file_path.rfind('/NNClient')
        all_files.append(full_file_path[nnclient_start_idx:].replace('/', '\\'))

all_files_with_pwd = []
for file in all_files:
    file_with_pwd = '$$PWD' + file
    all_files_with_pwd.append(file_with_pwd)

headers = 'HEADERS += \\\n'
sources = 'SOURCES += \\\n'

all_files_with_pwd = sorted(all_files_with_pwd)

for file in all_files_with_pwd:
    if file.endswith('.h'):
        headers += '\t\t' + file + ' \\\n'
    if file.endswith('.cpp'):
        sources += '\t\t' + file + ' \\\n'

reference_str = headers + '\n\n' + sources + '\n' + 'INCLUDEPATH += $$PWD' + '\n\nwin32: LIBS += Advapi32.lib\n'

if len(sys.argv) > 1:
    with open(sys.argv[1], 'w') as fdump:
        fdump.write(reference_str)

with open(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'NNClient/NNClient.pri')) as ftest:
    test_str = ftest.read()
    assert test_str == reference_str

print('NNClient.pri is correct')
