from xbeecoord import *
import multiprocessing





def main():
	poweron()
	#getRssi()
	#wakeup()
	#startwait()
	#poweroff()


if __name__=='__main__':
	p = multiprocessing.Process(target=main,name="routine")
	p.start()

	sleep(10)
	print "terminating routine"
	
	poweroff()
	sleep(1)
	p.terminate()
