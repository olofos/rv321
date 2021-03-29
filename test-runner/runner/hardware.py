import re

NUM_CHIPS = 5

class PinState:
    def __init__(self, chip, bit, pullup, output_signal, input_signal):
        self.chip = chip
        self.bit = bit
        self.pullup = pullup
        self.output_signal = output_signal
        self.input_signal = input_signal
        self.output = None
        self.expected_input = None
        self.read_input = None

class PinConfig:
    def __init__(self, chip, bit):
        assert 0 <= chip < 5
        assert 0 <= bit < 16

        self.chip = chip
        self.bit = bit
        self.pullup = True
        self.default_output = None
        self.output_signal = None
        self.input_signal = None

    def get_state(self, signals):
        state = PinState(self.chip, self.bit, self.pullup, self.output_signal, self.input_signal)

        if self.output_signal is not None:
            sig, bit = self.output_signal
            if sig in signals:
                val = signals[sig]
                if type(val) is int:
                    state.output = (val >> bit) & 1
                elif val == 'X':
                    raise ValueError('Value "x" is not allowed for an output')
            else:
                state.output = self.default_output

        if self.input_signal is not None:
            sig, bit = self.input_signal
            if sig in signals:
                val = signals[sig]
                if type(val) is int:
                    state.expected_input = (val >> bit) & 1

        return state

class HardwareConfig:
    def __init__(self):
        self.pins = { (chip, bit): PinConfig(chip, bit) for chip in range(0,NUM_CHIPS) for bit in range(0,16) }
        self.connectors = {}
        self.output_signals = {}
        self.input_signals = {}

    def add_connector(self, len, pins):
        if len in self.connectors:
            self.connectors[len].extend(pins)
        else:
            self.connectors[len] = pins
        for pin in pins:
            if pin is not None:
                self.pins[pin].pullup = False

    def add_connector_signals(self, conn_pin, output, input):
        (conn, pin_num) = conn_pin
        pin_name = self.connectors[conn][pin_num]

        if output is not None:
            sig, bit = output
            self.pins[pin_name].output_signal = (sig, bit)
            if not sig in self.output_signals:
                self.output_signals[sig] = {}
            self.output_signals[sig][bit] = pin_name

        if input is not None:
            if type(input) is tuple:
                sig, bit = input
            else:
                sig, bit = input, 0
            self.pins[pin_name].input_signal = (sig, bit)
            if not sig in self.input_signals:
                self.input_signals[sig] = {}
            self.input_signals[sig][bit] = pin_name

    def add_signal_default(self, sig, val):
        if type(val) is int:
            out_filter = lambda p: (p.output_signal is not None) and (p.output_signal[0] == sig)
            for pin in filter(out_filter, self.pins.values()):
                pin.default_output = (val >> pin.output_signal[1]) & 1
        elif val in 'Pp':
            any_filter = lambda p: ((p.output_signal is not None) and (p.output_signal[0] == sig)) or ((p.input_signal is not None) and (p.input_signal[0] == sig))
            for pin in filter(any_filter, self.pins.values()):
                pin.pullup = True

    def get_state(self, signals):
        pins = { pin: self.pins[pin].get_state(signals) for pin in self.pins }
        return HardwareState(pins, self.input_signals)

class HardwareState:
    def __init__(self, pins, input_signals):
        self.pins = pins
        self.input_signals = input_signals

        self.output = [0] * NUM_CHIPS
        self.expected_input = [0] * NUM_CHIPS
        self.input_mask = [0] * NUM_CHIPS
        self.pullup = [0] * NUM_CHIPS
        self.iodir = [0xFFFF] * NUM_CHIPS
        self.read_input = None

        for chip in range(0, NUM_CHIPS):
            for bit in range(0, 16):
                pin = self.pins[(chip, bit)]

                if pin.output is not None:
                    self.output[chip] |= pin.output << bit
                    self.iodir[chip] &= ~(1 << bit)

                if pin.expected_input is not None:
                    self.expected_input[chip] |= pin.expected_input << bit
                    self.input_mask[chip] |= 1 << bit

                if pin.pullup:
                    self.pullup[chip] |= 1 << bit

    def __len__(self):
        return NUM_CHIPS

    def __bool__(self):
        for chip in range(0, NUM_CHIPS):
            expected = self.expected_input[chip] & self.input_mask[chip]
            read = self.read_input[chip] & self.input_mask[chip]
            if expected != read:
                return False
        return True

    @property
    def mismatched_signals(self):
        mismatched_signals = set()

        for sig, sig_bits in self.input_signals.items():
            for chip, bit in sig_bits.values():
                expected = (self.expected_input[chip] & self.input_mask[chip]) & (1 << bit)
                read = (self.read_input[chip] & self.input_mask[chip]) & (1 << bit)
                if expected != read:
                    mismatched_signals.add(sig)
                    break

        return { sig: self.get_input_values(sig) for sig in mismatched_signals }

    def get_input_values(self, signal):
        expected = 0
        read = 0
        for sig_bit, pin in self.input_signals[signal].items():
            chip, bit = pin
            expected |= ((self.expected_input[chip] >> bit) & 1) << sig_bit
            read |= ((self.read_input[chip] >> bit) & 1) << sig_bit
        return (expected, read)

if __name__ == "__main__":
    hardware = HardwareConfig()
    hardware.add_connector('20', ['x', '0b6', '0b7', '1b7', '1b6', '1b5', '1b4', '1b3', '1b2', '1b1', '1b0', '1a7', '1a6', '1a5', '1a4', '1a3', '1a2', '1a1', '1a0', 'x'])

    for s in [('20:1', None, None),('20:2', 'BUS-CLK', None),('20:3', None, 'GND'),('20:4', 'IN', None),('20:5', None, 'A'),('20:6', None, 'B'),('20:7', 'RESET', None),('20:8', '~{IN-EN}', None),('20:9', '~{A-EN}', None),('20:10', '~{B-EN}', None),('20:11', 'SD:0', None),('20:12', 'SD:1', None),('20:13', 'SD:2', None),('20:14', 'SB:0', None),('20:15', 'SB:1', None),('20:16', 'SB:2', None),('20:17', 'SA:0', None),('20:18', 'SA:1', None),('20:19', 'SA:2', None),('20:20', None, None)]:
        hardware.add_connector_signals(*s)

    for d in [('BUS-CLK', 0), ('IN', 0), ('RESET', 1), ('~{IN-EN}', 1), ('~{A-EN}', 1), ('~{B-EN}', 1), ('SA', 0), ('SB', 0), ('SD', 0), ('A', 'P'), ('B', 'P'), ('GND', 'Z')]:
        hardware.add_signal_default(*d)

    state = hardware.get_state({})
    row = state.to_binary()
    for chip in range(0,NUM_CHIPS):
        print(', '.join(['{:016b}'.format(n) for n in [ row.iodir[chip], row.output[chip], row.expected_input[chip], row.input_mask[chip], row.pullup[chip]]]))
