#! /usr/bin/python
#this version saves data on text file
#flexi number of sensors

from xbee import XBee, ZigBee
import serial
from time import sleep
import re
from datetime import datetime as dt
from ConfigParser import SafeConfigParser
from struct import *

config = SafeConfigParser()
config.read('LUN_config.txt')


#PORT = '/dev/ttyUSB0'
PORT='COM4'
BAUD_RATE = 9600

DEST_ADDR_LONG = "\x00\x00\x00\x00\x00\x00\xff\xff"
DEST_ADDR_RA = pack('>q',int(config.get('addr_long','ra'),16))
DEST_ADDR_RB = pack('>q',int(config.get('addr_long','rb'),16))
DEST_ADDR_RC = pack('>q',int(config.get('addr_long','rc'),16))
DEST_ADDR_RD = pack('>q',int(config.get('addr_long','rd'),16))
ADDR_RA = config.get('addr', 'ra')
ADDR_RB = config.get('addr', 'rb')
ADDR_RC = config.get('addr', 'rc')
ADDR_RD = config.get('addr', 'rd')
RA = config.get('name', 'ra')
RB = config.get('name', 'rb')
RC = config.get('name', 'rc')
RD = config.get('name', 'rd')

sc = config.get('sensorcount', 'site')

ser = serial.Serial(PORT, BAUD_RATE)
#ser.timeout = 60

# Create API object
xbee = ZigBee(ser,escaped=True)
import pprint
rssrep=""
parDB=0
rssdate=0
endA=0
endB=0
endC=0
endD=0


def getRssi():
	global rssrep
	global rssrepA
	global rssrepB
	global rssrepC
	global rssrepD
	global statDB
	global parDB
	global sc
	
	
	rssrep="COORDINATOR*RSSI,"config.get('name', 'c')
	rssrep=re.sub('[^A-Z0-9\,*]',"",rssrep)

	
	if '1'==sc or '2'==sc or '3'==sc or '4'==sc:
		#A
		rssrepA=""
		xbee.remote_at(							#remote_at RSSI
			dest_addr_long=DEST_ADDR_RA, 			
			command="DB", 
			frame_id="A")
		
		respdb_ra = xbee.wait_read_frame()
		statDB = respdb_ra['status']
		statDB = ord(statDB)
		
		if statDB is 0:
			parDB = respdb_ra['parameter']
			parDB = ord(parDB)
			print RA,"is alive. RSS is -",parDB,"dBm"
			rssrepA = ","+RA
			rssrepA = rssrepA+","+str(ord(respdb_ra['parameter']))
			rssrepA = re.sub('[^A-Zbcxy0-9\,]',"",rssrepA)
		else:
			print "Can't connect to", RA
			rssrepA = ","+RA
			rssrepA = rssrepA+","+"100"
			rssrepA = re.sub('[^A-Zbcxy0-9\,]',"",rssrepA)
		print "rss A is done"
	
	if '2'==sc or '3'==sc or '4'==sc:
		#B
		rssrepB=""
		xbee.remote_at(							#remote_at RSSI
			dest_addr_long=DEST_ADDR_RB, 			
			command="DB", 
			frame_id="A")
		
		respdb_rb = xbee.wait_read_frame()
		statDB = respdb_rb['status']
		statDB = ord(statDB)
		
		if statDB is 0:
			parDB = respdb_rb['parameter']
			parDB = ord(parDB)
			print RB,"is alive. RSS is -",parDB,"dBm"
			rssrepB = ","+RB
			rssrepB = rssrepB+","+str(ord(respdb_rb['parameter']))
			rssrepB = re.sub('[^A-Zbcxy0-9\,]',"",rssrepB)
		else:
			print "Can't connect to", RB
			rssrepB = ","+RB
			rssrepB = rssrepB+","+"100"
			rssrepB = re.sub('[^A-Zbcxy0-9\,]',"",rssrepB)
	
	if '3'==sc or '4'==sc:
		#C
		rssrepC=""
		xbee.remote_at( 							#remote_at RSSI
			dest_addr_long=DEST_ADDR_RC, 			
			command="DB", 
			frame_id="A")
		
		respdb_rc = xbee.wait_read_frame()
		statDB = respdb_rc['status']
		statDB = ord(statDB)
		
		if statDB is 0:
			parDB = respdb_rc['parameter']
			parDB = ord(parDB)
			print RC,"is alive. RSS is -",parDB,"dBm"
			rssrepC = ","+RC
			rssrepC = rssrepC+","+str(ord(respdb_rc['parameter']))
			rssrepC = re.sub('[^A-Zbcxy0-9\,]',"",rssrepC)
		else:
			print "Can't connect to", RC
			rssrepC = ","+RC
			rssrepC = rssrepC+","+"100"
			rssrepC = re.sub('[^A-Zbcxy0-9\,]',"",rssrepC)
	
	if '4'==sc:
		#D
		rssrepD=""
		xbee.remote_at( 							#remote_at RSSI
			dest_addr_long=DEST_ADDR_RD, 			
			command="DB", 
			frame_id="A")
		
		respdb_rd = xbee.wait_read_frame()
		statDB = respdb_rd['status']
		statDB = ord(statDB)
		
		if statDB is 0:
			parDB = respdb_rd['parameter']
			parDB = ord(parDB)
			print RD,"is alive. RSS is -",parDB,"dBm"
			rssrepD = ","+RD
			rssrepD = rssrepD+","+str(ord(respdb_rd['parameter']))
			rssrepD = re.sub('[^A-Zbcxy0-9\,]',"",rssrepD)
		else:
			print "Can't connect to", RD
			rssrepD = ","+RD
			rssrepD = rssrepD+","+"100"
			rssrepD = re.sub('[^A-Zbcxy0-9\,]',"",rssrepD)
	
	return

	
def wakeup():
	#part to discovery shot 16-bit address
	xbee.send("tx",data="000\n",dest_addr_long=DEST_ADDR_LONG,dest_addr="\xff\xfe")
	resp = xbee.wait_read_frame()
	#shot_addr = resp["dest_addr"]
	print "Wake up"

	#sleep(3)


	
def startwait():
	global rssrepA
	global rssrepB
	global rssrepC
	global rssrepD
	global rssdate
	global endA
	global endB
	global endC
	global endD
	
	paddr=""
	dataA=""
	dataB=""
	dataC=""
	dataD=""

	while True:
		try: 
			print "waiting"
			response = xbee.wait_read_frame()
			#print response
			rf = response['rf_data']
			print rf 
			rf=str(rf)
			datalen=len(rf)
			#print int(rf[1],16)

			paddr = paddr + hex(int(ord(response['source_addr_long'][4])))
			paddr = paddr + hex(int(ord(response['source_addr_long'][5])))
			paddr = paddr + hex(int(ord(response['source_addr_long'][6])))
			paddr = paddr + hex(int(ord(response['source_addr_long'][7])))
			
			hashStart=rf.find('#')
			
			#pag may voltage na pinadala
			if rf.find('VOLTAGE') is not -1:
				if paddr == ADDR_RA:
					voltRA = rf[hashStart+1:-1]
					voltRA = re.sub('[^.0-9\*]',"",voltRA)
					rssrepA = rssrepA+","+voltRA
					#f=open("/home/pi/Server/outbox/rssdata.txt", "a")
					f=open("outbox/rssdata.txt","a")
					f.write(rssrepA)
					f.close()
					print ">> Voltage from",RA
				if paddr == ADDR_RB:
					voltRB = rf[hashStart+1:-1]
					voltRB = re.sub('[^.0-9\*]',"",voltRB)
					rssrepB = rssrepB+","+voltRB
					#f=open("/home/pi/Server/outbox/rssdata.txt", "a")
					f=open("outbox/rssdata.txt","a")
					f.write(rssrepB)
					f.close()	
					print ">> Voltage from",RB
				if paddr == ADDR_RC:
					voltRC = rf[hashStart+1:-1]
					voltRC = re.sub('[^.0-9\*]',"",voltRC)
					rssrepC = rssrepC+","+voltRC
					#f=open("/home/pi/Server/outbox/rssdata.txt", "a")
					f=open("outbox/rssdata.txt","a")
					f.write(rssrepC)
					f.close()
					print ">> Voltage from",RC
				if paddr == ADDR_RD:
					voltRD = rf[hashStart+1:-1]
					voltRD = re.sub('[^.0-9\*]',"",voltRD)
					rssrepD = rssrepD+","+voltRD
					#f=open("/home/pi/Server/outbox/rssdata.txt", "a")
					f=open("outbox/rssdata.txt","a")
					f.write(rssrepD)
					f.close()
					print ">> Voltage from",RD
					
			else:
				if rssdate is 0:
					f=open("outbox/rssdata.txt","a")
					f.write("*"+dt.today().strftime("%y%m%d%H%M%S")+"\n")
					f.close()
					rssdate = 1

			
				if paddr == ADDR_RA:								#if packet is A 
					dataA=rf[hashStart+1:-1]
					dataA=re.sub('[^A-Zxyabc0-9\*]',"",dataA)
					f=open("outbox/dataA.txt","a")
					f.write(dataA)
					f.close()
					if rf.find("<") is not -1:
						f=open("outbox/dataA.txt","a")
						f.write("\n")
						f.close()
					print ">> Packet from",RA
				elif paddr == ADDR_RB:								#if packet is B
					dataB=rf[hashStart+1:-1]
					dataB=re.sub('[^A-Zxyabc0-9\*]',"",dataB)
					f=open("outbox/dataB.txt","a")
					f.write(dataB)
					f.close()
					if rf.find("<") is not -1:
						f=open("outbox/dataB.txt","a")
						f.write("\n")
						f.close()
					print ">> Packet from",RB
				
				elif paddr == ADDR_RC:								#if packet is C 
					dataC=rf[hashStart+1:-1]
					dataC=re.sub('[^A-Zxyabc0-9\*]',"",dataC)
					f=open("outbox/dataC.txt","a")
					f.write(dataC)
					f.close()
					if rf.find("<") is not -1:
						f=open("outbox/dataC.txt","a")
						f.write("\n")
						f.close()
					print ">> Packet from",RC
				elif paddr == ADDR_RD:								#if packet is D
					dataD=rf[hashStart+1:-1]
					dataD=re.sub('[^A-Zxyabc0-9\*]',"",dataD)
					f=open("outbox/dataD.txt","a")
					f.write(dataD)
					f.close()
					if rf.find("<") is not -1:
						f=open("outbox/dataD.txt","a")
						f.write("\n")
						f.close()
					print ">> Packet from",RD
				else:
					print ">> Unknown address"
			
			paddr=""
			
			if sc is 1 and endA is 1
				break
			elif sc is 2 and endA is 1 and endB is 1
				break
			elif sc is 3 and endA is 1 and endB is 1 and endC is 1
				break
			elif sc is 4 and endA is 1 and endB is 1 and endC is 1 and endD is 1
				break
			
				
		except KeyboardInterrupt:
			break
			
	ser.close()		

def reset():
	#poweroff
	xbee.remote_at(
	dest_addr_long=DEST_ADDR_LONG, 			
		command="D1",
		parameter='\x04')
		#frame_id="A")
		
	sleep(2)
	
	#poweron
	xbee.remote_at(
		dest_addr_long=DEST_ADDR_LONG, 			
		command="D1",
		parameter='\x05')
		#frame_id="A")
		
	print "Reset done"
	

	