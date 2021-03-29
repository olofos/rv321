from .digital.file import load
from .digital import parser
from . import header

def load(filename):
    try:
        tests = load(filename)
    except:
        with open(filename, 'r') as f:
            tests = [{'name': filename, 'test': f.read()}]
    return tests

def execute(test):
    if type(test) == dict:
        test = test['test']
    program = parser.parse(test)
    signals, hw_config = header.parse(test)
    mapped = program.eval().map(signals)
    return [ hw_config.get_state(row.signal_data) for row in mapped ]
