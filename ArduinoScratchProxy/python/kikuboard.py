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

    def connect(self, port):

        self.ser = serial.Serial(port, baudrate=38400)
        time.sleep(3)
        self.ser.write('#')  # Identify
        time.sleep(1)
        msg = self.ser.read_all()
        if msg.find('ASP Ver') != -1:
            print ("Found on port: %s" % port)
            return True
        else:
           return False

    def close(self):

        self.ser.close()
        time.sleep(1)
        self.ser=None

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
            ser.write('co%d' % n)
            time.sleep(0.3)
        self.ser.write('s%d' % n)
        self.outputs[num] = 1

    def reset(self, num):
        if num not in self.outputs:
            ser.write('co%d' % n)
            time.sleep(0.3)
        self.ser.write('r%d' % n)
        self.outputs[num] = 0

    def motor(self, num, val):
        ser.write('m%d %s' % (num, val))

    def poll(self):
        try:
            self.ser.write('g')  # g - read all inputs
            time.sleep(1)
            msg = self.ser.read_all()
            self.sensor_data = ast.literal_eval(str(msg))
        except:
            pass

    def getDI(self, num):
        return self.sensor_data['DI'][num-2]

    def getAI(self, num):
        return self.sensor_data['AI'][num-1]

if __name__ == '__main__':
    kb = KiKuBoard()
    kb.connect("COM3")
    print kb.version()
    kb.poll()
    for i in range(2,10):
        print kb.getDI(i)
    kb.close()