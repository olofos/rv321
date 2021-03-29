import pytest

from runner.hardware import HardwareConfig

def test_add_connector_simple():
    h = HardwareConfig()
    h.add_connector('3', [None, (1,2), (1,10) ])

    assert '3' in h.connectors
    assert h.connectors['3'] == [None, (1,2), (1,10) ]

def test_add_connector_twice():
    h = HardwareConfig()
    h.add_connector('6', [None, (0,1), (0,2)])
    h.add_connector('6', [(1,2), (1,3), None])

    assert '6' in h.connectors
    assert h.connectors['6'] == [None, (0,1), (0,2), (1,2), (1,3), None]

def test_add_connector_signal__can_add_signals():
    h = HardwareConfig()
    h.add_connector('6', [None, (0,0), (0,1), (0,2), (0,3), None])

    h.add_connector_signals(('6',1), ('A', 0), ('B',2))
    h.add_connector_signals(('6',2), ('C',2), None)

    assert h.pins[(0,0)].output_signal == ('A', 0)
    assert h.pins[(0,0)].input_signal == ('B', 2)

    assert h.pins[(0,1)].output_signal == ('C', 2)
    assert h.pins[(0,1)].input_signal is None

    assert h.output_signals['A'][0] == (0,0)
    assert h.output_signals['C'][2] == (0,1)
    assert h.input_signals['B'][2] == (0,0)

def test_add_connector_signal__can_and_nothing_to_unconnected_pin():
    h = HardwareConfig()
    h.add_connector('6', [None, (0,0), (0,1), (0,2), (0,3), None])

    h.add_connector_signals(('6',0), None, None)


def test_add_connector_signal__cant_add_a_signal_to_unconnected_pin():
    h = HardwareConfig()
    h.add_connector('6', [None, (0,0), (0,1), (0,2), (0,3), None])

    with pytest.raises(Exception):
        h.add_connector_signals(('6',0), (A,1), None)

def test_add_signal_default():
    h = HardwareConfig()
    h.add_connector('6', [(0,0), (0,1), (0,2), (0,3), (0,4), (0,5)])
    h.add_connector_signals(('6',0), ('A',0), ('B',0))
    h.add_connector_signals(('6',1), ('B',0), ('A',0))
    h.add_connector_signals(('6',2), ('C',0), ('D',0))
    h.add_connector_signals(('6',3), None, ('C',0))
    h.add_connector_signals(('6',4), ('E',0), None)
    h.add_connector_signals(('6',5), ('E',1), None)

    h.add_signal_default('A', 0)
    h.add_signal_default('B', 1)
    h.add_signal_default('C', 'P')
    h.add_signal_default('D', 'Z')
    h.add_signal_default('E', 2)

    assert h.pins[(0,0)].default_output == 0
    assert h.pins[(0,1)].default_output == 1
    assert h.pins[(0,2)].default_output is None
    assert h.pins[(0,3)].default_output is None
    assert h.pins[(0,4)].default_output == 0
    assert h.pins[(0,5)].default_output == 1

    assert h.pins[(0,0)].pullup == False
    assert h.pins[(0,1)].pullup == False
    assert h.pins[(0,2)].pullup == True
    assert h.pins[(0,3)].pullup == True
    assert h.pins[(0,4)].pullup == False
    assert h.pins[(0,5)].pullup == False
