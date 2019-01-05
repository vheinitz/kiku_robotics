from os.path import expanduser
import serial
import time
import json
import ast
import sys
import serial.tools.list_ports

class KiKuBoard:

    def __init__(self):
        self.ser = None
        self.sensor_data={}
        self.outputs = {}
        self.isConnected = False

    def connect(self, port):

        self.ser = serial.Serial(port, baudrate=38400)
        time.sleep(3)
        self.ser.write('#')  # Identify
        time.sleep(1)
        msg = self.ser.read_all()
        if msg.find('ASP Ver') != -1:
            self.isConnected = True
            return True
        else:
           return False

    def close(self):
        try:
            self.ser.close()
        except:
            pass
        time.sleep(1)
        self.ser = None
        self.isConnected = False

    def connected(self):
        return self.isConnected

    def version(self):
        self.ser.write('#')  # Identify
        time.sleep(0.3)
        msg = self.ser.read_all()
        if msg.find('ASP Ver') != -1:
            return msg
        else:
            return ''

    def set(self, num):
        if num not in self.outputs:
            self.ser.write('co%d' % num)
            time.sleep(1)
        self.ser.write('s%d' % num)
        self.outputs[num] = 1

    def reset(self, num):
        if num not in self.outputs:
            self.ser.write('co%d' % num)
            time.sleep(1)
        self.ser.write('r%d' % num)
        self.outputs[num] = 0

    def motor(self, num, val):
        self.ser.write('m%d %s' % (num, val))

    def stepper(self, num, steps):
        self.ser.write('t%d %s' % (num, steps))

    def poll(self, t=1):
        try:
            self.ser.write('g')  # g - read all inputs
            time.sleep(t)
            msg = self.ser.read_all()
            self.sensor_data = ast.literal_eval(str(msg))
        except:
            pass

    def getDI(self, num):
        return self.sensor_data['DI'][num]

    def DICnt(self):
        return len(self.sensor_data['DI'])

    def getAI(self, num):
        return self.sensor_data['AI'][num]

    def AICnt(self):
        return len(self.sensor_data['AI'])

if __name__ == '__main__':
    kb = KiKuBoard()
    kb.connect("COM6")
    print kb.version()
    kb.stepper(1,200)
    time.sleep(3)
    kb.poll()
    for i in range(2,10):
        print kb.getDI(i)
    kb.close()