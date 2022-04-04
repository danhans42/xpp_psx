# xpp v0.1 - xplorer control utility

 by @danhans42 - find me on discord - http://psx.dev

 Simple app that can talk to an Xplorer/Xploder cart using raspberry pi gpio. It works with both PSX and N64 varieties, on 
 all known firmware versions. N64 is however very limited on what it will work with (only two commands).

 As this code evolves, it should tidy up naturally and will start to have more Unirom+XP specific funtions

 sketchy - but works. Only meant as a test to see if it was straightforward, and as a 
 testbed for something ESP32 based.

 Works on all 40pin GPIO Raspberry Pis that are compatible with WiringPi. 

 You can probably try this on anything WiringPi has been ported to with minimal effort - mileage may vary.

 Parts Shopping List
 ===================
 
 - [ ] PSX/N64 [of course]
 - [ ] Xplorer Cart of some description
 - [ ] RaspberryPi (ZeroW is ideal) running Linux (only tested on raspbian-minimal)
 - [ ] Level shifters (possibly - see hardware)
 - [ ] F-F Dupont leads for wiring up ( 14 if running 3.3v/XP64 or ~32 for level shifters)  


 Hardware
 ========

 Wire up via level shifters (observe signal direction if not bidirectional).
 
 Alternatively cut the VCC pins to Xplorer 74HC373/74HC245 and connect to +3.3v pin on the PCB/PIO
 connector solder joint. I have done this mod, and it still works with my parallel port perfectly
 on my machine. However do this at your own risk.

 If you only want Xplorer64.. it uses 3.3v levels on its DB25 - so no level shiftery needed there.

 Do not try and be a hero and run the rasberry pi without the +3.3v mod or level shifters.. Pi GPIO
 is not 5v tolerant
 
 Pinouts/Connections
 ===================
 
  |      Name|    BCM IO | RPi GPIO PIN  | Xplorer DB25 PIN|
  |----------|-----------|---------------|-----------------|
  |       D0 |     17    | 11            |   2             |
  |       D1 |     18    | 12            |   3             |             
  |       D2 |     27    | 13            |   4             |
  |       D3 |     22    | 15            |   5             |
  |       D4 |     23    | 16            |   6             |
  |       D5 |     24    | 18            |   7             |
  |       D6 |     25    | 22            |   8             |
  |       D7 |      4    | 7             |   9             |
  |      SEL |     12    | 32            |  17             |
  |     SLCT |     16    | 36            |  13             |
  |       PE |     20    | 38            |  12             |
  |     BUSY |     21    | 40            |  11             |
  |      ACK |      5    | 29            |  10             |
  |      GND |      -    | 6,9,25,30     |  18-25          |   
  

 Building 'xpp'
 ============== 

 On your Pi ensure you have wiringpi installed - do an 'sudo apt get wiringpi' before building
 run ./build.sh to build

 Ensure you dont have any pins enabled that we are going to use. I recommend making sure SPI/I2C is
 disabled. You can configure this with 'sudo raspi-config' in the interface menu.
 
 Compatibility
 =============

 Xplorers are funny fuckers - some stuff works on some versions and not on others. Mileage may vary.
 Xplorer64 only supports flash/status/upload&exec bin commands.


  
 
