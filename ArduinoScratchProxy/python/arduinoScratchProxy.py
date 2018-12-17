from os.path import expanduser
from Scratch import Scratch
import time
import json
import ast
import sys
import serial.tools.list_ports
from kikuboard import KiKuBoard

debug = 0
home = './'  # expanduser("~")

class ArduinoScratchProxy:

    def __init__(self):
        self.scr = None
        self.kiku = None

    def start(self):
        while True:

            if not self.kiku or self.kiku.connected() == False:
                try:
                    lines = open("%s/arduinoscratchproxy.cfg" % home, 'r').readlines()
                    cfg = {}
                    for l in lines:
                        l = l.strip()
                        if l.find('=') != -1:
                            parts = l.split('=')
                            cfg[parts[0]] = parts[1]
                    try:
                        debug = int(cfg['debug'])
                    except:
                        pass

                    port = '\\\\.\\COM%s' % (cfg['ArduinoPort'])
                    self.kiku = KiKuBoard()
                    if self.kiku.connect(port) == False:
                        self.kiku = None
                        raise Excption()
                    else:
                        print ("Found KiKu-Board on port: %s" % port)
                except Exception, ex:
                    for p in serial.tools.list_ports.comports():
                        print p

                    data = raw_input("Enter Arduino Port or 'q' to quit: ")
                    if data == 'q':
                        print ("Quit")
                        return
                    port = int(data)
                    cfg = open("%s/arduinoscratchproxy.cfg" % home, 'w')
                    cfg.write("ArduinoPort=%d\n" % port)
                    cfg.write("debug=%d\n" % debug)
                    cfg.close()

            if not self.scr:
                try:
                    self.scr = Scratch()
                except:
                    self.scr = None
                    print ("SCRATCH not running or no remote-sensors connection enabled" )
                    time.sleep(3)


            while self.scr and self.kiku:
                try:
                    self.kiku.poll(0.3)

                    scrmsg = {}
                    for a in range(0, self.kiku.AICnt()):
                        scrmsg['AI%d' % (a )] = self.kiku.getAI(a)
                        # break

                    for a in range(0, self.kiku.DICnt()):
                        scrmsg['DI%d' % (a )] = self.kiku.getDI(a)

                    self.scr.sensorupdate(scrmsg)
                    if debug:
                        print ("SensorData: |%s|" % scrmsg)


                    if self.scr.hasData():
                        m = self.scr.receive()
                        for k, v in m['sensor-update'].iteritems():
                            if k.find('DO') == 0:
                                n = int(k.split('DO')[1])
                                if v == '0':
                                    self.kiku.reset( n)
                                else:
                                    self.kiku.set(n)
                            elif k.find('M') == 0:
                                n = int(k.split('M')[1])
                                self.kiku.motor(n, v)

                        if debug:
                            print ("Received: %s" % m['sensor-update'])
                except Exception, ex:
                    self.kiku.close()
                    self.kiku = None
                    self.scr = None
                    pass


if __name__ == '__main__':
    asp = ArduinoScratchProxy()
    asp.start()



