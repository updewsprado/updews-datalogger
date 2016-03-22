from xbeecoord import *
from time import sleep
import multiprocessing

reset()
sleep(5)
getRssi()
sleep(2)
wakeup()
startwait()




'''
def main():
	reset()
	sleep(5)
	getRssi()
	sleep(2)
	wakeup()
	startwait()

if __name__=='__main__':
	p = multiprocessing.Process(target=main,name="routine")
	p.start()

	sleep(300)
	print "terminating routine"
	
	p.terminate()
'''