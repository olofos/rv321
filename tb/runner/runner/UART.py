class UART:
    def __init__(self, serial):
        self.serial = serial
        self.send('echo off')

    def send(self, string):
        self.serial.write(string.encode() + b'\r')
        self.serial.readline() # Skip newline

    def send_data_command(self, command, data):
        if data is not None:
            string = '{0} {1}'.format(command, ' '.join([format(n, 'x') for n in data]))
            self.send(string)

    def send_row(self, row):
        self.send_data_command('write-pullup', row.delta.pullup)
        self.send_data_command('write-iodir', row.delta.iodir)
        self.send_data_command('write-io', row.delta.output)

    def read_io(self):
        self.send('read-io')
        response = self.serial.readline()
        return [int(n,16) for n in response.split()]
