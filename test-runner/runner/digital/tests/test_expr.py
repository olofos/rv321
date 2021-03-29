from runner.digital.parser import parser
from runner.digital.evaluated import Context as EvalContext
import runner.digital.expr as expr

import pytest
import random

def parse_expr(input):
    result = parser.parse('let a = ' + input + ';\n')
    return result.stmts[0].expr

def eval_expr(input, vars = {}):
    result = parser.parse('let a = ' + input + ';\n')
    ctx = EvalContext()
    ctx.vars = vars
    result.eval(ctx)
    return ctx.get_variable('a')

@pytest.mark.parametrize('op', ['+', '-', '*', '/', '%', '&', '|', '^', '=', '<=', '>=', '!=', '<<', '>>'])
def test_parse_binop(op):
    result = parse_expr('1' + op + '2')
    assert type(result) is expr.BinOp
    assert result.op == op
    assert type(result.left) is expr.Number
    assert type(result.right) is expr.Number
    assert result.left.value == 1
    assert result.right.value == 2

@pytest.mark.parametrize('op', ['-', '~', '!'])
def test_parse_unaryop(op):
    result = parse_expr(op + '2')
    assert type(result) is expr.UnaryOp
    assert result.op == op
    assert type(result.expr) is expr.Number
    assert result.expr.value == 2

@pytest.mark.parametrize('input,value', [('0',0), ('2',2), ('0xFF',255), ('0b101010',42), ('017',15)])
def test_parse_number(input, value):
    result = parse_expr(input)
    assert type(result) is expr.Number
    assert result.value == value

def test_parse_variable():
    result = parse_expr('a')
    assert type(result) is expr.Variable
    assert result.name == 'a'

def test_parse_function():
    result = parse_expr('f(1,2+2,a)')
    assert type(result) is expr.Function
    assert len(result.params) == 3
    assert type(result.params[0]) == expr.Number
    assert type(result.params[1]) == expr.BinOp
    assert type(result.params[2]) == expr.Variable

@pytest.mark.parametrize('input,expected', [
    ('1+2',3), ('1-2',-1), ('2-1',1),
    ('3*7',21), ('22/7',3), ('22%7',1),
    ('127&13',13), ('32|64',96), ('127^13',114),
    ('1=2',0), ('2=2',1), ('2=1',0),
    ('1!=2',1), ('2!=2',0), ('2!=1',1),
    ('1>2',0), ('2>2',0), ('2>1',1),
    ('1>=2',0), ('2>=2',1), ('2>=1',1),
    ('1<2',1), ('2<2',0), ('2<1',0),
    ('1<=2',1), ('2<=2',1), ('2<=1',0),

    ('!0',1), ('!1',0), ('!2',0),
    ('-0',0), ('-1',-1), ('-2',-2),

    ('3+4*5',23), ('(3+4)*5', 35),
    ('1+2+3',6),

    ('1<<3',8), ('8>>3',1), ('8>>4',0),

    ('(~0b10101010)&0xFF',0b01010101),
    ('-1+2',1),
])
def test_eval_expr(input, expected):
    result = eval_expr(input)
    assert result == expected

def test_eval_variable():
    result = eval_expr('b', {'b':42})
    assert result == 42

@pytest.mark.parametrize('seed,value', [(1,137),(42,654)])
def test_eval_random(seed, value):
    random.seed(seed)
    result = eval_expr('random(1000)')
    assert result == value
