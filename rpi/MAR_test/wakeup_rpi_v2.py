#! /usr/bin/python
#this version saves data on text file


from xbee import XBee, ZigBee
import serial
from time import sleep
import re
from datetime import datetime as dt

from ConfigParser import SafeConfigParser

config = SafeConfigParser()
config.read('cfg_mag.ini')

PORT = config.get('port', 'xb')
#PORT='COM6'
BAUD_RATE = 9600

DEST_ADDR_LONG = '\x00\x00\x00\x00\x00\x00\xff\xff'

DEST_ADDR_RA = config.get('addr_long', 'ra')
DEST_ADDR_RB = config.get('addr_long', 'rb')
ADDR_RA = config.get('addr', 'ra')
ADDR_RB = config.get('addr', 'rb')
RA = config.get('name', 'ra')
RB = config.get('name', 'rb')

ser = serial.Serial(PORT, BAUD_RATE)

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
	rssrep="COORDINATOR*,"+config.get('name', 'c')
	f=open("/home/pi/Server/outbox/rssdata.txt", "a")
	#f=open("outbox/rssdata.txt","a")
	f.write(rssrep)
	f.close()
	rssrep=""
	
	#rssrep=rssrep+config.get('name', 'c')
	
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
		f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		#f=open("outbox/rssdata.txt","a")
		f.write(rssrep)
		f.close()
	else:
		print "Can't connect to", RA
		rssrep = rssrep+","+RA
		rssrep = rssrep+","+"100"
		rssrep=re.sub('[^A-Zbcxy0-9\,]',"",rssrep)
		f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		#f=open("outbox/rssdata.txt","a")
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
		f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		#f=open("outbox/rssdata.txt","a")
		f.write(rssrep)
		f.close()
	else:
		print "Can't connect to", RB
		rssrep = rssrep+","+RB
		rssrep = rssrep+","+"100"
		rssrep=re.sub('[^A-Zbcxy0-9\,]',"",rssrep)
		f=open("/home/pi/Server/outbox/rssdata.txt", "a")
		#f=open("outbox/rssdata.txt","a")
		f.write(rssrep)
		f.close()
	rssrep=""
	
	f=open("/home/pi/Server/outbox/rssdata.txt", "a")
	#f=open("outbox/rssdata.txt","a")
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
