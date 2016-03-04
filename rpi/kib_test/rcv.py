#! /usr/bin/python
#this version saves data on text file
#four routers

from xbee import XBee, ZigBee
import serial
from time import sleep
import re
from datetime import datetime as dt
import multiprocessing
from wup import *
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



def startwait():
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
			
			if paddr == ADDR_RA:								#if packet is A 
				dataA=rf[hashStart+1:-1]
				dataA=re.sub('[^A-Zxyabc0-9\*]',"",dataA)
				f=open("outbox/dataA.txt","a")
				f.write(dataA)
				f.close()
				if rf.find("<<") is not -1:
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
				if rf.find("<<") is not -1:
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
				if rf.find("<<") is not -1:
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
				if rf.find("<<") is not -1:
					f=open("outbox/dataD.txt","a")
					f.write("\n")
					f.close()
				print ">> Packet from",RD
			else:
				print ">> Unknown address"

			paddr=""
			
				
		except KeyboardInterrupt:
			break
			
	ser.close()		

	
if __name__=='__main__':
  p = multiprocessing.Process(target=startwait,name="rcv")
  p.start()

  sleep(120)
  print "terminating rcv"

  p.terminate()
