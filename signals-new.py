from itertools import groupby
from itertools import accumulate
from functools import reduce
import sys

addGaps = True
removeConst = True
removeCommon = True
hideHidden = True

# addGaps = False
# removeConst = False
removeCommon = False
# hideHidden = False

stepBits = 6

# Signals

CLK_COUNT = 'CLK_COUNT'
CLK = 'CLK'

ALU_N = 'ALU_N'
ALU_RESET = '\\neg{ALU_RESET}'
ALU_FLAG_LATCH = 'ALU_FLAG_LATCH'
ALU_SHIFT_STEP = '\\neg{ALU_SHIFT_STEP}'
ALU_AUX = 'ALU_AUX'
ALU_OP = 'ALU_OP'

STEP_LEN = 'STEP_LEN'
LENGTH_FIELD = 'LENGTH_FIELD'
LAST_STEP = 'LAST_STEP'

REG_IN_EN = '\\neg{REG_IN_EN}'
BUS_EN = 'BUS_EN'

PC_IN_LATCH = 'PC_IN_LATCH'
PC_OUT_LATCH = 'PC_OUT_LATCH'
PC_OUT_SP = 'PC_OUT_S\\neg{P}'
PC_CNT_PE = '\\neg{PC_CNT_PE}'
PC_CNT_EN = 'PC_CNT_EN'

ADDR_LATCH = 'ADDR_LATCH'
ADDR_CLK = 'ADDR_CLK'
ADDR_PE = '\\neg{ADDR_PE}'

MEM_OE = '\\neg{MEM_OE}'
MEM_WE = '\\neg{MEM_WE}'

OP_LATCH = 'OP_LATCH'
IMM_LATCH = 'IMM_LATCH'
IMM_SP = 'IMM_S\\neg{P}'

ALU_A_MUX = 'ALU_A_MUX'
ALU_B_MUX = 'ALU_B_MUX'
IMM_MUX = 'IMM_MUX'
REG_IN_MUX = 'REG_IN_MUX'
PC_IN_MUX = 'PC_IN_MUX'
OP_IN_MUX = 'OP_IN_MUX'

CSR_ADDR_LATCH = 'CSR_ADDR_LATCH'
CSR_OUT_LATCH = 'CSR_OUT_LATCH'
CSR_IN_LATCH = 'CSR_IN_LATCH'
CSR_OP = 'CSR_OP'
CSR_IN_MUX = 'CSR_IN_MUX'
SLEEP = 'SLEEP'
ILL_OP = '\\neg{ILL_OP}'

BUS_CLK = 'BUS_CLK'
INV_CLK = '\\neg{CLK}'

LAST_SUBSTEP = 'LAST_SUBSTEP'
STEP = 'STEP'
SUBSTEP = 'SUBSTEP'
STEP_DONE = '\\neg{STEP_DONE}'
OP_DONE = '\\neg{OP_DONE}'

META_SECTION = 'META_SECTION'
META_COMMENT = 'META_COMMENT'


signalDefaults = {
    CLK_COUNT: -1,
    CLK: 1,
    INV_CLK: 0,

    ALU_N: 0,
    ALU_RESET: 1,
    ALU_FLAG_LATCH: 1,
    ALU_SHIFT_STEP: 1,
    ALU_AUX: 1,

    REG_IN_EN: 1,
    BUS_EN: 0,

    PC_IN_LATCH: 0,
    PC_OUT_LATCH: 0,
    PC_OUT_SP: 0,
    PC_CNT_PE: 1,
    PC_CNT_EN: 0,

    ADDR_LATCH: 0,
    ADDR_CLK: 0,
    ADDR_PE: 0,

    MEM_OE: 1,
    MEM_WE: 1,

    OP_LATCH: 0,
    IMM_LATCH: 0,
    IMM_SP: 0,

    ALU_OP: 'U',
    ALU_A_MUX: 'U',
    ALU_B_MUX: 'U',
    IMM_MUX: 'U',
    REG_IN_MUX: 'U',
    PC_IN_MUX: 'U',
    OP_IN_MUX: 'U',

    CSR_ADDR_LATCH: 1,
    CSR_OUT_LATCH: 0,
    CSR_IN_LATCH: 1,
    CSR_OP: 'U',
    CSR_IN_MUX: 'U',
    SLEEP: 0,
    ILL_OP: 1,

    BUS_CLK: 0,

    LAST_SUBSTEP: 0,
    STEP_DONE: 0,
    LAST_STEP: 0,
    OP_DONE: 1,

    LENGTH_FIELD: 0,
    STEP_LEN: 1,
    STEP: -1,
    SUBSTEP: 0,

    META_SECTION: 'Fetch Opcode',
    META_COMMENT: {}
}


signalValues = {
    LENGTH_FIELD:   {0:      0b00000000_00000000_00000000_00000000,
                     1:      0b00000000_00000000_00000000_00000001,
                     2:      0b00000000_00000000_00000000_00000010,
                     3:      0b00000000_00000000_00000000_00000011,
    },
    SLEEP:          {0: 0,
                     1:      0b00000000_00000000_00000000_00000100},
    OP_LATCH:       {0: 0,
                     1:      0b00000000_00000000_00000000_00001000},
    PC_IN_LATCH:    {0: 0,
                     1:      0b00000000_00000000_00000000_00010000},
    PC_OUT_LATCH:   {0: 0,
                     1:      0b00000000_00000000_00000000_00100000},
    ILL_OP:         {0: 0,
                     1:      0b00000000_00000000_00000000_01000000},
    ADDR_LATCH:     {0: 0,
                     1:      0b00000000_00000000_00000000_10000000},
    ADDR_CLK:       {0: 0,
                     1:      0b00000000_00000000_00000001_00000000},
    MEM_OE:         {0: 0,
                     1:      0b00000000_00000000_00000010_00000000},
    MEM_WE:         {0: 0,
                     1:      0b00000000_00000000_00000100_00000000},
    REG_IN_EN:      {0: 0,
                     1:      0b00000000_00000000_00001000_00000000},
    ALU_N:          {0: 0,
                     1:      0b00000000_00000000_00010000_00000000},
    ALU_OP:         {'U':0,
                     'ADD':  0b00000000_00000000_00000000_00000000,
                     'OR':   0b00000000_00000000_00100000_00000000,
                     'XOR':  0b00000000_00000000_01000000_00000000,
                     'AND':  0b00000000_00000000_01100000_00000000,
                     'SLTU': 0b00000000_00000000_10000000_00000000,
                     'SLT':  0b00000000_00000000_10100000_00000000,
                     'SL':   0b00000000_00000000_11000000_00000000,
                     'SA':   0b00000000_00000000_11100000_00000000},
    ALU_AUX:        {0: 0,
                     1:      0b00000000_00000001_00000000_00000000},
    ALU_RESET:      {0: 0,
                     1:      0b00000000_00000010_00000000_00000000},

    CSR_ADDR_LATCH: {0: 0,
                     1:      0b00000000_00100000_00000000_00000000},
    CSR_OUT_LATCH:  {0: 0,
                     1:      0b00000000_01000000_00000000_00000000},
    CSR_IN_LATCH:   {0: 0,
                     1:      0b00000000_10000000_00000000_00000000},

    PC_IN_MUX:      {'U': 0,
                     'ALU':  0b00000000_00000000_00000000_00000000,
                     'CSR':  0b00000001_00000000_00000000_00000000},
    ALU_A_MUX:      {'U': 0,
                     'RS1':  0b00000000_00000000_00000000_00000000,
                     'PC':   0b00000010_00000000_00000000_00000000,
                     'Zero': 0b00000100_00000000_00000000_00000000,
                     'One':  0b00000110_00000000_00000000_00000000},
    IMM_MUX:      {'U': 0,
                     'RS2':  0b00000000_00000000_00000000_00000000,
                     'ImmCSR': 0b00000000_00000000_00000000_00000000,
                     'ImmI': 0b00001000_00000000_00000000_00000000,
                     'ImmS': 0b00010000_00000000_00000000_00000000,
                     'ImmB': 0b00011000_00000000_00000000_00000000,
                     'ImmU': 0b00100000_00000000_00000000_00000000,
                     'ImmJ': 0b00101000_00000000_00000000_00000000,
                     'Zero': 0b00110000_00000000_00000000_00000000,
                     'One':  0b00111000_00000000_00000000_00000000},
    REG_IN_MUX:     {'U': 0,
                     'ALU':  0b00000000_00000000_00000000_00000000,
                     'MEM':  0b01000000_00000000_00000000_00000000,
                     'CSR':  0b10000000_00000000_00000000_00000000,
                     'PC':   0b11000000_00000000_00000000_00000000},
}

busSignals = [
    CLK_COUNT,
    STEP,
    STEP_LEN,
    SUBSTEP,
    LENGTH_FIELD,
    ALU_OP,

    ALU_A_MUX,
    ALU_B_MUX,
    IMM_MUX,
    REG_IN_MUX,
    PC_IN_MUX,
    OP_IN_MUX,

    CSR_OP,
    CSR_IN_MUX,

    META_SECTION,
    META_COMMENT,
]

clockSignals = [
    CLK,
    INV_CLK,
    BUS_CLK,
]

counterSignals = [
    CLK_COUNT,
    SUBSTEP,
]

hiddenSignals = [
    PC_OUT_SP,
    PC_CNT_EN,
    PC_CNT_PE,
    IMM_LATCH,
    LENGTH_FIELD,
] if hideHidden else []

metaSignals = [
    META_SECTION,
    META_COMMENT,
]

specialSignals = list(filter(lambda s: s not in hiddenSignals, clockSignals + counterSignals))

def handleIdentifications(signal):
    signal[INV_CLK] = ~signal[CLK] & 1
    signal[BUS_CLK] = signal[CLK] & signal[BUS_EN]
    signal[LAST_SUBSTEP] = 1 if (signal[SUBSTEP] == signal[STEP_LEN] - 1) else 0
    signal[PC_OUT_SP] = signal[BUS_EN]
    signal[PC_CNT_EN] = signal[OP_LATCH]
    signal[IMM_SP] = signal[BUS_EN]
    signal[PC_CNT_PE] = ~signal[PC_IN_LATCH] & 1
    signal[ALU_AUX] = signal[ALU_FLAG_LATCH] & signal[ALU_SHIFT_STEP]
    signal[ALU_B_MUX] = 'U' if signal[IMM_MUX] == 'U' else 'RS2' if signalValues[IMM_MUX][signal[IMM_MUX]] == 0 else signal[IMM_MUX]

    if signal[BUS_EN] == 1:
        if signal[STEP_LEN] == 4:
            signal[LENGTH_FIELD] = 3
        elif signal[STEP_LEN] == 1:
            signal[LENGTH_FIELD] = 2
        else:
            raise Exception('STEP_LEN should be 1 or 4')
        if signal[LAST_STEP] == 1:
            raise Exception('Last step should have length 1')
    else:
        if signal[STEP_LEN] == 1:
            if signal[LAST_STEP] == 1:
                signal[LENGTH_FIELD] = 0
            else:
                signal[LENGTH_FIELD] = 1
        else:
            raise Exception('STEP_LEN should be 1 when bus is disabled')


def onClockUp(prev, next):
    next[STEP_DONE] = ~prev[LAST_SUBSTEP] & 1
    next[OP_DONE] = ~(prev[LAST_STEP] & prev[LAST_SUBSTEP]) & 1
    next[IMM_LATCH] = prev[OP_LATCH]
    next[ADDR_PE] = prev[ADDR_LATCH]

def onClockDown(prev, next):
    next[CLK_COUNT] = prev[CLK_COUNT] + 1
    if prev[STEP_DONE] == 0:
        next[SUBSTEP] = 0
        next[STEP] = prev[STEP] + 1
    else:
        next[SUBSTEP] = prev[SUBSTEP] + 1

def step(op, prev, next):
    if(prev[CLK] == 0):
        next[CLK] = 1
        onClockUp(prev, next)
    else:
        next[CLK] = 0
        onClockDown(prev, next)

        for k, v in op[next[STEP]].items():
            next[k] = v

    handleIdentifications(next)


opFetch = [
    { ADDR_LATCH: 0, PC_OUT_LATCH: 0, ALU_A_MUX: 'PC', IMM_MUX: 'Zero', ALU_OP: 'OR', BUS_EN: 1, STEP_LEN: 4, META_SECTION: 'Fetch', META_COMMENT: {ALU_A_MUX: 'PC $\\to$ Addr'} },
    { BUS_EN: 1, STEP_LEN: 4 },
    { BUS_EN: 1, STEP_LEN: 4 },
    { BUS_EN: 1, STEP_LEN: 4 },
    { BUS_EN: 1, STEP_LEN: 4 },
    { BUS_EN: 1, STEP_LEN: 4 },
    { BUS_EN: 1, STEP_LEN: 4 },
    { BUS_EN: 1, STEP_LEN: 4 },
    { ADDR_LATCH: 1, ALU_A_MUX: 'U', IMM_MUX: 'U', ALU_OP: 'U', BUS_EN: 0, STEP_LEN: 1},
    { ADDR_LATCH: 0, ADDR_CLK: 1, BUS_EN: 0, STEP_LEN: 1 },
    { ADDR_CLK: 0, MEM_OE: 0, BUS_EN: 0, STEP_LEN: 1 },
    { MEM_OE: 1, OP_IN_MUX: 'MEM', BUS_EN: 1, STEP_LEN: 4, META_COMMENT: {MEM_OE: '[Addr] $\\to$ Opcode \\#0'} },
    { ADDR_CLK: 1, BUS_EN: 1, STEP_LEN: 4 },
    { ADDR_CLK: 0, MEM_OE: 0, BUS_EN: 0, STEP_LEN: 1 },
    { MEM_OE: 1, BUS_EN: 1, STEP_LEN: 4, META_COMMENT: {MEM_OE: '[Addr+1] $\\to$ Opcode \\#1'} },
    { ADDR_CLK: 1, BUS_EN: 1, STEP_LEN: 4 },
    { ADDR_CLK: 0, MEM_OE: 0, BUS_EN: 0, STEP_LEN: 1 },
    { MEM_OE: 1, BUS_EN: 1, STEP_LEN: 4, META_COMMENT: {MEM_OE: '[Addr+2] $\\to$ Opcode \\#2'} },
    { ADDR_CLK: 1, BUS_EN: 1, STEP_LEN: 4 },
    { ADDR_CLK: 0, MEM_OE: 0, BUS_EN: 0, STEP_LEN: 1 },
    { MEM_OE: 1, BUS_EN: 1, STEP_LEN: 4, META_COMMENT: {MEM_OE: '[Addr+3] $\\to$ Opcode \\#3'} },
    { BUS_EN: 1, STEP_LEN: 4 },
    { OP_LATCH: 1, OP_IN_MUX: 'U', BUS_EN: 0, STEP_LEN: 1 },
]

def simpleBinCommon(op):
    A = 'PC' if op == 'AUIPC' else 'Zero' if op == 'LUI' else 'RS1'
    B = 'ImmU' if op in ['LUI', 'AUIPC'] else 'ImmI' if 'I' in op else 'RS2'
    aluOp = 'ADD' if op in ['SUB', 'LUI', 'AUIPC'] else op[0:-1] if op[-1] == 'I' else op
    sign = 1 if op == 'SUB' else 0
    opSign = '-' if op == 'SUB' else '+' if aluOp == 'ADD' else '\\&' if aluOp == 'AND' else '|' if aluOp == 'OR' else '\\wedge'

    return [
        { ALU_RESET: 0, ALU_N: sign, OP_LATCH: 0, BUS_EN: 0, STEP_LEN: 1, META_SECTION: op},
        { ALU_RESET: 1, REG_IN_EN: 0, ALU_OP: aluOp, ALU_A_MUX: A, IMM_MUX: B, REG_IN_MUX: 'ALU',  BUS_EN: 1, STEP_LEN: 4, META_COMMENT: {REG_IN_EN: '%s $%s$ %s $\\to$ RD' % (A, opSign, B)}},
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { REG_IN_EN: 1, ALU_N: 0, ALU_OP: 'U', ALU_A_MUX: 'U', IMM_MUX: 'U', REG_IN_MUX: 'U', LAST_STEP: 1, PC_OUT_LATCH: 1, BUS_EN: 0, STEP_LEN: 1, },
        { BUS_EN: 0, STEP_LEN: 1}
    ]

def setLowerCommon(op):
    aluOp = 'SLTU' if 'U' in op else 'SLT'
    B = 'ImmI' if 'I' in op else 'RS2'
    return [
        { ALU_RESET: 0, ALU_N: 1, OP_LATCH: 0, BUS_EN: 0, STEP_LEN: 1, META_SECTION: op + ' (Common)'},
        { ALU_RESET: 1, ALU_FLAG_LATCH: 0, ALU_OP: aluOp, ALU_A_MUX: 'RS1', IMM_MUX: B,  BUS_EN: 1, STEP_LEN: 4,},
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { ALU_FLAG_LATCH: 1,  ALU_N: 0, ALU_RESET: 0, ALU_OP: 'U', ALU_A_MUX: 'U', IMM_MUX: 'U', BUS_EN: 0, STEP_LEN: 1 },
    ]

def setLowerTrue(op):
    return setLowerCommon(op) + [
        { ALU_RESET: 1, REG_IN_EN: 0, ALU_A_MUX: 'Zero', IMM_MUX: 'One', ALU_OP: 'OR', REG_IN_MUX: 'ALU', BUS_EN: 1, STEP_LEN: 1, META_SECTION: op },
        { IMM_MUX: 'Zero', BUS_EN: 1, STEP_LEN: 1 },
        { BUS_EN: 1, STEP_LEN: 1 },
        { BUS_EN: 1, STEP_LEN: 1 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { IMM_MUX: 'Zero', BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { REG_IN_EN: 1, ALU_A_MUX: 'U', IMM_MUX: 'U', ALU_OP: 'U', REG_IN_MUX: 'U', LAST_STEP: 1, PC_OUT_LATCH: 1, BUS_EN: 0, STEP_LEN: 1 },
        { BUS_EN: 0, STEP_LEN: 1 }
    ]

def setLowerFalse(op):
    return setLowerCommon(op) + [
        { REG_IN_EN: 0, ALU_A_MUX: 'Zero', IMM_MUX: 'Zero', ALU_OP: 'OR', REG_IN_MUX: 'ALU', BUS_EN: 1, STEP_LEN: 4, META_SECTION: op },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { REG_IN_EN: 1, ALU_A_MUX: 'U', IMM_MUX: 'U', ALU_OP: 'U', REG_IN_MUX: 'U', LAST_STEP: 1, PC_OUT_LATCH: 1, BUS_EN: 0, STEP_LEN: 1 },
        { BUS_EN: 0, STEP_LEN: 1 }
    ]

def loadCommon(op):
    len = 8 if op[0:2] == 'LB' else 16 if op[0:2] == 'LH' else 32
    result = [
        { ALU_RESET: 0, OP_LATCH: 0, BUS_EN: 0, STEP_LEN: 1, META_SECTION: op},
        { ADDR_LATCH: 0, ALU_RESET: 1, ALU_OP: 'ADD', ALU_A_MUX: 'RS1', IMM_MUX: 'ImmI', BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { ADDR_LATCH: 1, ALU_OP: 'U', ALU_A_MUX: 'U', IMM_MUX: 'U', BUS_EN: 0, STEP_LEN: 1 },
        { ADDR_LATCH: 0, ADDR_CLK: 1, BUS_EN: 0, STEP_LEN: 1 },
        { ADDR_CLK: 0, MEM_OE: 0, BUS_EN: 0, STEP_LEN: 1 },
        { MEM_OE: 1, REG_IN_EN: 0, REG_IN_MUX: 'MEM', BUS_EN: 1, STEP_LEN: 4 },
    ]

    if len > 8:
        result += [
            { ADDR_CLK: 1, BUS_EN: 1, STEP_LEN: 4 },
            { ADDR_CLK: 0, MEM_OE: 0, BUS_EN: 0, STEP_LEN: 1 },
            { MEM_OE: 1, REG_IN_EN: 0, REG_IN_MUX: 'MEM', BUS_EN: 1, STEP_LEN: 4 },
        ]
    else:
        result += [
            { ADDR_CLK: 0, BUS_EN: 1, STEP_LEN: 4 },
            { ADDR_CLK: 0, BUS_EN: 1, STEP_LEN: 4 },
        ]

    if len > 16:
        result += [
            { ADDR_CLK: 1, BUS_EN: 1, STEP_LEN: 4 },
            { ADDR_CLK: 0, MEM_OE: 0, BUS_EN: 0, STEP_LEN: 1 },
            { MEM_OE: 1, REG_IN_EN: 0, REG_IN_MUX: 'MEM', BUS_EN: 1, STEP_LEN: 4 },
            { ADDR_CLK: 1, BUS_EN: 1, STEP_LEN: 4 },
            { ADDR_CLK: 0, MEM_OE: 0, BUS_EN: 0, STEP_LEN: 1 },
            { MEM_OE: 1, REG_IN_EN: 0, REG_IN_MUX: 'MEM', BUS_EN: 1, STEP_LEN: 4 },
            { BUS_EN: 1, STEP_LEN: 4 },
        ]
    else:
        result += [
            { ADDR_CLK: 0, BUS_EN: 1, STEP_LEN: 4 },
            { ADDR_CLK: 0, BUS_EN: 1, STEP_LEN: 4 },
            { BUS_EN: 1, STEP_LEN: 4 },
            { BUS_EN: 1, STEP_LEN: 4 },
            { BUS_EN: 1, STEP_LEN: 4 },
        ]

    result += [
        { LAST_STEP: 1, PC_OUT_LATCH: 1, BUS_EN: 0, STEP_LEN: 1 },
        { BUS_EN: 0, STEP_LEN: 1},
    ]

    return result

def storeCommon(op):
    len = 8 if op == 'SB' else 16 if op == 'SH' else 32

    result = [
        { ALU_RESET: 0, OP_LATCH: 0, BUS_EN: 0, STEP_LEN: 1, META_SECTION: op},
        { ADDR_LATCH: 0, ALU_RESET: 1, ALU_OP: 'ADD', ALU_A_MUX: 'RS1', IMM_MUX: 'ImmS', BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { ALU_OP: 'U', ALU_A_MUX: 'U', IMM_MUX: 'U', ADDR_LATCH: 1, BUS_EN: 1, STEP_LEN: 4 },
        { ADDR_LATCH: 0, ADDR_CLK: 1, BUS_EN: 1, STEP_LEN: 4 },
        { ADDR_CLK: 0, MEM_WE: 0, BUS_EN: 0, STEP_LEN: 1 }
    ]

    if len > 8:
        result += [
            { MEM_WE: 1, BUS_EN: 1, STEP_LEN: 4 },
            { ADDR_CLK: 1, BUS_EN: 1, STEP_LEN: 4 },
            { ADDR_CLK: 0, MEM_WE: 0, BUS_EN: 0, STEP_LEN: 1 }
        ]
    else:
        result += [
            { BUS_EN: 1, STEP_LEN: 4 },
            { BUS_EN: 1, STEP_LEN: 4 },
        ]

    if len > 16:
        result += [
            { MEM_WE: 1, BUS_EN: 1, STEP_LEN: 4 },
            { ADDR_CLK: 1, BUS_EN: 1, STEP_LEN: 4 },
            { ADDR_CLK: 0, MEM_WE: 0, BUS_EN: 0, STEP_LEN: 1 },
            { MEM_WE: 1, BUS_EN: 1, STEP_LEN: 4 },
            { ADDR_CLK: 1, BUS_EN: 1, STEP_LEN: 4 },
            { ADDR_CLK: 0, MEM_WE: 0, BUS_EN: 0, STEP_LEN: 1 },
        ]
    else:
        result += [
            { BUS_EN: 1, STEP_LEN: 4 },
            { BUS_EN: 1, STEP_LEN: 4 },
            { BUS_EN: 1, STEP_LEN: 4 },
            { BUS_EN: 1, STEP_LEN: 4 },
        ]


    result += [
        { MEM_WE: 1, LAST_STEP: 1, PC_OUT_LATCH: 1, BUS_EN: 0, STEP_LEN: 1 },
        { BUS_EN: 0, STEP_LEN: 1},
    ]


    return result

def branchCommon(op):
    aluOp = 'XOR' if op in ['BEQ', 'BNE'] else 'SLT' if op in ['BLT', 'BGE'] else 'SLTU' if op in ['BLTU', 'BGEU'] else 'ERROR'

    return [
        { ALU_RESET: 0, ALU_N: 1, OP_LATCH: 0, BUS_EN: 0, STEP_LEN: 1, META_SECTION: op + ' [Test]' },
        { ALU_RESET: 1, ALU_FLAG_LATCH: 0, ALU_OP: aluOp, ALU_A_MUX: 'RS1', IMM_MUX: 'RS2',  BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { ALU_N: 0, ALU_FLAG_LATCH: 1, ALU_OP: 'U', ALU_A_MUX: 'U', IMM_MUX: 'U', BUS_EN: 0, STEP_LEN: 1 },
    ]

def branchNotTaken(op):
    return branchCommon(op) + [
        { LAST_STEP: 1, PC_OUT_LATCH: 1, BUS_EN: 0, STEP_LEN: 1, META_SECTION: op + ' [Not taken]' },
        { BUS_EN: 0, STEP_LEN: 1 },
    ]

def branchTaken(op):
    return branchCommon(op) + [
        { ALU_RESET: 0, BUS_EN: 0, STEP_LEN: 1, META_SECTION: op + ' [Taken]' },
        { ALU_RESET: 1, ALU_OP: 'ADD', ALU_A_MUX: 'PC', IMM_MUX: 'ImmB', PC_IN_MUX: 'ALU', BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { PC_IN_LATCH: 1, ALU_OP: 'U', ALU_A_MUX: 'U', IMM_MUX: 'U', PC_IN_MUX: 'U', BUS_EN: 0, STEP_LEN: 1, },
        { PC_IN_LATCH: 0, LAST_STEP: 1, PC_OUT_LATCH: 1, BUS_EN: 0, STEP_LEN: 1 },
        { BUS_EN: 0, STEP_LEN: 1 },
    ]

def jumpCommon(op):
    A = 'PC' if op == 'JAL' else 'RS1' if op == 'JALR' else 'ERROR'
    B = 'ImmJ' if op == 'JAL' else 'ImmI' if op == 'JALR' else 'ERROR'

    return [
        { ALU_RESET: 0, OP_LATCH: 0, BUS_EN: 0, STEP_LEN: 1, META_SECTION: op},
        # Store the new PC = PC + J in the incoming shift register
        { ALU_RESET: 1, ALU_OP: 'ADD', ALU_A_MUX: A, IMM_MUX: B, PC_IN_MUX: 'ALU', BUS_EN: 1, STEP_LEN: 4, META_COMMENT: {
            PC_IN_MUX: 'PC${}_{\\mbox{out}}$ $+$ J $\\to$ PC${}_{\\mbox{in}}$' if op == 'JAL' else 'RS1 $+$ I $\\to$ PC${}_{\\mbox{in}}$'
        }},
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        # Latch increased PC from counter to outgoing shift register
        { PC_OUT_LATCH: 1, ALU_RESET: 0, ALU_A_MUX: 'U', IMM_MUX: 'U', PC_IN_MUX: 'U', BUS_EN: 0, STEP_LEN: 1},
        { PC_OUT_LATCH: 0, PC_IN_LATCH: 1, BUS_EN: 0, STEP_LEN: 1},
        # Output increased PC to RD
        { PC_IN_LATCH: 0, ALU_RESET: 1, REG_IN_EN: 0, REG_IN_MUX: 'ALU', ALU_A_MUX: 'PC', IMM_MUX: 'Zero', ALU_OP: 'OR', BUS_EN: 1, STEP_LEN: 4, META_COMMENT: {REG_IN_MUX: 'PC $\\to$ RD' }},
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { REG_IN_EN: 1, REG_IN_MUX: 'U', ALU_A_MUX: 'U', IMM_MUX: 'U', ALU_OP: 'U', LAST_STEP: 1, PC_OUT_LATCH: 1, BUS_EN: 0, STEP_LEN: 1 },
        { BUS_EN: 0, STEP_LEN: 1}
    ]

def shiftCommon(op):
    aluOp = 'SA' if 'A' in op else 'SL'
    B = 'ImmI' if 'I' in op else 'RS2'
    n = 1 if 'R' in op else 0

    return [
        { ALU_RESET: 0, ALU_N: n, OP_LATCH: 0, BUS_EN: 0, ALU_SHIFT_STEP: 1, STEP_LEN: 1, META_SECTION: op},
        { ALU_RESET: 1, ALU_OP: aluOp, ALU_A_MUX: 'One', IMM_MUX: B, BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 1 },
        { ALU_A_MUX: 'Zero', IMM_MUX: 'Zero', BUS_EN: 1, STEP_LEN: 1 },
        { BUS_EN: 1, STEP_LEN: 1 },
        { BUS_EN: 1, STEP_LEN: 1 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { ALU_SHIFT_STEP: 0, BUS_EN: 1, STEP_LEN: 4 },
        { ALU_SHIFT_STEP: 1, ALU_N: 0, ALU_A_MUX: 'U', IMM_MUX: 'U', BUS_EN: 0, STEP_LEN: 1 },
        { ALU_SHIFT_STEP: 0, BUS_EN: 0, STEP_LEN: 1 },
        { ALU_A_MUX: 'RS1', IMM_MUX: 'U', ALU_SHIFT_STEP: 1, BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { ALU_SHIFT_STEP: 0, BUS_EN: 1, STEP_LEN: 4 },
        { ALU_A_MUX: 'U', REG_IN_EN: 0, REG_IN_MUX: 'ALU', ALU_SHIFT_STEP: 1, BUS_EN: 1, STEP_LEN: 4 } ,
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { ALU_OP: 'U', REG_IN_EN: 1,  LAST_STEP: 1, PC_OUT_LATCH: 1, BUS_EN: 0, STEP_LEN: 1 },
        { BUS_EN: 0, STEP_LEN: 1 },
    ]

def csrCommon(op):
    csrOp = 'Swap' if 'CSRRW' in op else 'Set' if 'CSRRS' in op else 'Clear'
    B = 'Imm' if 'I' in op else 'RS1'

    return [
        { OP_LATCH: 0, ALU_OP: 'U', ALU_A_MUX: 'U', IMM_MUX: 'ImmI', CSR_ADDR_LATCH: 0, BUS_EN: 1, STEP_LEN: 4, META_SECTION: op },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { CSR_ADDR_LATCH: 1, BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { CSR_OUT_LATCH: 1, BUS_EN: 0, STEP_LEN: 1},
        { REG_IN_MUX: 'CSR', REG_IN_EN: 0, CSR_OUT_LATCH: 0, CSR_IN_LATCH: 0, IMM_MUX: 'ImmCSR', CSR_OP:csrOp, CSR_IN_MUX: B, BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { REG_IN_MUX: 'U', REG_IN_EN: 1, CSR_IN_LATCH: 1, IMM_MUX: 'U', CSR_OP: 'U', CSR_IN_MUX: 'U', BUS_EN: 0, STEP_LEN: 1, LAST_STEP: 1, PC_OUT_LATCH: 1},
        { BUS_EN: 0, STEP_LEN: 1},
    ]

def nopCommon(op):
    return [
        { OP_LATCH: 0, BUS_EN: 0, STEP_LEN: 1 },
        { BUS_EN: 0, STEP_LEN: 1, LAST_STEP: 1, PC_OUT_LATCH: 1, META_SECTION: op },
        { BUS_EN: 0, STEP_LEN: 1 },
    ]


opcodes = {
    'LUI':   simpleBinCommon('LUI'),
    'AUIPC': simpleBinCommon('AUIPC'),

    'BEQ [Taken]':      branchTaken('BEQ'),
    'BEQ [Not taken]':  branchNotTaken('BEQ'),
    'BNE [Taken]':      branchTaken('BNE'),
    'BNE [Not taken]':  branchNotTaken('BNE'),
    'BLT [Taken]':      branchTaken('BLT'),
    'BLT [Not taken]':  branchNotTaken('BLT'),
    'BGE [Taken]':      branchTaken('BGE'),
    'BGE [Not taken]':  branchNotTaken('BGE'),
    'BLTU [Taken]':     branchTaken('BLTU'),
    'BLTU [Not taken]': branchNotTaken('BLTU'),
    'BGEU [Taken]':     branchTaken('BGEU'),
    'BGEU [Not taken]': branchNotTaken('BGEU'),


    'JAL':  jumpCommon('JAL'),
    'JALR': jumpCommon('JALR'),

    'LB':  loadCommon('LB'),
    'LH':  loadCommon('LH'),
    'LW':  loadCommon('LW'),
    'LBU': loadCommon('LBU'),
    'LHU': loadCommon('LHU'),

    'SB': storeCommon('SB'),
    'SH': storeCommon('SH'),
    'SW': storeCommon('SW'),

    'ADDI': simpleBinCommon('ADDI'),
    'XORI': simpleBinCommon('XORI'),
    'ORI':  simpleBinCommon('ORI'),
    'ANDI': simpleBinCommon('ANDI'),

    'SLLI': shiftCommon('SLLI'),
    'SRLI': shiftCommon('SRLI'),
    'SRAI': shiftCommon('SRAI'),

    'SLL': shiftCommon('SLL'),
    'SRL': shiftCommon('SRL'),
    'SRA': shiftCommon('SRA'),

    'SLTIU [True]':  setLowerTrue('SLTIU'),
    'SLTIU [False]': setLowerFalse('SLTIU'),
    'SLTI [True]':   setLowerTrue('SLTI'),
    'SLTI [False]':  setLowerFalse('SLTI'),

    'ADD': simpleBinCommon('ADD'),
    'SUB': simpleBinCommon('SUB'),
    'XOR': simpleBinCommon('XOR'),
    'OR':  simpleBinCommon('OR'),
    'AND': simpleBinCommon('AND'),

    'SLTU [True]':  setLowerTrue('SLTU'),
    'SLTU [False]': setLowerFalse('SLTU'),
    'SLT [True]':   setLowerTrue('SLT'),
    'SLT [False]':  setLowerFalse('SLT'),

    'CSRRW': csrCommon('CSRRW'),
    'CSRRS': csrCommon('CSRRS'),
    'CSRRC': csrCommon('CSRRC'),
    'CSRRWI': csrCommon('CSRRWI'),
    'CSRRSI': csrCommon('CSRRSI'),
    'CSRRCI': csrCommon('CSRRCI'),

    'IllOp': [
        { OP_LATCH: 0, BUS_EN: 0, STEP_LEN: 1, META_SECTION: 'IllOp' },
        { BUS_EN: 0, ILL_OP: 0, STEP_LEN: 1},
        { LAST_STEP: 1, BUS_EN: 0, STEP_LEN: 1, },
    ],

    'FENCE': nopCommon('FENCE'),
    'FENCE.I': nopCommon('FENCE.I'),

    'TRAP': [
        { OP_LATCH: 0, PC_IN_MUX: 'CSR', BUS_EN: 1, STEP_LEN: 4, META_SECTION: 'TRAP' },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { PC_IN_MUX: 'U', PC_IN_LATCH: 1, BUS_EN: 0, STEP_LEN: 1 },
        { PC_IN_LATCH: 0, LAST_STEP: 1, PC_OUT_LATCH: 1, BUS_EN: 0, STEP_LEN: 1, }
    ],

    'MRET': [
        { OP_LATCH: 0, PC_IN_MUX: 'CSR', BUS_EN: 1, STEP_LEN: 4, META_SECTION: 'MRET' },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { BUS_EN: 1, STEP_LEN: 4 },
        { PC_IN_MUX: 'U', PC_IN_LATCH: 1, BUS_EN: 0, STEP_LEN: 1 },
        { PC_IN_LATCH: 0, LAST_STEP: 1, PC_OUT_LATCH: 1, BUS_EN: 0, STEP_LEN: 1, }
    ],

    'WFI': [
        { OP_LATCH: 0, SLEEP: 1, BUS_EN: 0, STEP_LEN: 1, META_SECTION: 'WFI' },
        { SLEEP: 0, LAST_STEP: 1, PC_OUT_LATCH: 1, BUS_EN: 0, STEP_LEN: 1 },
    ],
}

opcodeValues = {
    'LUI':   { 'op': 0b01101 },
    'AUIPC': { 'op': 0b00101 },

    'JAL':   { 'op': 0b11011 },
    'JALR': { 'op': 0b11001, 'func3': 0b000 },

    'BEQ [Taken]':      { 'op': 0b11000, 'func3': 0b000, 'flag': 1 },
    'BEQ [Not taken]':  { 'op': 0b11000, 'func3': 0b000, 'flag': 0 },
    'BNE [Taken]':      { 'op': 0b11000, 'func3': 0b001, 'flag': 0 },
    'BNE [Not taken]':  { 'op': 0b11000, 'func3': 0b001, 'flag': 1 },
    'BLT [Taken]':      { 'op': 0b11000, 'func3': 0b100, 'flag': 1 },
    'BLT [Not taken]':  { 'op': 0b11000, 'func3': 0b100, 'flag': 0 },
    'BGE [Taken]':      { 'op': 0b11000, 'func3': 0b101, 'flag': 0 },
    'BGE [Not taken]':  { 'op': 0b11000, 'func3': 0b101, 'flag': 1 },
    'BLTU [Taken]':     { 'op': 0b11000, 'func3': 0b110, 'flag': 1 },
    'BLTU [Not taken]': { 'op': 0b11000, 'func3': 0b110, 'flag': 0 },
    'BGEU [Taken]':     { 'op': 0b11000, 'func3': 0b111, 'flag': 0 },
    'BGEU [Not taken]': { 'op': 0b11000, 'func3': 0b111, 'flag': 1 },

    'LB':    { 'op': 0b00000, 'func3': 0b000 },
    'LH':    { 'op': 0b00000, 'func3': 0b001 },
    'LW':    { 'op': 0b00000, 'func3': 0b010 },
    'LBU':   { 'op': 0b00000, 'func3': 0b100 },
    'LHU':   { 'op': 0b00000, 'func3': 0b101 },

    'SB':    { 'op': 0b01000, 'func3': 0b000 },
    'SH':    { 'op': 0b01000, 'func3': 0b001 },
    'SW':    { 'op': 0b01000, 'func3': 0b010 },

    'ADDI':  { 'op': 0b00100, 'func3': 0b000 },
    'XORI':  { 'op': 0b00100, 'func3': 0b100 },
    'ORI':   { 'op': 0b00100, 'func3': 0b110 },
    'ANDI':  { 'op': 0b00100, 'func3': 0b111 },
    'SLLI':  { 'op': 0b00100, 'func3': 0b001, 'sa': 0 },
    'SRLI':  { 'op': 0b00100, 'func3': 0b101, 'sa': 0 },
    'SRAI':  { 'op': 0b00100, 'func3': 0b101, 'sa': 1 },

    'SLTI [True]':   { 'op': 0b00100, 'func3': 0b010, 'flag': 1 },
    'SLTI [False]':  { 'op': 0b00100, 'func3': 0b010, 'flag': 0 },
    'SLTIU [True]':  { 'op': 0b00100, 'func3': 0b011, 'flag': 1 },
    'SLTIU [False]': { 'op': 0b00100, 'func3': 0b011, 'flag': 0 },

    'ADD':   { 'op': 0b01100, 'func3': 0b000, 'sa': 0, 'sys': 0 },
    'SUB':   { 'op': 0b01100, 'func3': 0b000, 'sa': 1, 'sys': 0 },
    'SLL':   { 'op': 0b01100, 'func3': 0b001, 'sa': 0, 'sys': 0 },
    'XOR':   { 'op': 0b01100, 'func3': 0b100, 'sa': 0, 'sys': 0 },
    'SRL':   { 'op': 0b01100, 'func3': 0b101, 'sa': 0, 'sys': 0 },
    'SRA':   { 'op': 0b01100, 'func3': 0b101, 'sa': 1, 'sys': 0 },
    'OR':    { 'op': 0b01100, 'func3': 0b110, 'sa': 0, 'sys': 0 },
    'AND':   { 'op': 0b01100, 'func3': 0b111, 'sa': 0, 'sys': 0 },

    'SLTU [True]':  { 'op': 0b01100, 'func3': 0b011, 'sa': 0, 'flag': 1 },
    'SLTU [False]': { 'op': 0b01100, 'func3': 0b011, 'sa': 0, 'flag': 0 },

    'SLT [True]':   { 'op': 0b01100, 'func3': 0b010, 'sa': 0, 'flag': 1 },
    'SLT [False]':  { 'op': 0b01100, 'func3': 0b010, 'sa': 0, 'flag': 0 },

    'FENCE':   { 'op': 0b00011, 'func3': 0b000 },
    'FENCE.I': { 'op': 0b00011, 'func3': 0b001 },

    'CSRRW':  { 'op': 0b11100, 'func3': 0b001 },
    'CSRRS':  { 'op': 0b11100, 'func3': 0b010 },
    'CSRRC':  { 'op': 0b11100, 'func3': 0b011 },
    'CSRRWI': { 'op': 0b11100, 'func3': 0b101 },
    'CSRRSI': { 'op': 0b11100, 'func3': 0b110 },
    'CSRRCI': { 'op': 0b11100, 'func3': 0b111 },

    'TRAP': { 'op': 0b11100, 'func3': 0b000, 'sa': 0, 'sys': 0 },
    'MRET': { 'op': 0b11100, 'func3': 0b000, 'sa': 1, 'sys': 1 },
    'WFI':  { 'op': 0b11100, 'func3': 0b000, 'sa': 0, 'sys': 1 },

    'IllOp': { 'op': 0b11111, 'func3': 0b111, 'sa': 1, 'flag': 1, 'sys': 1 },
}


def getOpcodeValues(opName):
    op = opcodeValues[opName]['op']

    values = []

    if 'func3' in opcodeValues[opName]:
        func3 = opcodeValues[opName]['func3']

        values.append(((func3 << 5) | op) << stepBits)
    else:
        for f in range(0,8):
            values.append(((f << 5) | op) << stepBits)


    n = len(values)

    for i in range (0,n):
        if 'sa' in opcodeValues[opName].keys():
            sa = opcodeValues[opName]['sa']
            values[i] |= (sa << 9) << stepBits
        else:
            values.append(values[i] | ((1 << 9) << stepBits))

    n = len(values)

    for i in range(0,n):
        if 'flag' in opcodeValues[opName].keys():
            flag = opcodeValues[opName]['flag']
            values[i] |= flag << (10 + stepBits)
        else:
            values.append(values[i] | (1 << (10 + stepBits)))

    n = len(values)
    for i in range(0,n):
        if 'sys' in opcodeValues[opName].keys():
            sys = opcodeValues[opName]['sys']
            values[i] |= sys << (8 + stepBits)
        else:
            values.append(values[i] | (1 << (8 + stepBits)))

    return values

def formatOpcodeValues(opName):
    vals = opcodeValues[opName]
    op = format(vals['op'], '05b')
    func3 = format(vals['func3'], '03b') if 'func3' in vals else 'XXX'
    sa = format(vals['sa'], '01b') if 'sa' in vals else 'X'
    flag = format(vals['flag'], '01b') if 'flag' in vals else 'X'
    sys = format(vals['sys'], '01b') if 'sys' in vals else 'X'
    return sys + flag + sa + func3 + op

def generateOp(opName):
    op = opFetch + opcodes[opName]

    signals = [ signalDefaults.copy() ]

    while signals[-1][OP_DONE] == 1 or signals[-1][CLK] == 0:
        signals.append(signals[-1].copy())
        step(op, signals[-2], signals[-1])

    opcodeValues = getOpcodeValues(opName)

    print('%s: %s ' % (opName, ', '.join(map(lambda v: '%03X' % (v >> stepBits), opcodeValues))))

    steps = list(map(lambda s: s[STEP_LEN], filter(lambda t: (t[CLK] == 0) and (t[SUBSTEP] == 0) and (t[STEP] >= 0), signals)))
    sigLen = len(steps)
    print('%s: %d steps' % (opName, sigLen))

    if sigLen >= (1 << stepBits):
        print('Warning: %s exceeds %d steps' % (opName, 1 << stepBits))

    for s in signals:
        if (s[CLK] == 0) and (s[SUBSTEP] == 0) and (s[STEP] >= 0):
            val = 0
            for sigName in signalValues.keys():
                val |= signalValues[sigName][s[sigName]]
            for opVal in opcodeValues:
                microcode[opVal | s[STEP]] = val

    signals.append(signalDefaults.copy())
    return signals

def plotOp(opName, signals, f):
    f.write('\\vfill\n')
    f.write('\\begin{tikztimingtable}[')
    f.write('xscale=0.85,')
    f.write('yscale=1,')
    f.write('thick,')
    f.write('timing/slope=0,')
    f.write('timing/u/background/.style={pattern=north west lines,')
    f.write('pattern color=black!10},')
    f.write('timing/d/background/.style={fill=black!5},')
    f.write('timing/expand count=2000,')
    f.write(']')

    signalNames = list(filter(lambda s: s not in hiddenSignals, signals[0].keys()))

    talliedSignals = {}

    for sigName in signalNames:
        if sigName == 'SUBSTEP':
            talliedSignals[sigName] = [{'value': -1, 'count': 1}] + [{'value': n, 'count': 2} for i, n in enumerate(list(map(lambda s: s[sigName], signals))[1::2])]
            talliedSignals[sigName][-1]['value'] = -1
        else:
            talliedSignals[sigName] = [{'value': g, 'count': len(list(v))} for g, v in groupby(map(lambda s: s[sigName], signals))]

        if sigName == 'STEP_LEN':
            stepLenList = [{'value': -1, 'count': 1} ]
            for ev in talliedSignals['STEP_LEN'][1:]:
                val = ev['value']
                count = ev['count']
                for i in range(0, count // (2 * val)):
                    stepLenList.append({'value': val, 'count': 2 * val})
            stepLenList[-1]['value'] = -1
            talliedSignals[sigName] = stepLenList


        t = 0
        for event in talliedSignals[sigName]:
            event['start'] = t
            t += event['count']
            event['end'] = t

    commonRemain = 4
    commonEnd = 0

    if removeCommon:
        commonEnd = talliedSignals[OP_LATCH][1]['start']
        for sigName in signalNames:
            talliedSignals[sigName] = list(filter(lambda ev: ev['end'] > commonEnd - commonRemain, talliedSignals[sigName]))
            for ev in filter(lambda ev: ev['start'] < commonEnd - commonRemain, talliedSignals[sigName]):
                ev['count'] -= commonEnd - commonRemain - ev['start']
                ev['start'] = commonEnd - commonRemain

    if removeConst:
        talliedSignals = {k: v for k, v in talliedSignals.items() if len(v) > 1}
        signalNames = talliedSignals.keys()


    gaps = []
    if addGaps:
        eventStarts = []

        for sigName in filter(lambda name: name not in specialSignals, signalNames):
            for event in talliedSignals[sigName]:
                eventStarts.append(event['start'])

        eventStarts = sorted(list(set(eventStarts)))

        gapMin = 8
        gapRemain = 3

        for i in range(0, len(eventStarts)-1):
            if eventStarts[i+1] - eventStarts[i] > gapMin:
                gaps.append((eventStarts[i] + gapRemain, eventStarts[i+1] - gapRemain))

        gaps = list(map(lambda g: (((g[0]-1)//2) * 2, ((g[1]+3)//2) * 2), gaps))

        newTalliedSignals = { sigName: talliedSignals[sigName] for sigName in specialSignals if sigName in signalNames }

        for sigName in filter(lambda name: name not in specialSignals, signalNames):
            newTally = []
            g = gaps.copy()

            for event in talliedSignals[sigName]:
                ev = event
                while len(g) > 0 and event['end'] > g[0][0]:
                    if sigName in busSignals:
                        ev = {'value': event['value'], 'count': (ev['count']) - (g[0][1] - g[0][0]) + gapRemain, 'start': ev['start'], 'end': ev['end']}
                    else:
                        newTally.append({'value': event['value'], 'count': g[0][0] - ev['start'], 'start': ev['start'], 'end': g[0][0]})
                        newTally.append({'value': 'G'+str(event['value']), 'count': gapRemain, 'start': g[0][0], 'end': g[0][1]})
                        ev = {'value': event['value'], 'count': ev['end'] - g[0][1], 'start': g[0][1], 'end': ev['end']}
                    g.pop(0)
                newTally.append(ev)

            newTalliedSignals[sigName] = newTally

        for sigName in counterSignals:
            newTally = talliedSignals[sigName]
            for g in gaps:
                before = list(filter(lambda ev: ev['start'] < g[0], newTally))
                during = list(filter(lambda ev: ev['start'] >= g[0] and ev['end'] <= g[1], newTally))
                after = list(filter(lambda ev: ev['end'] > g[1], newTally))
                gap = []
                if len(during) > 0:
                    gap.append({ 'value': 'U', 'count': gapRemain + (during[-1]['end'] - during[0]['start']) - (g[1] - g[0]), 'start': during[0]['start'], 'end': during[-1]['end'] })
                newTally = before + gap + after
            newTalliedSignals[sigName] = newTally


        for sigName in clockSignals:
            if sigName in signalNames:
                newTally = talliedSignals[sigName]
                for g in gaps:
                    before = list(filter(lambda ev: ev['start'] < g[0] and ev['end'] <= g[1], newTally))
                    during = list(filter(lambda ev: ev['start'] >= g[0] and ev['end'] <= g[1], newTally))
                    over = list(filter(lambda ev: ev['start'] < g[0] and ev['end'] > g[1], newTally))
                    after = list(filter(lambda ev: ev['start'] >= g[0] and ev['end'] > g[1], newTally))

                    gap = []
                    if len(during) > 0:
                        gap.append({ 'value': 'U', 'count': gapRemain + (during[-1]['end'] - during[0]['start']) - (g[1] - g[0]), 'start': during[0]['start'], 'end': during[-1]['end'] })
                    if len(over) > 0:
                        gap.append({'value': over[0]['value'], 'count': g[0] - over[0]['start'], 'start': over[0]['start'], 'end': g[0]})
                        gap.append({'value': 'G'+str(over[0]['value']), 'count': gapRemain, 'start': g[0], 'end': g[1]})
                        gap.append({'value': over[0]['value'], 'count': over[0]['end'] - g[1], 'start': g[1], 'end': over[0]['end']})

                    newTally = before + gap + after
                newTalliedSignals[sigName] = newTally
            talliedSignals = newTalliedSignals

        newGaps = []
        removed = 0
        for i in range(0,len(gaps)):
            newGaps.append((gaps[i][0] - removed, gaps[i][0] - removed + gapRemain))
            removed += gaps[i][1] - gaps[i][0] - gapRemain
        gaps = newGaps

    shift = commonEnd - commonRemain if addGaps and removeCommon else 0
    shiftedGaps = [ (g[0] - shift, g[1] - shift) for g in gaps ]

    for ev in talliedSignals[META_COMMENT]:
        for sigName, comment in ev['value'].items():
            if sigName in talliedSignals:
                l = list(filter(lambda e: e['start'] == ev['start'], talliedSignals[sigName]))
                if len(l) > 0:
                    l[0]['comment'] = comment

    commentCount = 0
    commentList = []

    for sigName in filter(lambda s: s not in metaSignals, signalNames):
        if sigName == 'CLK_COUNT':
            f.write(' & ')
        else:
            f.write('\\texttt{%s} & ' % sigName.replace('_', '\\_'))

        values = talliedSignals[sigName]

        styleFormat = {
            0: '',
            1: '',
            'G0': ';[gray,dashed]',
            'G1': ';[gray,dashed]',
            'U': '',
        }

        valueFormat = {
            0: 'L',
            1: 'H',
            'X': 'X',
            'G0': 'L;[black]',
            'G1': 'H;[black]',
            'U': 'U'
        }

        for val in values:
            if 'comment' in val:
                commentList.append(val['comment'])
                f.write('N(C%d)' % commentCount)
                commentCount += 1

            if sigName in busSignals:
                if sigName in [STEP, CLK_COUNT, SUBSTEP, STEP_LEN] and val['value'] == -1:
                    f.write('%dD{}' % (val['count']))
                elif isinstance(val['value'], int):
                    f.write('%dD{%d}' % (val['count'], val['value']))
                elif val['value'] == 'U':
                    f.write('%dU{}' % (val['count']))
                elif val['value'] == 'G':
                    f.write(';[gray,dashed]%dU{};[black]' % (val['count']))
                else:
                    f.write('%dD{%s}' % (val['count'], val['value'].replace('_', '\\_')))
            else:
                f.write('%s%d%s' % (styleFormat[val['value']], val['count'], valueFormat[val['value']]))

        f.write('\\\\\n')

    markerList = list(accumulate(map(lambda ev: ev['count'], talliedSignals[META_SECTION])))[0:-2]

    stepValues = []

    stepPos = 0
    for ev in talliedSignals[STEP]:
        stepPos += ev['count']
        if stepPos not in markerList:
            stepValues.append(stepPos)

    f.write('\\extracode\n')
    f.write('\\tableheader{}{%s [%s]}\n' % (opName, formatOpcodeValues(opName)))
    f.write('\\begin{pgfonlayer}{background}\n')
    f.write('\\begin{scope}[semitransparent,semithick]\n')
    f.write('\\horlines[gray]{}\n')
    f.write('\\vertlines[gray]{%s}\n' % ', '.join(map(str, stepValues)))
    # f.write('\\vertlines[very thick,red,dashed]{%d}\n' % (fetchLinePos))
    f.write('\\vertlines[very thick,red,dashed]{%s}\n' % ', '.join(map(str,markerList)))
    if len(shiftedGaps) > 0:
        f.write('\\vertlines[thick, gray, dashed]{%s}\n' % ', '.join(map(lambda g: str(g[0]), shiftedGaps)))
        f.write('\\vertlines[thick, gray, dashed]{%s}\n' % ', '.join(map(lambda g: str(g[1]), shiftedGaps)))
    f.write('\\end{scope}\n')
    f.write('\\begin{scope}[semithick]\n')
    for i in range(0, commentCount):
        f.write('\\node at ($(C%d.HIGH)-(0.4,0.4)$) [anchor=south west] {\\tiny %s};\n' % (i, commentList[i]))
    f.write('\\end{scope}\n')
    f.write('\\end{pgfonlayer}\n')
    f.write('\\end{tikztimingtable}')
    f.write('\\vfill\n\n')
    # f.write('\\newpage\n\n')





microcode = [0] * (1 << 17)

opSignals = {}

for opName in opcodes.keys():
    opSignals[opName] = generateOp(opName)

numUndef = 1
for i in range(0, len(microcode), 1 << stepBits):
    if microcode[i] == 0:
        numUndef += 1
        for j in range(0,1 << stepBits):
            microcode[i | j] = microcode[(0x7FF << stepBits) | j]

print('%d undefined opcodes\n' % numUndef)

f = open("Digital/microcode.hex", "w")

f.write('v2.0 raw\n')

for val in microcode:
    f.write('%X\n' % val)

f.close()


plotList = sys.argv[1:]
if plotList:
    print('Generating plot data:')
    f = open("signals.tex", "w")

    f.write('\\documentclass{article}\n')
    f.write('\\usepackage[margin=0.5cm,landscape,a1paper]{geometry}\n')
    f.write('\\usepackage{tikz-timing}[2009/12/09]\n')
    f.write('\\usetikztiminglibrary{advnodes}\n')
    f.write('\\usetikzlibrary{patterns}\n')
    f.write('\\renewcommand{\\neg}[1]{$\\overline{\\mbox{#1}}$}\n')
    f.write('\\pagestyle{empty}\n')
    f.write('\\begin{document}\n')

    for opName in plotList:
        if opName in opSignals:
            print('%s' % opName)
            plotOp(opName, opSignals[opName], f)
        else:
            print('%s -- Unknown instruction' % opName)

    f.write('\\end{document}\n\n')
    f.close()
