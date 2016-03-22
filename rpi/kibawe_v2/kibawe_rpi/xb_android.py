from xbee import XBee, ZigBee
import serial
from time import sleep
import serial.tools.list_ports


#PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600

ports = list(serial.tools.list_ports.comports())
for p in ports:
    # The SparkFun XBee Explorer USB board uses an FTDI chip as USB interface
    if "FTDIBUS" in p[2]:
        print "Found possible XBee on " + p[0]
        if not portfound:
            portfound = True
            portname = p[0]
            print "Using " + p[0] + " as XBee COM port."
        else:
            print "Ignoring this port, using the first one that was found."
 
if portfound:
    ser = serial.Serial(portname, 9600)
else:
    sys.exit("No serial port seems to have an XBee connected.")

ser = serial.Serial(PORT, BAUD_RATE)
xbee = ZigBee(ser=ser, escaped=True)



xbee.at(command="AT")

print xbee.wait_read_frame()