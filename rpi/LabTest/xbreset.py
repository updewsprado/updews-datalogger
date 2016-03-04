from xbeecoord import getRssi, poweron, poweroff
from time import sleep

sleep(5)
poweron()
print "Turning on D1 pin"
sleep(10)
poweroff()
print "Turning off D1 pin"