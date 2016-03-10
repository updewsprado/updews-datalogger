from xbeecoord import *
from time import sleep
#import multiprocessing

reset()
sleep(3)
getRssi()
sleep(1)
wakeup()
startwait()


'''
def main():
	#reset()
	#getRssi()
	#wakeup()
	#startwait()

if __name__=='__main__':
	p = multiprocessing.Process(target=main,name="routine")
	p.start()

	sleep(10)
	print "terminating routine"
	
	p.terminate()
'''