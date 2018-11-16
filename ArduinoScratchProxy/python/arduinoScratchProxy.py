from os.path import expanduser
from Scratch import Scratch
import serial
import time
import json
import ast
import sys

debug = 0
home = './'  # expanduser("~")
ser = None
while True:
    try:
        lines = open("%s/arduinoscratchproxy.cfg"% home,'r').readlines()
        cfg={}
        for l in lines:
            if l.find('=') != -1:
                parts = l.split('=')
                cfg[parts[0]] = parts[1]
        try:
            debug  = int(cfg['debug'])
        except:
            pass

        port = '\\\\.\\COM%s' % (cfg['ArduinoPort'])
        ser = serial.Serial(port)
        time.sleep(3)
        ser.write('#')  # Identify
        time.sleep(1)
        msg = ser.read_all()
        if msg.find('ASP Ver') != -1:
            print ("Found on port: %s" % port)
            break
        else:
            ser.close()
            ser = None



    except Exception, ex:
        data = raw_input("Enter Arduino Port or 'q' to quit: ")
        if data == 'q':
            print ("Quit")
            exit(1)
        port = int(data)
        cfg = open("%s/arduinoscratchproxy.cfg" % home, 'w')
        cfg.write("ArduinoPort=%d" % port)
        cfg.close()

if not ser:
    print ("No Arduino Scratch Proxy connected.")
    exit(1)

try:
    s = Scratch()

    Output = {}

    while True:
        time.sleep(0.1)
        ser.write('g')  #g - read all inputs
        time.sleep(0.1)
        msg = ser.read_all()
        if len(msg)<1:
            continue

        if debug:
            print ("M:%s" % msg)
        try:
            sensor_data = ast.literal_eval(str(msg))

            scrmsg = {}
            for a in range(0, len(sensor_data['AI'])):
                scrmsg['A%d'%(a+1)] = sensor_data['AI'][a]
                #break

            for a in range(0, len(sensor_data['DI'])):
                #if 'DO%d'%a in Output:
                #   continue  # Skip pin defined as output
                scrmsg['DI%d'%(a+2)] = sensor_data['DI'][a]
                #break

            s.sensorupddate(scrmsg)
            if debug:
                print ("SensorData: |%s|" % scrmsg)

        except Exception, ex:
            pass
        if s.hasData():
            m = s.receive()
            for k,v in m['sensor-update'].iteritems():
                if k.find('DO') == 0:
                    n= int(k.split('DO')[1])
                    if k in Output:
                        if v == '0':
                            ser.write('r%d'%n)
                        else:
                            ser.write('s%d'%n)
                    else:
                        ser.write('co%d'%n)

                    Output[k] = v

                elif k.find('M') == 0:
                    n= int(k.split('M')[1])
                    ser.write('m%d %s'%(n,v))


                    Output[k] = v

            if debug:
                print ("Received: %s" % m['sensor-update'])
except:
    print ("Error connecting Scratch or serial port communication problem")

print ("END")