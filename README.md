# rv321

rv321 is an RISC-V processor implemented using 74HC series logic. Currently the RV32E base instruction set is supported. The design has been simulated using [hneemann's Digital](https://github.com/hneemann/Digital). The simulated circuit also supports the C and Zicsr extensions, though these are not fully implemented in the hardware. The circuit is highly modular, and support for these extra extensions, or for the full RV32I base instruction set, can easily be added by changing only some of the modules.

The rv321 is implemented with a [bit-serial architecture](https://en.wikipedia.org/wiki/Bit-serial_architecture) where, for example, the ALU acts on one bit at a time. This makes the CPU very slow, each instruction requires around 100 clock cycles. The benefit of this architecture is that a one bit ALU is much smaller than a 32-bit ALU and that internal data paths only require a single line instead of a big parallel bus. This makes it possible to implement various modules as individual components connected through simple cables.

## Supported Instructions

The CPU supports all the instructions of the RV32E as well as the C and Zicsr extensions. However, the instructions are not completely decoded, which means that some illegal instructions will be incorrectly interpreted as valid instructions. This mostly affect instruction encodings that are not defined in the RISC-V specification. However, a few encodings that are valid RISC-V instructions, such as `sret` and `uret` will be erroneously decoded instead of raising an illegal instruction exception, even though these instructions are illegal since code on rv321 only runs in machine mode.

## CSRs

Currently the following CSRs are implemented:

* `mstatus`
* `mepc`
* `mtvec`
* `mscratch`
* `mcause`
* `mip`
* `mie`

All other CSRs are read as zero. Note that reading from or writing to an invalid CSR does *not* cause an illegal instruction exception.

## Traps and Interrupts

# Technical Details

The rv321 CPU is microcoded with the code stored in four 128kB parallel flash ROMs. The instructions are decoded by addressing these ROMs by 10 bits of the opcode, together with a six bit step counter and a flag bit, which is used by for example the branch instructions. Each opcode is processed in a number of *steps*, which in turn consist of either one or four *clock cycles*. There are two types of clock cycles: *control cycles* and *bus cycles*. During a control cycle only the control lines are being changed, while bus cycles are used to transfer data along the various internal and external buses. A step with a control cycle is always a single cycle long, but a step with bus cycles can be either one or four cycles long. 

Internal registers are implemented using shift registers that feed back into themselves, effectively creating delay line memories. Because of this, the bus cycles always have to come in groups of 32. Most data transfers are 32 bits and thus automatically take 32 bus cycles to complete anyway. However, there are a couple of cases where a shorter data transfer has to be extended by extra bus cycles to keep the registers in sync. One such examples is storing a single byte to memory, where only 8 cycles are needed for the actual data transfer, but an additional 24 cycles are needed to keep the alignment of the registers (note that a load of an 8 bit byte would always need 32 bus cycles since we need to fill the upper 24 bit with either zeros or the sign extension of the loaded byte). A second example is when transferring a 12 bit CSR address, where a padding of 20 cycles is necessary. These extra dummy bus cycles could have been avoided by gating the bus clock to registers that are not involved in the transfer, but the current design saves a bit of complexity by not introducing extra clock gates and control lines to drive them. 

## The ALU

The ALU supports the following operations:

* Bitwise boolean operations: `and`, `or` and `xor`
* Addition and subtraction
* Comparisons: equality, signed less-than and unsigned less-than
* Logical shift left, logical shift right and arithmetic shift right

The first three sets of operations are fairly straight forward.

The shift operations are more interesting. It does not use a full 32 bit barrel shifter since this would take up quite a bit of space. Instead the HEF4557B variable length shift register is used as intermediate storage during the computation (the same type of shift register is used in several other places in the rv321 as a convenient serial-in serial-out shift register with a fairly large number of bits, but the shifter in the ALU is the only circuitry that takes advantage of the possibility to vary the length of the shift register). The computation of a value shifted left or right by `n` bits goes in several steps:

1. Use the addition circuitry of the ALU to calculate the needed shift register length: `L = 32 + n` for a left shift and `L = 32 - n` for a right shift
1. Configure the HEF4557B shift register to be `L` bits long and clear its content
1. Shift in 32 bits of input data into the shift register
1. Shift out 32 bits of output data from the shift register

If the shift register length `L` is less than 32 (as is the case for a right shift with `n > 0`) we fill the remaining bits with either zero for a logical shift, or with the top bit of the output (the sign bit) for an arithmetic shift. The sign bit is stored in an extra flip flop which is set in the previous step.

As an example let us shift `0x87654321` by 12 bits to the left

1. The length of the shift register is `L = 32 + 12 = 44`.
1. The shift register is cleared. Its content is `0x00000000000`.
1. We shift in our 32 bits of data, starting with the least significant bit. After this the content of the shift register is `0x87654321000`.
1. We shift out 32 bits of output data, starting with the least significant bit. The result is the bottom four bytes of the shift register, `0x54321000`, which is exactly the expected result. The shift register still contains `0x00000000876`, but these bits are never used.

As a second example let us take the arithmetical shift `0x87654321` by 12 bits to the right

1. The length of the shift register is `L = 32 - 12 = 20`.
1. The shift register is cleared. Its content is `0x00000`.
1. We shift in our 32 bits of data, starting with the least significant bit. After this the content of the shift register is `0x87654`. Note that the lowest 12 bits have been discarded since they did not fit in the register.
1. We shift out 32 bits of output data, starting with the least significant bit. Since the shift register now contains less than 32 bits, we repeatedly shift in the sign bit of the input, which is our case is one. The resulting output is `0xFFF87654`, which again is the expected result. The shift register is left containing `0xFFFFF`.



## Possible Optimisations

The bit-serial architecture makes the rv321 CPU very inefficient. However, there is room for optimisation of the current implementation. There are two main steps for each instruction. First the instruction is fetched and then it is executed. The fetching is exactly the same for all instructions. It would be more efficient to begin fetching the next instruction while executing the previous instruction. This would make any instruction that does not affect the program counter or access memory run faster. However, this would require a more complicated reset circuit, to ensure that the very first instruction is properly fetched, and some parts of the circuit are made simpler by the fact that each instruction has a well-defined start and end. Moreover, the program counter is currently always routed through the ALU, so that there is only one source of memory addresses. For the ALU to run simultaneously with the loading of the memory address from the program counter, a new multiplexer would need to be added.

    

