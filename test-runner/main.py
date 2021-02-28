import sys
from parser import parser
from eval_context import EvalContext
import stmt

if len(sys.argv) < 2:
    print('usage: {0} filename'.format(sys.argv[0]))
    sys.exit(1)

filename = sys.argv[1]

signal_line = None
main_program = ''
commands = []

with open(filename, 'r') as f:
    for line in f:
        stripped_line = line.strip()
        if (stripped_line != '') and (not signal_line):
            signal_line = stripped_line
            main_program += '\n'
        else:
            if stripped_line.startswith('#!'):
                commands.append(stripped_line)
            main_program += line

signals = signal_line.split()

print('Signals:\n' + str(signals) + '\n')

program = parser.parse(main_program)

print('Input:\n' + str(program))

result = program.eval(EvalContext())

print('Output:\n' + str(result) + '\n')

print(result.map(signals))
