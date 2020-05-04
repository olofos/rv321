opcodes = [
    list(map(lambda n: 0xFFFFFFFF if n == 0 else n, [int(n,16) for n in open('compressed-0.dat', 'r').read().split()])),
    list(map(lambda n: 0xFFFFFFFF if n == 0 else n, [int(n,16) for n in open('compressed-1.dat', 'r').read().split()])),
    list(map(lambda n: 0xFFFFFFFF if n == 0 else n, [int(n,16) for n in open('compressed-2.dat', 'r').read().split()])),
    list(map(lambda n: 0xFFFF0003 | (n << 2), range(0, 1 << 14)))
]

lo = open('Digital/compressed-lo.hex', 'w')
hi = open('Digital/compressed-hi.hex', 'w')

lo.write('v2.0 raw\n')
hi.write('v2.0 raw\n')

for op_bits in range(0, 1 << 16):
    op_bits_lo = op_bits & 0x3
    op_bits_hi = op_bits >> 2
    op = opcodes[op_bits_lo][op_bits_hi]

    lo.write('%04X\n' % (op & 0xFFFF))
    hi.write('%04X\n' % ((op >> 16) & 0xFFFF))
