class Context :
    def __init__(self):
        self.vars = {}
        self.stack = []

    def push(self):
        self.stack.append(self.vars)

    def pop(self):
        self.vars = self.stack.pop()

    def set_variable(self, name, value):
        self.vars = {**self.vars, name: value}
        pass

    def get_variable(self, name):
        return self.vars[name]

class List:
    def  __init__(self, rows):
        self.rows = rows

    def format(self, indent):
        return ''.join(row.format(indent) for row in self.rows)

    def __str__(self):
        return self.format('  ')

    def map(self, signals):
        return [ row.map(signals) for row in self.rows ]

    def __len__(self):
        return len(self.rows)

    def __getitem__(self, index):
        return self.rows[index]

class DataRow():
    def __init__(self, line, data, vars = None):
        self.line = line
        self.data = data
        self.vars = vars

    def format(self, indent):
        return str(self.line) + indent + ' '.join(str(d) for d in self.data) + '\n'

    def __repr__(self):
        return ' '.join(str(d) for d in self.data)

    def map(self, signals):
        entries = self.data
        assert len(entries) == len(signals)
        result = {}
        for i in range(0, len(signals)):
            try:
                result[signals[i]] = int(entries[i])
            except:
                pass
        return MappedDataRow(result, self)

class MappedDataRow():
    def __init__(self, signal_data, row):
        self.signal_data = signal_data
        self.row = row
