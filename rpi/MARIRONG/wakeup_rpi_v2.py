#! /usr/bin/python
#this version saves data on text file


from xbee import XBee, ZigBee
import serial
from time import sleep
import re
from datetime import datetime as dt

PORT = '/dev/ttyUSB0'
#PORT='COM6'
BAUD_RATE = 9600

DEST_ADDR_LONG = '\x00\x00\x00\x00\x00\x00\xff\xff'

DEST_ADDR_R1 = '\x00\x13\xa2\x00\x40\xba\xd1\xc3'
DEST_ADDR_R2 = '\x00\x13\xa2\x00\x40\xb5\x9d\x4c'
DEST_ADDR_R3 = '\x00\x13\xa2\x00\x40\xe2\xde\xb7'

ser = serial.Serial(PORT, BAUD_RATE)

# Create API object
xbee = ZigBee(ser,escaped=True)
import pprint
rssrep="CMAR"
parDB=0

def getRssi():
	global rssrep
	global statDB
	global parDB
	
	rssrep=re.sub('[^A-Z0-9\,]',"",rssrep)
	f=open("/home/pi/Server/outbox/rssdata.txt", "a")
	f.write(rssrep)
	f.close()
	
	rssrep=""

	xbee.remote_at(dest_addr='\xff\xfe', 							#remote_at RSSI
		dest_addr_long=DEST_ADDR_R1, 			
		command="DB", 
		frame_id="A")
	
	respDB_r1 = xbee.wait_read_frame()
	print respDB_r1
	statDB = respDB_r1['status']
	statDB = ord(statDB)
	
	if statDB is 0:
		parDB = respDB_r1['parameter']
		parDB = ord(parDB)
		print "MARTA is alive. RSS is -",parDB,"dBm"
		rssrep = rssrep+",MARTA,"
		print rssrep
		rssrep = rssrep+str(ord(respDB_r1['parameter']))
		print rssrep
		rssrep=re.sub('[^A-Zbcxy0-9\,]',"",rssrep)
		print rssrep
		f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		f.write(rssrep)
		f.close()
	else:
		print "Can't connect to MARTA"
		rssrep = rssrep+",MARTA,100"
		print rssrep
		rssrep=re.sub('[^A-Zbcxy0-9\,]',"",rssrep)
		print rssrep
		f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		f.write(rssrep)
		f.close()
	rssrep=""
	
	xbee.remote_at(dest_addr='\xff\xfe', 							#remote_at RSSI
		dest_addr_long=DEST_ADDR_R2, 			
		command="DB", 
		frame_id="A")
	
	respDB_r2 = xbee.wait_read_frame()
	print respDB_r2
	statDB = respDB_r2['status']
	statDB = ord(statDB)
	
	if statDB is 0:
		parDB = respDB_r2['parameter']
		parDB = ord(parDB)
		print "MARTB is alive. RSS is -",parDB,"dBm"
		rssrep = rssrep+",MARTB,"
		print rssrep
		rssrep = rssrep+str(ord(respDB_r2['parameter']))
		print rssrep
		rssrep=re.sub('[^A-Z0-9\,]',"",rssrep)
		print rssrep
		f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		f.write(rssrep)
		f.close()
	else:
		print "Can't connect to MARTB"
		rssrep = rssrep+",MARTB,100"
		print rssrep
		rssrep=re.sub('[^A-Z0-9\,]',"",rssrep)
		print rssrep
		f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		f.write(rssrep)
		f.close()
	
	f=open("/home/pi/Server/outbox/rssdata.txt", "a")
	f.write("*"+dt.today().strftime("%y%m%d%H%M%S")+"\n")
	f.close()
	return statDB

	
def wakeup():
	#part to discovery shot 16-bit address
	xbee.send("tx",data="000\n",dest_addr_long=DEST_ADDR_LONG,dest_addr="\xff\xfe")
	resp = xbee.wait_read_frame()
	#shot_addr = resp["dest_addr"]
	print "Wake up"

	sleep(3)

	
getRssi()
wakeup()
ser.close()
