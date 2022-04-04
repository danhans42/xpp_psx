/****************************************************
 * xpp - XPlorer Communications For RaspberryPi     *
 * by danhans42 (danhans42@gmail.com)               *
 * Version 0.1 CODENAME: DOESFUCKALL                *
 * github.com/danhans42/xpp                         *
 *                                                  *
 * To see the Raspberry Pi GPIO pinout              *
 * run  $gpio readall                               *
 *                                                  *
 ****************************************************/

/* D0-D7 are chosen so the WriteByte function can be used from WiringPi to easily write all 8 at once
   The other pins dont really matter so much, so you can probably swap them around.

   Alternatively, you could use something like an SPI/I2C IO Expander to do this and save on RPi IO pins */

// Outputs	Name	BCM IO	Xplorer DB25 PIN

   #define	D0     	17 	//DB25 2
   #define 	D1     	18 	//DB25 3
   #define 	D2     	27 	//DB25 4
   #define 	D3     	22 	//DB25 5
   #define 	D4     	23 	//DB25 6
   #define 	D5     	24 	//DB25 7
   #define 	D6     	25 	//DB25 8
   #define 	D7     	4  	//DB25 9
   #define 	SEL    	12 	//DB25 17

// Inputs	Name	BCM IO	Xplorer DB25 PIN

   #define 	SLCT   	16 	//DB25 13
   #define 	PE     	20 	//DB25 12
   #define 	BUSY   	21 	//DB25 11
   #define 	ACK    	5  	//DB25 10

/* Xplorer Commands

  Taken from the psx-spex docsfrom noca$h - no link to Martins document directly as its down.
  However, here is a link to the conversion of his information : https://psx-spx.consoledev.net/

  The commands are described as they are for the psx. Not sure what we can and can not
  get working on different xplorer64 firmware versions. Probably will stick some
  sort of matrix here with the compatibility as I check each version I can obtain.

  XP_CMDPREFIX is always sent prior to a command, followed by the command and any parameters.

  Xplorer 64 Compatibilty (Tested on Version 1.00/1.067E)
  =======================================================
  (n/a)      MenuSetMemAndBurnFirm Tx(5755h,Addr32,Len32,Data[Len]), TxRxChksum ;burnFlash
  (/status)  GetStateGameOrMenu    Tx(5757h), Rx(47h=Game, or 58h=Menu)
  (/execbin) SetMemAndExecute      Tx(5758h,Addr32,Len32,Data[Len]), TxRxChksum ;call Addr

  PSX Xplorer/Pro/FX

  Turbo Commands are currently not supported (TurboGetMem)
*/


   #define  	XP_CMDPREFIX		 0x57 // Command Prefix

   #define  	XP_PEEK		 	 0x02 // Call Addr
   #define  	XP_MCSTATUS	 	 0x0B // Gem Memory Card Slots Status
   #define  	XP_RAWEXEC	 	 0x0F // Call Addr
   #define  	XP_REBOOTKERNEL 	 0x42 // Jump to 0xbfc00000 (Reboot)
   #define  	XP_GETMEM	 	 0x47 // Get Memory
   #define  	XP_FREEZE	 	 0x4C // Freeze Console
   #define  	XP_UNFREEZE	 	 0x52 // Un-freeze Console
   #define  	XP_SENDMEM	 	 0x53 // upload binary data to address
   #define  	XP_SENDMEMANDBURNFW	 0x55 // MenuSetMemAndBurnFirm (be careful!)
   #define  	XP_GETSTATUS		 0x57 // Returns 0x36 when in menu
   #define  	XP_SENDMEMANDEXECUTE 	 0x58 // upload binary data to address and call



