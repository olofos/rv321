from eval_context import EvalContext
import data

class List :
    def __init__(self, stmts):
        self.stmts = stmts

    def format(self, indent):
        return ''.join(stmt.format(indent) for stmt in self.stmts)

    def __str__(self):
        return self.format('  ')

    def eval(self, ctx):
        result = []
        for stmt in self.stmts:
            result.extend(stmt.eval(ctx))
        return List(result)

    def map(self, signals):
        return [ stmt.map(signals) for stmt in self.stmts ]

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

class Data(Stmt):
    def __init__(self, line, data):
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
        return [ Data(self.line, entries) ]

    def map(self, signals):
        entries = [ entry.get_value() for entry in self.data ]
        if len(entries) != len(signals):
            print('Len oops')
        else:
            return { signals[i]: entries[i] for i in range(0, len(signals)) }

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
        inner_ctx = EvalContext(ctx)
        for n in range(0, self.number):
            inner_ctx.set_variable('n', n)
            entries = []
            for entry in self.data:
                entries.extend(entry.eval(inner_ctx))
            result.append(Data(self.line, entries))
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
        for n in range(0, self.number):
            inner_ctx = EvalContext(ctx)
            inner_ctx.set_variable(self.variable, n)
            result.extend(self.stmt_list.eval(inner_ctx).stmts)
        return result
