#MAKE CONFIG FILE FOR KIBAWE
from ConfigParser import SafeConfigParser

config = SafeConfigParser()
config.read('cfg_lab.ini')
config.add_section('name')
config.add_section('addr')
config.add_section('addr_long')
config.add_section('sensorcount')

config.set('name', 'C', 'LAB')
config.set('name', 'RA', 'LABTA')
config.set('name', 'RB', 'LABTB')
config.set('name', 'RC', 'LABTC')
config.set('name', 'RD', 'LABTD')


config.set('addr', 'C', '0x400xe20xde0xa0')     
config.set('addr', 'RA', '0x400xe20xde0xb2')    
config.set('addr', 'RB', '0x400xe20xde0xae')       
config.set('addr', 'RC', '')    
config.set('addr', 'RD', '')   

config.set('addr_long', 'C', "\x00\x13\xa2\x00\x40\xe2\xde\xa0")     
config.set('addr_long', 'RA', "\x00\x13\xa2\x00\x40\xe2\xde\xb2")    
config.set('addr_long', 'RB', "\x00\x13\xa2\x00\x40\xe2\xde\xae") 
config.set('addr_long', 'RC', "")    
config.set('addr_long', 'RD', "") 

config.set('sensorcount', 'site', '2')   


with open('cfg_lab.ini', 'w') as f:
	config.write(f)