#! /usr/bin/python
#this version saves data on text file


from xbee import XBee, ZigBee
import serial
from time import sleep
import multiprocessing

from wakeup_rpi_v2 import *

PORT = '/dev/ttyUSB0'
#PORT='COM6'
BAUD_RATE = 9600


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

DEST_ADDR_R1 = 0x0013a20040bad1c3L
DEST_ADDR_R2 = 0x0013a20040b59d4cL

# Open serial port and enable flow control
#ser = serial.Serial(PORT, BAUD_RATE, bytesize=8, parity='N', stopbits=1, timeout=None, xonxoff=1, rtscts=1, dsrdtr=1)
ser = serial.Serial(PORT, BAUD_RATE)
ser.timeout = 60
# Create API object
xbee = ZigBee(ser,escaped=True)
import pprint
import re

def startwait():
	global dev1
	global dev2
	global data
	global delimAstart
	global delimAend
	global delimBstart
	global delimBend
	global ranid_a1
	global ranid_b1
	global end_seq1
	global end_seq2
	global sentA
	global sentB
	global sendFlagA
	global sendFlagB

	while True:
		try: 
			print "waiting"
			response = xbee.wait_read_frame()
			print response
			rf = response['rf_data']
			print rf 
			rf=str(rf)
			datalen=len(rf)
			#print int(rf[1],16)

			'''
			if (rf[0] == 'A'):					#check if data is from new set for A
				ranid_a1=rf[6]
				dev1=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]					#if yes, empty the current data
				end_seq1=0 
				sentA=0
				print "this is a new set for A\n"
			if (ranid_b1 != rf[6] and rf[0] == 'B'):					#check if data is from new set for B
				ranid_b1=rf[6]
				dev2=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]					#if yes, empty the current data
				end_seq2=0 
				sentB=0
				print "this is a new set for B\n"
			'''
			p = response['source_addr_long']
			if str(hex(ord(p[-1]))) == '0xc3':											#if packet is A 
				delimAstart=rf.find(">>")						#check if first packet
				delimAend=rf.find("<<")							#check if last packet
				print ">> Packet from MARTA"
			elif str(hex(ord(p[-1]))) == '0x4c':											#if packet is B
				delimBstart=rf.find(">>")						#check if first packet
				delimBend=rf.find("<<")							#check if last packet								#check if last packet 
				print ">> Packet from MARTB"
			else:
				print ">> Unknown address"
			'''
			delimstart=rf.find(">>")
			delimend=rf.find("<<")
			'''
			hashStart=rf.find('#')
			

			
			#dataA = re.sub('[^A-Zbcxy0-9\*]',"",dataA)
			if delimAstart is not -1 and delimAend is not -1:
				dataA=rf[hashStart+1:-1]
				dataA=re.sub('[^A-Zxyabc0-9\*]',"",dataA)
				print dataA
				f=open("/home/pi/Server/outbox/dataA.txt", "a")
				f.write(dataA)
				f.write("\n")
				f.close()
				delimAstart=-1
				delimAend=-1
			elif delimAstart is not -1:
				dataA=rf[hashStart+1:-1]
				dataA=re.sub('[^A-Zxyabc0-9\*]',"",dataA)
				print dataA
				delimAstart=-1
			elif delimAend is not -1:
				dataA=dataA+rf[1:-1]
				dataA=re.sub('[^A-Zxyabc0-9\*]',"",dataA)
				f=open("/home/pi/Server/outbox/dataA.txt", "a")
				f.write(dataA)
				f.write("\n")
				f.close()
				delimAend=-1
				
			
			#dataB = re.sub('[^A-Zbcxy0-9\*]',"",data)
			if delimBstart is not -1 and delimBend is not -1:
				dataB=rf[hashStart+1:-1]
				dataB=re.sub('[^A-Zxyabc0-9\*]',"",dataB)
				print dataB
				f=open("/home/pi/Server/outbox/dataB.txt", "a")
				f.write(dataB)
				f.write("\n")
				f.close()
				delimBstart=-1
				delimBend=-1
			elif delimBstart is not -1:
				dataB=rf[hashStart+1:-1]
				dataB=re.sub('[^A-Zxyabc0-9\*]',"",dataB)
				print dataB
				delimBstart=-1
			elif delimBend is not -1:
				dataB=dataB+rf[1:-1]
				dataB=re.sub('[^A-Zxyabc0-9\*]',"",dataB)
				f=open("/home/pi/Server/outbox/dataB.txt", "a")
				f.write(dataB)
				f.write("\n")
				f.close()
				delimBend=-1
			
			
				
				
		except KeyboardInterrupt:
			break
	ser.close()		
	
if __name__=='__main__':
  p = multiprocessing.Process(target=startwait,name="receiveall")
  p.start()

  sleep(120)
  print "terminating receiveall"

  p.terminate()
