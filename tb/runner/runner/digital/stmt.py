from . import evaluated

class List :
    def __init__(self, stmts):
        self.stmts = stmts

    def format(self, indent):
        return ''.join(stmt.format(indent) for stmt in self.stmts)

    def __str__(self):
        return self.format('  ')

    def eval(self, ctx = None):
        if ctx is None:
            ctx = evaluated.Context()
        result = []
        for stmt in self.stmts:
            result.extend(stmt.eval(ctx))
        return evaluated.List(result)

class Stmt : pass

class Let(Stmt):
    def __init__(self, line, variable, expr):
        self.line = line
        self.variable = variable
        self.expr = expr

    def format(self, indent):
        return str(self.line) + indent + 'let ' + str(self.variable) + ' = ' + str(self.expr) + ';\n'

    def __str__(self):
        return 'let ' + str(self.variable) + ' = ' + str(self.expr) + ';'

    def eval(self, ctx):
        ctx.set_variable(self.variable, self.expr.eval(ctx))
        return []

class DataRow(Stmt):
    def __init__(self, line, data, vars = None):
        self.line = line
        self.data = data

    def format(self, indent):
        return str(self.line) + indent + ' '.join(str(d) for d in self.data) + '\n'

    def __str__(self):
        return ' '.join(str(d) for d in self.data)

    def eval(self, ctx):
        entries = []
        for entry in self.data:
            entries.extend(entry.eval(ctx))
        return [ evaluated.DataRow(self.line, entries, ctx.vars) ]

    def get_values(self):
        return [ entry.get_value() for entry in self.data ]

class Repeat(Stmt):
    def __init__(self, line, number, data):
        self.line = line
        self.number = number
        self.data = data

    def format(self, indent):
        return str(self.line) + indent + 'repeat (' + str(self.number) + ') ' + ' '.join(str(d) for d in self.data) + '\n'

    def __str__(self):
        return 'repeat (' + str(self.number) + ') ' + ' '.join(str(d) for d in self.data)

    def eval(self, ctx):
        result = []
        ctx.push()
        for n in range(0, self.number):
            ctx.set_variable('n', n)
            entries = []
            for entry in self.data:
                entries.extend(entry.eval(ctx))
            result.append(evaluated.DataRow(self.line, entries))
        ctx.pop()
        return result

class Loop(Stmt):
    def __init__(self, line, variable, number, stmt_list):
        self.line = line
        self.variable = variable
        self.number = number
        self.stmt_list = stmt_list

    def format(self, indent):
        first_line = 'loop (' + str(self.variable) + ', ' + str(self.number) + ')\n'
        stmt_lines = self.stmt_list.format(indent + '  ')
        last_line = ' ' * len(str(self.line) + indent) + 'end loop\n'
        return str(self.line) + indent + first_line + stmt_lines + last_line

    def __str__(self):
        first_line = 'loop (' + str(self.variable) + ', ' + str(self.number) + ')\n'
        stmt_lines = str(self.stmt_list) + '\n'
        last_line = 'end loop\n'
        return first_line + stmt_lines + last_line

    def eval(self, ctx):
        result = []
        ctx.push()
        for n in range(0, self.number):
            ctx.set_variable(self.variable, n)
            result.extend(self.stmt_list.eval(ctx).rows)
        ctx.pop()
        return result
