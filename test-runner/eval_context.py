import random

def func_random(params):
    assert len(params) == 1
    return random.randrange(params[0])

func_tab = {
    'random': func_random
}

class EvalContext :
    def __init__(self, parent = None):
        self.parent = parent
        self.vars = {}

    def set_variable(self, name, value):
        self.vars[name] = value
        pass

    def get_variable(self, name):
        if name in self.vars:
            return self.vars[name]
        elif self.parent:
            return self.parent.get_variable(name)

    def get_function(self, name):
        return func_tab[name]
