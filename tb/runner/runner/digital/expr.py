import random

class Expr : pass

class BinOp(Expr):
    def __init__(self, left, op, right):
        self.left = left
        self.right = right
        self.op = op

    def __str__(self):
        return '(' + str(self.left) + ' ' + str(self.op) + ' ' + str(self.right) + ')'

    def eval(self, ctx):
        left = self.left.eval(ctx)
        right = self.right.eval(ctx)
        if self.op == '=':
            return 1 if (left == right) else 0
        elif self.op == '!=':
            return 1 if (left != right) else 0
        elif self.op == '<':
            return 1 if (left < right) else 0
        elif self.op == '>':
            return 1 if (left > right) else 0
        elif self.op == '<=':
            return 1 if (left <= right) else 0
        elif self.op == '>=':
            return 1 if (left >= right) else 0
        elif self.op == '|':
            return (left | right)
        elif self.op == '^':
            return (left ^ right)
        elif self.op == '&':
            return (left & right)
        elif self.op == '<<':
            return (left << right)
        elif self.op == '>>':
            return (left >> right)
        elif self.op == '+':
            return (left + right)
        elif self.op == '-':
            return (left - right)
        elif self.op == '*':
            return (left * right)
        elif self.op == '/':
            return (left // right)
        elif self.op == '%':
            return (left % right)

class UnaryOp(Expr):
    def __init__(self, op, expr):
        self.op = op
        self.expr = expr

    def __str__(self):
        return str(self.op) + str(self.expr)

    def eval(self, ctx):
        value = self.expr.eval(ctx)
        if self.op == '-':
            return -value
        elif self.op == '~':
            return ~value
        elif self.op == '!':
            return 1 if value == 0 else 0

class Number(Expr):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return str(self.value)

    def eval(self, ctx):
        return self.value

class Variable(Expr):
    def __init__(self, name):
        self.name = name

    def __str__(self):
        return self.name

    def eval(self, ctx):
        return ctx.get_variable(self.name)


def func_random(params):
    assert len(params) == 1
    return random.randrange(params[0])

def func_ite(params):
    assert len(params) == 3
    if params[0] != 0:
        return params[1]
    return params[2]

func_tab = {
    'random': func_random,
    'ite': func_ite,
}

class Function(Expr):
    def __init__(self, name, params):
        self.name = name
        self.params = params

    def __str__(self):
        return self.name + '(' + ', '.join(str(p) for p in self.params) + ')'

    def eval(self, ctx):
        params = [ p.eval(ctx) for p in self.params ]
        func = func_tab[self.name]
        return func(params)
