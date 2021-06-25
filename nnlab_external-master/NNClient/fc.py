# In asynchronous computations last (nireq - 1) lines are not logged
# Decided to not overcomplicate samples, and ignore line number difference

import argparse
import sys

def fc(reference_file, output_file, max_line_num_diff = 10):

    with open(output_file, 'r') as f:
        output_lines = [line.strip() for line in f.readlines() if len(line) > 0]

    with open(reference_file, 'r') as f:
        reference_lines = [line.strip() for line in f.readlines() if len(line) > 0]

    if abs(len(output_lines) - len(reference_lines)) > max_line_num_diff:
        print('Line number difference is greater than threshold')
        return False

    if len(reference_file) == 0:
        print('Empty reference file')
        return False

    compare_lines_num = min(len(output_lines), len(reference_lines))

    for i in range(compare_lines_num):
        if output_lines[i] != reference_lines[i]:
            print('Files mismatch on line {}:'.format(i + 1))
            print('"{}" vs "{}".'.format(output_lines[i], reference_lines[i]))
            return False

    print('Files match')
    return True

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="fc for nnlab tests")
    parser.add_argument('-r', '--reference', help='path to reference.txt', required=True)
    parser.add_argument('-o', '--output', help='path to output file', required=True)

    args = parser.parse_args()

    if fc(args.reference, args.output):
        sys.exit(0)

    sys.exit(1)
