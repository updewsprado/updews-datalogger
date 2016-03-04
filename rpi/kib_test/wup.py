#! /usr/bin/python
#this version saves data on text file
#four routers

from xbee import XBee, ZigBee
import serial
from time import sleep
import re
from datetime import datetime as dt
from ConfigParser import SafeConfigParser

config = SafeConfigParser()
config.read('cfg_mag.ini')

#PORT = config.get('port', 'xb')
PORT='COM3'
BAUD_RATE = 9600

DEST_ADDR_LONG = '\x00\x00\x00\x00\x00\x00\xff\xff'
DEST_ADDR_RA = config.get('addr_long', 'ra')
DEST_ADDR_RB = config.get('addr_long', 'rb')
DEST_ADDR_RC = config.get('addr_long', 'rc')
DEST_ADDR_RD = config.get('addr_long', 'rd')
ADDR_RA = config.get('addr', 'ra')
ADDR_RB = config.get('addr', 'rb')
ADDR_RC = config.get('addr', 'rc')
ADDR_RD = config.get('addr', 'rd')
RA = config.get('name', 'ra')
RB = config.get('name', 'rb')
RC = config.get('name', 'rc')
RD = config.get('name', 'rd')

ser = serial.Serial(PORT, BAUD_RATE)
ser.timeout = 60

# Create API object
xbee = ZigBee(ser,escaped=True)
import pprint
rssrep=""
parDB=0





def getRssi():
	global rssrep
	global statDB
	global parDB
	
	rssrep=re.sub('[^A-Z0-9\,]',"",rssrep)
	#f=open("/home/pi/Server/outbox/rssdata.txt", "a")
	f=open("outbox/rssdata.txt","a")
	f.write(rssrep)
	f.close()
	
	rssrep=rssrep+config.get('name', 'c')
	
	#A
	xbee.remote_at(dest_addr='\xff\xfe', 							#remote_at RSSI
		dest_addr_long=DEST_ADDR_RA, 			
		command="DB", 
		frame_id="A")
	
	respdb_ra = xbee.wait_read_frame()
	#print respdb_ra
	statDB = respdb_ra['status']
	statDB = ord(statDB)
	
	if statDB is 0:
		parDB = respdb_ra['parameter']
		parDB = ord(parDB)
		print RA,"is alive. RSS is -",parDB,"dBm"
		rssrep = rssrep+","+RA
		rssrep = rssrep+","+str(ord(respdb_ra['parameter']))
		rssrep=re.sub('[^A-Zbcxy0-9\,]',"",rssrep)
		#f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		f=open("outbox/rssdata.txt","a")
		f.write(rssrep)
		f.close()
	else:
		print "Can't connect to", RA
		rssrep = rssrep+","+RA
		rssrep = rssrep+","+"100"
		rssrep=re.sub('[^A-Zbcxy0-9\,]',"",rssrep)
		#f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		f=open("outbox/rssdata.txt","a")
		f.write(rssrep)
		f.close()
	rssrep=""
	
	#B
	xbee.remote_at(dest_addr='\xff\xfe', 							#remote_at RSSI
		dest_addr_long=DEST_ADDR_RB, 			
		command="DB", 
		frame_id="A")
	
	respdb_rb = xbee.wait_read_frame()
	#print respdb_rb
	statDB = respdb_rb['status']
	statDB = ord(statDB)
	
	if statDB is 0:
		parDB = respdb_rb['parameter']
		parDB = ord(parDB)
		print RB,"is alive. RSS is -",parDB,"dBm"
		rssrep = rssrep+","+RB
		rssrep = rssrep+","+str(ord(respdb_rb['parameter']))
		rssrep=re.sub('[^A-Zbcxy0-9\,]',"",rssrep)
		#f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		f=open("outbox/rssdata.txt","a")
		f.write(rssrep)
		f.close()
	else:
		print "Can't connect to", RB
		rssrep = rssrep+","+RB
		rssrep = rssrep+","+"100"
		rssrep=re.sub('[^A-Zbcxy0-9\,]',"",rssrep)
		#f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		f=open("outbox/rssdata.txt","a")
		f.write(rssrep)
		f.close()
	rssrep=""
	
	#C
	xbee.remote_at(dest_addr='\xff\xfe', 							#remote_at RSSI
		dest_addr_long=DEST_ADDR_RC, 			
		command="DB", 
		frame_id="A")
	
	respdb_rc = xbee.wait_read_frame()
	#print respdb_ra
	statDB = respdb_rc['status']
	statDB = ord(statDB)
	
	if statDB is 0:
		parDB = respdb_rc['parameter']
		parDB = ord(parDB)
		print RC,"is alive. RSS is -",parDB,"dBm"
		rssrep = rssrep+","+RC
		rssrep = rssrep+","+str(ord(respdb_rc['parameter']))
		rssrep=re.sub('[^A-Zbcxy0-9\,]',"",rssrep)
		#f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		f=open("outbox/rssdata.txt","a")
		f.write(rssrep)
		f.close()
	else:
		print "Can't connect to", RC
		rssrep = rssrep+","+RC
		rssrep = rssrep+","+"100"
		rssrep=re.sub('[^A-Zbcxy0-9\,]',"",rssrep)
		#f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		f=open("outbox/rssdata.txt","a")
		f.write(rssrep)
		f.close()
	rssrep=""
	
	#D
	xbee.remote_at(dest_addr='\xff\xfe', 							#remote_at RSSI
		dest_addr_long=DEST_ADDR_RD, 			
		command="DB", 
		frame_id="A")
	
	respdb_rd = xbee.wait_read_frame()
	#print respdb_ra
	statDB = respdb_rd['status']
	statDB = ord(statDB)
	
	if statDB is 0:
		parDB = respdb_rd['parameter']
		parDB = ord(parDB)
		print RD,"is alive. RSS is -",parDB,"dBm"
		rssrep = rssrep+","+RD
		rssrep = rssrep+","+str(ord(respdb_rd['parameter']))
		rssrep=re.sub('[^A-Zbcxy0-9\,]',"",rssrep)
		#f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		f=open("outbox/rssdata.txt","a")
		f.write(rssrep)
		f.close()
	else:
		print "Can't connect to", RD
		rssrep = rssrep+","+RD
		rssrep = rssrep+","+"100"
		rssrep=re.sub('[^A-Zbcxy0-9\,]',"",rssrep)
		#f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		f=open("outbox/rssdata.txt","a")
		f.write(rssrep)
		f.close()
	rssrep=""

	
	#f=open("/home/pi/Server/outbox/rssdata.txt", "a")
	f=open("outbox/rssdata.txt","a")
	f.write("*"+dt.today().strftime("%y%m%d%H%M%S")+"\n")
	f.close()
	#return statDB
	return




	
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

	
