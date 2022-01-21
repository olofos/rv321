import sys
import argparse
import serial
from runner import runner, UART
# from uart import UART


parser = argparse.ArgumentParser()
parser.add_argument('filename', help='name of test file')
parser.add_argument('-p', '--port', help='serial port', default='/dev/ttyUSB0')
parser.add_argument('-b', '--baud', help='baud rate', type=int, default=1000000)
parser.add_argument('-w', '--wait', help='wait for input between rows', action='store_true')
group = parser.add_mutually_exclusive_group()
group.add_argument('-n', '--test-number', type=int)
group.add_argument('-t', '--test-name', type=str)
args = parser.parse_args()

filename = args.filename

tests = runner.load(filename)
test = None

if (args.test_number is None) and (args.test_name is None):
    if len(tests) == 1:
        test = tests[0]
elif args.test_number is not None:
    test = tests[args.test_number]
elif args.test_name is not None:
        for test_case in tests:
            if test_case['name'] == args.test_name:
                test = test_case
                break
if test is None:
    test_name_string = 'Available test cases:\n' + '\n'.join(['{0}: {1}'.format(i,t['name']) for i,t in enumerate(tests)])
    sys.exit(test_name_string)

rows = runner.execute(test)

print('Opening serial port.')
uart = UART.UART(serial.Serial(args.port, args.baud, timeout=1))

# print('Running test.')

number_of_errors = 0

uart.send('set-anim 2')

for row_index, row in enumerate(rows):
    # print('Running test, line {0}/{1}\r'.format(row_index, len(rows)), end='', flush=True)
    print('Running test [{}%]\r'.format(100 * row_index // len(rows)), end='', flush=True)
    uart.send_row(row)
    if row.needs_input:
        row.read_input = uart.read_io()

        if not row:
            uart.send('set-anim 1')
            number_of_errors += 1
            print('Mismatch on line {}: '.format(row.line), end='')
            mismatches = [ '{0}: F={1}/E={2}'.format(sig, vals['found'], vals['expected']) for sig, vals in row.mismatched_signals.items() ]
            print(', '.join(mismatches))
            print(row.vars)
    if args.wait:
        print(row.vars)
        input('Press enter for next line ({})'.format(row.line))

if number_of_errors == 0:
    uart.send('set-anim 3')

print('Done.')
