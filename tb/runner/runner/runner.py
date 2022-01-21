from .digital import file
from .digital import parser
from . import header

def load(filename):
    try:
        tests = file.load(filename)
    except:
        with open(filename, 'r') as f:
            tests = [{'name': filename, 'test': f.read()}]
    return tests

def execute(test):
    if type(test) == dict:
        test = test['test']

    print('Parsing headers', end='', flush=True)
    signals, hw_config = header.parse(test)
    print('.')

    print('Parsing test', end='', flush=True)
    program = parser.parse(test)
    print('.')

    print('Evaluating test', end='', flush=True)
    evaluated = program.eval()
    print('.')

    print('Mapping signal data', end='', flush=True)
    mapped = evaluated.map(signals)
    print('.')

    print('Mapping pin data', end='', flush=True)
    result = []
    progress_step = len(mapped)//100 if len(mapped) > 100 else 1

    for i, row in enumerate(mapped):
        result.append(hw_config.get_state(row.signal_data, row.row.line, row.row.vars, result[i-1] if i > 0 else None))
        if i % progress_step == 0:
            print('\rMapping pin data [{}%]'.format(100 * i//len(mapped)), end='', flush=True)
    print('\rMapping pin data.      ')

    # for row in result:
    #     str = ''
    #     str += ' '.join([ format(n, '04x') for n in row.output ])
    #     str += '   '
    #     str += ' '.join([ format(n, '04x') for n in row.iodir ])
    #     str += '   '
    #     str += ' '.join([ format(n, '04x') for n in row.pullup ])
    #     str += ' | '
    #     if row.delta.output is not None:
    #         str += ' '.join([ format(n, '04x') for n in row.delta.output ])
    #     else:
    #         str += '                   '
    #     str += '   '
    #     if row.delta.iodir is not None:
    #         str += ' '.join([ format(n, '04x') for n in row.delta.iodir ])
    #     else:
    #         str += '                   '
    #     str += '   '
    #     if row.delta.pullup is not None:
    #         str += ' '.join([ format(n, '04x') for n in row.delta.pullup ])
    #     else:
    #         str += '                   '
    #     print(str)

    return result
