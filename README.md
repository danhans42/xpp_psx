# xpp v0.1 - xplorer control utility

 by @danhans42 - find me on discord - http://psx.dev

 Simple app that can talk to an Xplorer/Xploder cart using raspberry pi gpio. It works with both PSX and N64 varieties, on 
 all known firmware versions. N64 is however very limited on what it will work with (only two commands).

 As this code evolves, it should tidy up naturally and will start to have more Unirom+XP specific funtions

 sketchy - but works. Only meant as a test to see if it was straightforward, and as a 
 testbed for something ESP32 based.

 Works on all 40pin GPIO Raspberry Pis that are compatible with WiringPi. 

 You can probably try this on anything WiringPi has been ported to with minimal effort - mileage may vary.

 Hardware Needed
 ===============
 
 - [ ] PSX/N64 [of course]
 - [ ] Xplorer/Xploder Cart of some description
 - [ ] RaspberryPi (ZeroW is ideal) running Linux (only tested on raspbian-minimal)
 - [ ] Level shifters (possibly - see hardware)
 - [ ] F-F Dupont leads for wiring up ( 14 if running 3.3v/XP64 or ~32 for level shifters)  


 Hardware Connections
 ====================

 Wire up via level shifters (observe signal direction if not bidirectional).
 
 Alternatively cut the VCC pins to Xplorer 74HC373/74HC245 and connect to +3.3v pin on the PCB/PIO
 connector solder joint. I have done this mod, and it still works with my parallel port perfectly
 on my machine. However do this at your own risk.

 If you only want Xplorer64.. it uses 3.3v levels on its DB25 - so no level shiftery needed there.

 Do not try and be a hero and run the rasberry pi without the +3.3v mod or level shifters.. Pi GPIO
 is NOT 5v tolerant.
 
 Check the orientation of your particular Pi Device.
 
 
  |      Name|    BCM IO | RPi GPIO PIN  | Xplorer DB25 PIN|  Direction  |
  |----------|-----------|---------------|-----------------|-------------|
  |       D0 |     17    | 11            |   2             |  OUT        |
  |       D1 |     18    | 12            |   3             |  OUT        |             
  |       D2 |     27    | 13            |   4             |  OUT        |
  |       D3 |     22    | 15            |   5             |  OUT        |
  |       D4 |     23    | 16            |   6             |  OUT        |
  |       D5 |     24    | 18            |   7             |  OUT        |
  |       D6 |     25    | 22            |   8             |  OUT        |
  |       D7 |      4    | 7             |   9             |  OUT        |
  |      SEL |     12    | 32            |  17             |  OUT        |
  |     SLCT |     16    | 36            |  13             |  IN         | 
  |       PE |     20    | 38            |  12             |  IN         |
  |     BUSY |     21    | 40            |  11             |  IN         |
  |      ACK |      5    | 29            |  10             |  IN         |
  |      GND |      -    | 6,9,25,30     |  18-25          |  IN         | 
  
  
 Direction above is relative to the Raspberry Pi.
  
 For level shifting you can use something like the cheap level shifters you find on aliexpress/ebay etc - just make sure to observe the signal direction    if not bidirectional. Something like TXB0108 or BSS138 based works well. 
 
 
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
 
 Commands/Features
 ================
 
 |Command                                     |FX4.2 |PRO 2.0   |XP_1.9  |XP_64    |
 |--------------------------------------------|------|----------|--------|---------|                               
 /status                                      |  *   |    *     |   *    |   *     |  
 /mcstatus*                                   |  *   |    X     |   X    |   X     |   
 /reboot                                      |  *   |    *     |   *    |   X     |        
 /freeze*                                     |  *   |    *     |   *    |   X     |        
 /unfreeze*                                   |  *   |    *     |   *    |   X     |     
 /peek <address>                              |  *   |    *     |   *    |   X     |     
 /view <address>                              |  *   |    *     |   *    |   X     |        
 /upload <binfile>                            |  *   |    *     |   *    |   X     |       
 /download <outfile> <address> <length>       |  *   |    *     |   *    |   X     |        
 /execbin <binfile> <address>                 |  *   |    *     |   *    |   *     |        
 /exe <psxexe>                                |  *   |    *     |   *    |   X     |
 
   * = Tested OK     X = Incompatible     ? Untested

As well as the above, the software can work with xflash for some functions such as EXE running etc.
 
 Credits Etc
 ===========
 
 Thanks to everyone on the psx.dev discord. If you like this software or use it please swing by and let me know.
 
 Im not a C programmer at all, so the above may seem somewhat amatuerish. Feel free to credit and rewrite or whatever. The code was jsut some simple tests I wrote to see if it would work and how it would perform.
 
  
 
