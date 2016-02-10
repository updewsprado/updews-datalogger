#! /usr/bin/python
#this version saves data on text file
#four routers

from xbee import XBee, ZigBee
import serial
from time import sleep
import re
from datetime import datetime as dt
#import multiprocessing
from ConfigParser import SafeConfigParser

config = SafeConfigParser()
config.read('cfg_mag.ini')

#PORT = '/dev/ttyUSB0'
PORT='COM11'
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
	#print respdb_ra
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

'''
if __name__=='__main__':
  p = multiprocessing.Process(target=startwait,name="receiveall")
  p.start()

  sleep(120)
  print "terminating receiveall"

  p.terminate()
'''
	
getRssi()
wakeup()
startwait()
#ser.close()
