#!/usr/bin/env python3

import sys

print(sys.argv)

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

tests = {}

def readFile(file):
    while True:
        line = file.readline()
        if len(line) == 0:
            break
        parts = line.strip().split(' ')
        if parts[0] == 'test:':
            tests[parts[1]] = { }
        elif parts[0] == 'success:':
            tests[parts[1]]['result'] = 'success'
        elif parts[0] == 'skip:':
            tests[parts[1]]['result'] = 'skip'
        elif parts[0] == 'failure:':
            tests[parts[1]]['result'] = 'failure'
            if parts[2] == '[':
                line2 = file.readline()
                line3 = file.readline()
                file_parts = line3.split(' ')[0].split(':')
                tests[parts[1]]['file'] = file_parts[0]
                tests[parts[1]]['line'] = file_parts[1]

print('---------------------------------------------')
if len(sys.argv) > 1:
    print('Reading test results\n')
    for filename in sys.argv[1:]:
        print(f'  {filename}')
        file = open(filename)
        readFile(file)
else:
    readFile(sys.stdin)
print(f'\n{len(tests)} test found')
print('---------------------------------------------')

passed = {k: v for k, v in tests.items() if v['result'] == 'success'}
failed = {k: v for k, v in tests.items() if v['result'] == 'failure'}
skipped = {k: v for k, v in tests.items() if v['result'] == 'skip'}

fail_color = bcolors.FAIL if len(failed) > 0 else ''
pass_color = bcolors.FAIL if len(failed) > 0 else '' if len(skipped) > 0 else bcolors.OKGREEN

print(f'Skipped: {len(skipped)} / {len(tests)}')
if len(skipped) > 0:
    print('');
for k, v in skipped.items():
    print(f'  {k}')
if len(skipped) > 0:
    print('');
print('---------------------------------------------')
print(f'Failed:  {fail_color}{len(failed)} / {len(tests)}{bcolors.ENDC}')
if len(failed) > 0:
    print('');
for k, v in failed.items():
    print(f'  {k} (' + v['file'] + ':' + v['line']  + ')')
if len(failed) > 0:
    print('');
print('---------------------------------------------')
print(f'Passed:  {pass_color}{len(passed)} / {len(tests)}{bcolors.ENDC}')
print('---------------------------------------------\n')
