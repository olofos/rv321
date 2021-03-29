import re

from .hardware import HardwareConfig

def _str_to_chip_bit(str):
    if str in 'xX':
        return None
    assert re.match(r'[0-4][AaBb][0-7]', str)
    chip, bank, bit = str
    return int(chip), 8 * (ord(bank.upper()) - ord('A')) + int(bit)


def _split_signal_and_bit(str):
    if str == '-':
        return None
    split = str.split(':')
    assert len(split) < 3
    signal = split[0]
    if len(split) == 2:
        bit = int(split[1])
    elif len(split) == 1:
        bit = 0
    return ( signal, bit )

def _split_connector_and_bit(str):
    ( conn, bit ) = _split_signal_and_bit(str)
    if bit > 0:
        bit = bit - 1
    return (conn, bit)


def parse(input):
    signals_line = re.search(r'[^ \t\f\r\n][^\n]*\n', input).group()
    signals = signals_line.split()

    lines = input.split('\n')
    commands = []
    for line in lines:
        stripped = line.strip()
        if stripped.startswith('#!'):
            commands.append(stripped[2:].lstrip())

    hardware_config = HardwareConfig()

    re_pin_to_signal = re.compile(r'([1-9][0-9]*:[1-9][0-9]*)\s+([^\s]+)\s+([^\s]+)\s*(?:#.*)?$')
    for cmd in commands:
        if cmd.upper().startswith('CONN '):
            _, length, *pin_names = cmd.split()
            hardware_config.add_connector(length, [ _str_to_chip_bit(pin) for pin in pin_names ])
        elif re_pin_to_signal.match(cmd):
            m = re_pin_to_signal.match(cmd)
            conn_pin, output, input = m.groups()
            hardware_config.add_connector_signals(_split_connector_and_bit(conn_pin), _split_signal_and_bit(output), _split_signal_and_bit(input))
        else:
            sig, val = cmd.split()
            hardware_config.add_signal_default(sig, val)

    return signals, hardware_config
