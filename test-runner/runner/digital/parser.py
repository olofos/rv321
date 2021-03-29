import ply.yacc as yacc
import re

from .lexer import tokens

from . import stmt
from . import data
from . import expr

def p_start(p):
    '''start : stmt_list'''
    p[0] = stmt.List(p[1])
    return p

def p_stmt_list(p):
    '''stmt_list : stmt_list stmt
                 | stmt'''
    if len(p) == 2:
        p[0] = [p[1]] if p[1] else []
    else:
        p[0] = p[1]
        if p[2]:
            p[0].append(p[2])
    return p

def p_stmt_let(p):
    '''stmt : LET ID '=' expr ';' EOL '''
    p[0] = stmt.Let(p.lineno(3), p[2], p[4])
    return p

def p_stmt_loop(p):
    '''stmt : LOOP '(' ID ',' number ')' EOL stmt_list END LOOP EOL'''
    p[0] = stmt.Loop(p.lineno(2), p[3], p[5].value, stmt.List(p[8]))
    return p

def p_stmt_repeat(p):
    '''stmt : REPEAT '(' number ')' data_row EOL'''
    p[0] = stmt.Repeat(p.lineno(2), p[3].value, p[5])
    return p

def p_stmt_data_row(p):
    '''stmt : data_row EOL'''
    p[0] = stmt.DataRow(p.lineno(2), p[1])
    return p

def p_stmt_empty(p):
    '''stmt : EOL '''
    pass

def p_data_row(p):
    '''data_row : data_row data_entry
                | data_entry'''
    if len(p) == 2:
        p[0] = [p[1]]
    else:
        p[0] = p[1]
        p[0].append(p[2])
    return p

def p_data_entry_number(p):
    '''data_entry : number'''
    p[0] = data.Number(p[1].value)
    return p

def p_data_entry_expr(p):
    '''data_entry : '(' expr ')' '''
    p[0] = data.Expr(p[2])
    return p

def p_data_entry_bits(p):
    '''data_entry : BITS '(' number ',' expr ')' '''
    p[0] = data.Bits(p[3], p[5])
    return p

def p_data_entry_id(p):
    '''data_entry : X
                  | Z'''
    p[0] = data.Id(p[1].upper())
    return p

def p_expr(p):
    '''expr        : or_expr '=' or_expr
                   | or_expr NOT_EQUAL or_expr
                   | or_expr '<' or_expr
                   | or_expr '>' or_expr
                   | or_expr SMALLER_EQUAL or_expr
                   | or_expr GREATER_EQUAL or_expr
                   | or_expr
       or_expr     : or_expr '|' xor_expr
                   | xor_expr
       xor_expr    : xor_expr '^' and_expr
                   | and_expr
       and_expr    : and_expr '&' shift_expr
                   | shift_expr
       shift_expr  : arithm_expr SHIFTLEFT arithm_expr
                   | arithm_expr SHIFTRIGHT arithm_expr
                   | arithm_expr
       arithm_expr : arithm_expr '+' term
                   | arithm_expr '-' term
                   | term
       term        : term '*' factor
                   | term '/' factor
                   | term '%' factor
                   | factor
       factor      : '-' factor
                   | '~' factor
                   | '!' factor
                   | number
                   | function
                   | variable'''

    if len(p) == 2:
        p[0] = p[1]
    elif len(p) == 3:
        p[0] = expr.UnaryOp(p[1], p[2])
    elif len(p) == 4:
        p[0] = expr.BinOp(p[1],p[2],p[3])
    return p

def p_factor_bracket(p):
    '''factor : '(' expr ')' '''
    p[0] = p[2]
    return p

def p_number(p):
    '''number : NUMBER_DEC
              | NUMBER_HEX
              | NUMBER_BIN
              | NUMBER_OCT'''
    p[0] = expr.Number(p[1])
    return p

def p_function(p):
    '''function : ID '(' expr_list ')' '''
    p[0] = expr.Function(p[1], p[3])
    return p

def p_variable(p):
    '''variable : ID'''
    p[0] = expr.Variable(p[1])
    return p

def p_expr_list(p):
    '''expr_list : expr_list ',' expr
                 | expr'''
    if len(p) == 2:
        p[0] = [p[1]]
    else:
        p[0] = p[1]
        p[0].append(p[3])
    return p

# Error rule for syntax errors
def p_error(p):
    line   = p.lineno        # line number of the PLUS token
    index  = p.lexpos        # Position of the PLUS token
    print("Syntax error at line {0}, index {1}".format(line, index))

# Build the parser
parser = yacc.yacc()

def parse(input):
    # Remove the signals line
    input = re.sub(r'[^ \t\f\r\n][^\n]*\n', '\n', input, count=1)
    return parser.parse(input)
