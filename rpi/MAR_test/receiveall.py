#! /usr/bin/python
#this version saves data on text file


from xbee import XBee, ZigBee
import serial
from time import sleep
import multiprocessing
from ConfigParser import SafeConfigParser

config = SafeConfigParser()
config.read('cfg_mag.ini')

from wakeup_rpi_v2 import *

PORT = config.get('port', 'xb')
#PORT='COM6'
BAUD_RATE = 9600

DEST_ADDR_RA = config.get('addr_long', 'ra')
DEST_ADDR_RB = config.get('addr_long', 'rb')
ADDR_RA = config.get('addr', 'ra')
ADDR_RB = config.get('addr', 'rb')
RA = config.get('name', 'ra')
RB = config.get('name', 'rb')

dev1=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
dev2=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
data=''
delimstart=0
delimend=0
delimAstart=0
delimAend=0
delimBstart=0
delimBend=0
ranid_a1="000"
ranid_b1="000"
end_seq1=0
end_seq2=0
sentA=0
sentB=0
sendFlagA=0
sendFlagB=0

DEST_ADDR_RA = config.get('addr_long', 'ra')
DEST_ADDR_RB = config.get('addr_long', 'rb')
ADDR_RA = config.get('addr', 'ra')
ADDR_RB = config.get('addr', 'rb')
RA = config.get('name', 'ra')
RB = config.get('name', 'rb')

# Open serial port and enable flow control
#ser = serial.Serial(PORT, BAUD_RATE, bytesize=8, parity='N', stopbits=1, timeout=None, xonxoff=1, rtscts=1, dsrdtr=1)
ser = serial.Serial(PORT, BAUD_RATE)
ser.timeout = 60
# Create API object
xbee = ZigBee(ser,escaped=True)
import pprint
import re

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
				if rf.find('#') is not -1
					dataA=rf[hashStart+1:-1]
				else
					dataA=rf[1:-1]
				
				dataA=re.sub('[^A-Za-z0-9\*]',"",dataA)
				f=open("/home/pi/Server/outbox/dataA.txt","a")
				f.write(dataA)
				f.close()
				
				if rf.find("<<") is not -1:
					f=open("/home/pi/Server/outbox/dataA.txt","a")
					f.write("\n")
					f.close()
				print ">> Packet from",RA
			elif paddr == ADDR_RB:								#if packet is B
				if rf.find('#') is not -1
					dataB=rf[hashStart+1:-1]
				else
					dataB=rf[1:-1]
					
				dataB=re.sub('[^A-Za-z0-9\*]',"",dataB)
				f=open("/home/pi/Server/outbox/dataB.txt","a")
				f.write(dataB)
				f.close()
				
				if rf.find("<<") is not -1:
					f=open("outbox/dataB.txt","a")
					f.write("\n")
					f.close()
				print ">> Packet from",RB
				
		except KeyboardInterrupt:
			break
			
	ser.close()		
	
if __name__=='__main__':
  p = multiprocessing.Process(target=startwait,name="receiveall")
  p.start()

  sleep(120)
  print "terminating receiveall"

  p.terminate()
