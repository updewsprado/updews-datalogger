#MAKE CONFIG FILE FOR KIBAWE
from ConfigParser import SafeConfigParser

config = SafeConfigParser()
config.read('cfg_mar.ini')
config.add_section('name')
config.add_section('addr')
config.add_section('addr_long')
config.add_section('port')

config.set('name', 'C', 'MAR')
config.set('name', 'RA', 'MARTA')
config.set('name', 'RB', 'MARTB')

config.set('addr', 'C', '0x400xba0xd10xd0')     
config.set('addr', 'RA', '0x400xba0xd10xc3')    
config.set('addr', 'RB', '0x400xb50x9d0x4c')    

config.set('addr_long', 'C', "\x00\x13\xa2\x00\x40\xba\xd1\xd0")     
config.set('addr_long', 'RA', "\x00\x13\xa2\x00\x40\xba\xd1\xc3")    
config.set('addr_long', 'RB', "\x00\x13\xa2\x00\x40\xb5\x9d\x4c")  

config.set('port', 'xb', '/dev/xbeeusbport')

with open('cfg_mag.ini', 'w') as f:
	config.write(f)