class UART:
    def __init__(self, serial):
        self.serial = serial

    def send_row(self, row):
        data = bytearray()
        for chip in range(0,5):
            data.append(row.iodir[chip] & 0xFF)
            data.append((row.iodir[chip] >> 8) & 0xFF)
            data.append(row.pullup[chip] & 0xFF)
            data.append((row.pullup[chip] >> 8) & 0xFF)
            data.append(row.output[chip] & 0xFF)
            data.append((row.output[chip] >> 8) & 0xFF)
            data.append(row.expected_input[chip] & 0xFF)
            data.append((row.expected_input[chip] >> 8) & 0xFF)
            data.append(row.input_mask[chip] & 0xFF)
            data.append((row.input_mask[chip] >> 8) & 0xFF)
