#MAKE CONFIG FILE FOR KIBAWE
from ConfigParser import SafeConfigParser

config = SafeConfigParser()
config.read('cfg_mag.ini')
config.add_section('name')
config.add_section('addr')
config.add_section('addr_long')

config.set('name', 'C', 'CMAG')
config.set('name', 'RA', 'MAGTA')
config.set('name', 'RB', 'MAGTB')
config.set('name', 'RC', 'MAGTC')
config.set('name', 'RD', 'MAGTD')

config.set('addr', 'C', '0x400xe20xde0x9b')     
config.set('addr', 'RA', '0x400xba0xd10xcb')    
config.set('addr', 'RB', '0x400xe20xde0xb7')    
config.set('addr', 'RC', '0x400xba0xd10xba')    
config.set('addr', 'RD', '0x400xb50x9d0x6a')    

config.set('addr_long', 'C', "\x00\x13\xa2\x00\x40\xe2\xde\x9b")     
config.set('addr_long', 'RA', "\x00\x13\xa2\x00\x40\xba\xd1\xcb")    
config.set('addr_long', 'RB', "\x00\x13\xa2\x00\x40\xe2\xde\xb7")    
config.set('addr_long', 'RC', "\x00\x13\xa2\x00\x40\xba\xd1\xba")    
config.set('addr_long', 'RD', "\x00\x13\xa2\x00\x40\xb5\x9d\x6a")  

#config.set('main', 'MAGTB', '789')

with open('cfg_mag.ini', 'w') as f:
	config.write(f)