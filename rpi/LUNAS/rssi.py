from xbeecoord import getRssi
from time import sleep

while True:
	getRssi()
	sleep(2)
