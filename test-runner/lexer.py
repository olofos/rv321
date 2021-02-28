import ply.lex as lex
import re
import fileinput

literals = [ '&', '|', '^', '~', '+', '-', '*', '(', ')', ',', '=', '>', '<', '/', '%', ';', '!', ]
reserved = { 'end': 'END', 'loop': 'LOOP', 'let': 'LET', 'repeat': 'REPEAT', 'bits': 'BITS', 'x': 'X', 'z': 'Z' }

tokens = [
    'ID',
    'NUMBER_DEC',
    'NUMBER_HEX',
    'NUMBER_BIN',
    'NUMBER_OCT',
    'SHIFTLEFT',
    'SHIFTRIGHT',
    'NOT_EQUAL',
    'GREATER_EQUAL',
    'SMALLER_EQUAL',
    'EOL',
] + list(reserved.values())

def t_ID(t):
     r'[a-zA-Z_][a-zA-Z_0-9]*'
     t.type = reserved.get(t.value.lower(),'ID')    # Check for reserved words
     if t.type != 'ID':
         t.value = t.value.lower()
     return t

def t_NUMBER_DEC(t):
    r'[1-9][0-9]*'
    t.value = int(t.value, 10)
    return t

def t_NUMBER_HEX(t):
    r'0x[0-9a-fA-F]+'
    t.value = int(t.value, 16)
    return t

def t_NUMBER_BIN(t):
    r'0b[01]+'
    t.value = int(t.value, 2)
    return t

def t_NUMBER_OCT(t):
    r'0[0-7]*'
    t.value = int(t.value, 8)
    return t

t_SHIFTLEFT = r'<<'
t_SHIFTRIGHT = r'>>'
t_GREATER_EQUAL = r'>='
t_SMALLER_EQUAL = r'<='
t_NOT_EQUAL = r'!='
t_ignore_COMMENT = r'#[^\n]*'

def t_EOL(t):
    r'\n'
    t.lexer.lineno += len(t.value)
    return t

def t_error(t):
    print("Illegal character '%s'" % t.value[0])
    t.lexer.skip(1)

t_ignore  = ' \t'

lexer = lex.lex(reflags=re.IGNORECASE)

if __name__ == "__main__":
    input = ''
    for line in fileinput.input():
        input += line

    lexer.input(input)

    while True:
        tok = lexer.token()
        if not tok:
            break
        print(tok)
