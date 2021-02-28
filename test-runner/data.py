class Data : pass

class Bits(Data):
    def __init__(self, number, expr):
        self.number = number.value
        self.expr = expr

    def __str__(self):
        return 'bits(' + str(self.number) + ', ' + str(self.expr) + ')'

    def eval(self, ctx):
        value = self.expr.eval(ctx)
        return [ Number((value >> (self.number - n - 1)) & 1) for n in range(0, self.number) ]

class Expr(Data):
    def __init__(self, expr):
        self.expr = expr

    def __str__(self):
        return '(' + str(self.expr) + ')'

    def eval(self, ctx):
        return [ Number(self.expr.eval(ctx)) ]

class Number(Data):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return str(self.value)

    def eval(self, ctx):
        return [self]

    def get_value(self):
        return self.value

class Id(Data):
    def __init__(self, name):
        self.name = name

    def __str__(self):
        return self.name

    def get_value(self):
        return self.name

    def eval(self, ctx):
        return [self]
