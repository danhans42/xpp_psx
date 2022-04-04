
/****************************************************
 * xpp - XPlorer Communications For RaspberryPi     *
 * by danhans42 (danhans42@gmail.com)               *
 *                                                  *
 * github.com/danhans42/xpp                         *
 *                                                  *
 * See xpp.h for wiring scheme                      *
 ****************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <unistd.h>

#include "xpp.h"

FILE *fp;
char *ptr, *buffer;
u_int32_t bin_size;
u_int32_t bin_addr;
u_int16_t bin_c16;

void DisplayHelp();
void SetupPins();
void ClearPins();
void SendByte(char);
char ReadByte();

int main (int argc, char *argv[])
{

	int32_t i;
	char resp1,resp2;
	u_int16_t  checksum = 0x00;

	printf("==================================================\n");
	printf("    xpp v0.1 - xplorer control utility\n");
//	printf("    github.com/danhans42/xpp\n");
	printf("==================================================\n\n");

	if (argc <= 1) {
		printf("Not Enough Arguments - Try /help? \n\n"); 
		exit(0);
	}

        else if (strcmp(argv[1],"/help") == 0) {
		DisplayHelp();
	}

	else if (strcmp(argv[1],"/reboot") == 0) {
		SetupPins();
		printf("Rebooting Console...\n");
		SendByte(XP_CMDPREFIX);
		SendByte(XP_REBOOTKERNEL);
		printf("Done!\n");
		}

	else if (strcmp(argv[1],"/freeze") == 0) {
		SetupPins();
		printf("Freezing Console...\n");
		SendByte(XP_CMDPREFIX);
		SendByte(XP_FREEZE);
		printf("Done!\n");
		}


	else if (strcmp(argv[1],"/unfreeze") == 0) {
		SetupPins();
		printf("Un-freezing Console...\n");
		SendByte(XP_CMDPREFIX);
		SendByte(XP_FREEZE);
		printf("Done!\n");
		}

	else if (strcmp(argv[1],"/mcstatus") == 0) {
		printf("Checking for Memory Cards...\n");
		SetupPins();
		int slot = 0;
		for (slot=0;slot<2;slot++){
			SendByte(XP_CMDPREFIX);
			SendByte(XP_MCSTATUS);
			printf("Slot %x: ",slot );
			SendByte(slot);
			resp1 = ReadByte();
			if (resp1==0x00) printf("Card Found\n");
			else if (resp1==0x02) printf("Card Not Found\n");
			else printf("Unknown Response: 0x%x\n",resp1);

		}
		printf("Done!\n");
	}


	else if (strcmp(argv[1],"/status") == 0) {
		SetupPins();
		SendByte(XP_CMDPREFIX);
		SendByte(XP_GETSTATUS);
		printf("Checking Status... ");
		resp1 = ReadByte();
		if (resp1 == 0x58) printf("in Xplorer Menu\n");
		else if (resp1 == 0x47) printf("in Game\n");
		else if (resp1 == 0x75) printf("Unirom Found in Menu\n");
		else if (resp1 == 0xF1) printf("Xflash Found\n");
		else printf("Unknown Response: 0x%x\n",resp1);
		printf("Done!\n");
	}

	else if (strcmp(argv[1],"/peek") == 0) {
		SetupPins();
		bin_addr = strtoul(argv[2],&ptr,16);
		printf("Addr: 0x%X\n",bin_addr);
		SendByte(XP_CMDPREFIX);
		SendByte(XP_PEEK);
		delay(1);
		bin_addr = strtoul(argv[2],&ptr,16);
		SendByte( (bin_addr >> 24)  & 0xFF );
		SendByte( (bin_addr >> 16)  & 0xFF );
		SendByte( (bin_addr >> 8)  & 0xFF );
		SendByte( (bin_addr)  & 0xFF );
		delay(1);
		resp1 = ReadByte();
		printf("Read: 0x%02x\n",resp1);
		printf("Done!\n");
	}


	else if (strcmp(argv[1],"/view") == 0) {
		SetupPins();
		bin_addr = strtoul(argv[2],&ptr,16);
		printf("Addr: 0x%X ",bin_addr);
		SendByte(XP_CMDPREFIX);
		SendByte(XP_GETMEM);
		delay(1);
		bin_addr = strtoul(argv[2],&ptr,16);
		buffer = (char *)malloc(0x800);
		SendByte( (bin_addr >> 24)  & 0xFF );
		SendByte( (bin_addr >> 16)  & 0xFF );
		SendByte( (bin_addr >> 8)  & 0xFF );
		SendByte( (bin_addr)  & 0xFF );
		delay(1);
		SendByte(0x00);
		SendByte(0x00);
		SendByte(0x02);
		SendByte(0x00);

		int i=0;
		for (i=0;i<0x200;i++) {
			buffer[i]=ReadByte();
			checksum = checksum+buffer[i];
		}


		SendByte( (checksum >> 8)  & 0xFF );
		resp1 = ReadByte();
		SendByte( (checksum)  & 0xFF );
		resp2 = ReadByte();
		// ^^ not really bothered about handling the returned checksum 
		resp1 = ReadByte();
		resp2 = ReadByte();

		//Display

		if ((resp1=='O') && (resp2=='K')) printf("Got Data [CS OK]\n\n");
		if ((resp1=='B') && (resp2=='G')){
			printf("Got Data [CS Failed!]\n\n");
		}

	        printf("Offset     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F    Decoded Text\n\n");

		u_int16_t loop=0, loop2=0,loop3=0;

	        for (loop2 = 0;loop2<32; loop2++){
        	        printf("%X  ",bin_addr);

                	for (loop = 0;loop<16; loop++){
                        	printf(" %02X",buffer[(loop+loop3)]);
                	}
                	printf("    ");
                	for (loop = 0;loop<16; loop++){
                	        if ( buffer[loop+loop3] > 31 && buffer[(loop+loop3)] < 127) {
                	                printf("%c",buffer[(loop+loop3)]);
                	        }
                	        else {
                	                printf(".");
                	        }
                	}
                printf("\n");
                bin_addr+=16;
                loop3+=16;
       		}
		printf("\n");
		printf("Done!\n");

	}

	else if (strcmp(argv[1],"/exec") == 0) {
		bin_c16 = 0;

		if (argc <= 3) {
			printf("Not Enough Arguments\n\n");
			exit(0);
		}

		fp = fopen(argv[2], "rb");

		if (fp==NULL){
			printf("File Not found\n\n");
			exit(0);
		}

		SetupPins();
		fseek(fp, 0, SEEK_END);
		bin_size = ftell(fp);
		bin_addr = strtoul(argv[3],&ptr,16);
		printf("    File     : %s (%i bytes)\n",argv[2],bin_size );
		printf("    Address  : 0x%X\n",bin_addr);
		fseek(fp, 0, SEEK_SET);
		buffer = (char *)malloc(bin_size * sizeof(char));
		fread(buffer, bin_size, 1, fp);
		fclose(fp);
		for (i=0;i<bin_size;i++) {
			bin_c16 = bin_c16 + buffer [i];
		}
		printf("    Checksum : 0x%X\n\n",bin_c16);

		printf("==================================================\n\n");

		SendByte(XP_CMDPREFIX);
		SendByte(XP_SENDMEMANDEXECUTE);
		delay(1);
		//Send the address we are uploading to,
		printf("TX: Load Address\n");
		SendByte( (bin_addr >> 24)  & 0xFF );
		SendByte( (bin_addr >> 16)  & 0xFF );
		SendByte( (bin_addr >> 8)  & 0xFF );
		SendByte( (bin_addr)  & 0xFF );
		delay(1);
		//Send the length of the data
		printf("TX: Data Length\n");

		SendByte( (bin_size >> 24)  & 0xFF );
		SendByte( (bin_size >> 16)  & 0xFF );
		SendByte( (bin_size >> 8)  & 0xFF );
		SendByte( (bin_size)  & 0xFF );
		//Send the data
		delay(1);
		printf("TX: Data... ");

		for (i=0;i < bin_size ;i++) {
//			printf("%x\n",i);
			SendByte(buffer[i]);
			checksum = checksum+buffer[i];
		}
		printf("Sent\n");
		delay(10);
		printf("RX: Read Checksum [0x");
		//send checksum msb
		SendByte( (bin_c16 >> 8)  & 0xFF );
		//read checkscum msb
		//resp1 = ReadByte();
		printf("%X",ReadByte());
		//send checksum lsb
		SendByte( (bin_c16)  & 0xFF );
       		//read checksum lsb
		//resp1 = ReadByte();
		printf("%X",ReadByte());
		// read the response
		resp1 = ReadByte();
		resp2 = ReadByte();
		printf("]\n");
		if ((resp1=='O') && (resp2=='K')) printf("Checksum OK - Execute!\n");
		if ((resp1=='C') && (resp2=='F')) printf("Checksum Fail\n");
		//ClearPins();
		printf("Done!\n");
	}


	else if (strcmp(argv[1],"/exe") == 0) {
		bin_c16 = 0;
		if (argc <= 2) {
			printf("Not Enough Arguments\n\n");
			exit(0);
		}

		fp = fopen(argv[2], "rb");

		if (fp==NULL){
			printf("File Not found\n\n");
			exit(0);
		}

		SetupPins();
		fseek(fp, 0, SEEK_END);
		bin_size = ftell(fp);
		printf("    File     : %s (%i bytes)\n",argv[2],bin_size );
		fseek(fp, 0, SEEK_SET);
		buffer = (char *)malloc(bin_size * sizeof(char));
		fread(buffer, bin_size, 1, fp);
		fclose(fp);

		//data read into buffer - lets get the bits we need

		printf("    LoadAddr : %02x%02x%02x%02x\n",buffer[27],buffer[26],buffer[25],buffer[24]);
		printf("    CallAddr : %02x%02x%02x%02x\n",buffer[19],buffer[18],buffer[17],buffer[16]);
		printf("    DataSize : %02x%02x%02x%02x\n",buffer[31],buffer[30],buffer[29],buffer[28]);


		for (i=0;i<bin_size-0x800;i++) {
			bin_c16 = bin_c16 + buffer [i+0x800];
		}
		printf("    Checksum : 0x%X\n\n",bin_c16);

		printf("==================================================\n\n");

//		while(1);

		SendByte(XP_CMDPREFIX);
		SendByte(XP_SENDMEM);
		delay(1);
		//Send the address we are uploading to,
		printf("TX: Load Address\n");
		SendByte(buffer[27]);
		SendByte(buffer[26]);
		SendByte(buffer[25]);
		SendByte(buffer[24]);
		delay(1);
		//Send the length of the data
		printf("TX: Data Length\n");

		SendByte(buffer[31]);
		SendByte(buffer[30]);
		SendByte(buffer[29]);
		SendByte(buffer[28]);
		//Send the data
		delay(1);
		printf("TX: Data... ");

		for (i=0;i<bin_size-0x800;i++) {
			SendByte(buffer[i+0x800]);
			checksum = checksum+buffer[i];
		}
		printf("Sent\n");
		//while(1);
		delay(10);
		printf("RX: Read Checksum [0x");
		//send checksum msb
		SendByte( (bin_c16 >> 8)  & 0xFF );
		//read checkscum msb
		//resp1 = ReadByte();
		printf("%X",ReadByte());
		//send checksum lsb
		SendByte( (bin_c16)  & 0xFF );
       		//read checksum lsb
		//resp1 = ReadByte();
		printf("%X",ReadByte());
		// read the response
		resp1 = ReadByte();
		resp2 = ReadByte();
		printf("]\n");
		if ((resp1=='O') && (resp2=='K')) printf("Checksum OK - Execute!\n");
		if ((resp1=='B') && (resp2=='G')) printf("Checksum Fail\n");
		//ClearPins();
		SendByte(XP_CMDPREFIX);
		SendByte(XP_RAWEXEC);
		SendByte(buffer[19]);
		SendByte(buffer[18]);
		SendByte(buffer[17]);
		SendByte(buffer[16]);
		printf("Done!\n");
	}
	else if (strcmp(argv[1],"/flash") == 0) {
		bin_c16 = 0;

		if (argc <= 2) {
			printf("Not Enough Arguments\n\n");
			exit(0);
		}

		fp = fopen(argv[2], "rb");

		if (fp==NULL){
			printf("File Not found\n\n");
			exit(0);
		}

		SetupPins();
		fseek(fp, 0, SEEK_END);
		bin_size = ftell(fp);
		bin_addr = 0x80180000;
		printf("    ROM      : %s (%i bytes)\n",argv[2],bin_size );
		//printf("    Address  : 0x%X\n",bin_addr);
		fseek(fp, 0, SEEK_SET);
		buffer = (char *)malloc(bin_size * sizeof(char));
		fread(buffer, bin_size, 1, fp);
		fclose(fp);
		for (i=0;i<bin_size;i++) {
			bin_c16 = bin_c16 + buffer [i];
		}
		printf("    Checksum : 0x%X\n\n",bin_c16);

		printf("==================================================\n\n");

		SendByte(XP_CMDPREFIX);
		SendByte(XP_SENDMEMANDBURNFW);
		delay(1);
		//Send the address we are uploading to,
		//printf("TX: Load Address\n");
		SendByte( (bin_addr >> 24)  & 0xFF );
		SendByte( (bin_addr >> 16)  & 0xFF );
		SendByte( (bin_addr >> 8)  & 0xFF );
		SendByte( (bin_addr)  & 0xFF );
		delay(1);
		//Send the length of the data
		printf("TX: Data Length\n");

		SendByte( (bin_size >> 24)  & 0xFF );
		SendByte( (bin_size >> 16)  & 0xFF );
		SendByte( (bin_size >> 8)  & 0xFF );
		SendByte( (bin_size)  & 0xFF );
		//Send the data
		delay(1);
		printf("TX: Data... ");

		for (i=0;i < bin_size ;i++) {
			SendByte(buffer[i]);
			checksum = checksum+buffer[i];
		}
		printf("Sent\n");
		delay(10);
		printf("RX: Read Checksum [0x");

		SendByte( (bin_c16 >> 8)  & 0xFF );
		printf("%X",ReadByte());
		SendByte( (bin_c16)  & 0xFF );
		printf("%X",ReadByte());
		resp1 = ReadByte();
		resp2 = ReadByte();
		printf("]\n");
		if ((resp1=='O') && (resp2=='K')) printf("Checksum OK - ROM Image will now be flashed\n");
		if ((resp1=='C') && (resp2=='F')) printf("Checksum Fail\n");
		printf("Done!\n");
	}


	else if (strcmp(argv[1],"/download") == 0) {
		u_int32_t length=0;
		SetupPins();
		printf("File: %s\n",argv[2]);
		bin_addr = strtoul(argv[3],&ptr,16);
		length = strtoul(argv[4],&ptr,16);
		printf("Addr: 0x%X\n",bin_addr);
		printf("Size: 0x%X\n",length);
		SendByte(XP_CMDPREFIX);
		SendByte(XP_GETMEM);
		delay(1);
		bin_addr = strtoul(argv[2],&ptr,16);
		buffer = (char *)malloc(length);
		SendByte( (bin_addr >> 24)  & 0xFF );
		SendByte( (bin_addr >> 16)  & 0xFF );
		SendByte( (bin_addr >> 8)  & 0xFF );
		SendByte( (bin_addr)  & 0xFF );
		delay(1);
		SendByte( (length >> 24)  & 0xFF );
		SendByte( (length >> 16)  & 0xFF );
		SendByte( (length >> 8)  & 0xFF );
		SendByte( (length)  & 0xFF );
		int i=0;
		for (i=0;i<length;i++) {
			buffer[i]=ReadByte();
			checksum = checksum+buffer[i];
		}


		SendByte( (checksum >> 8)  & 0xFF );
		resp1 = ReadByte();
		SendByte( (checksum)  & 0xFF );
		resp2 = ReadByte();
		// ^^ not really bothered about handling the returned checksum 
		resp1 = ReadByte();
		resp2 = ReadByte();

		//Display

		if ((resp1=='O') && (resp2=='K')) printf("Got Data [CS OK]\n\n");
		if ((resp1=='B') && (resp2=='G')){
			printf("Got Data [CS Failed!]\n\n");
		}
	}

        else {
		printf("Unknown Command\n\n");
	}



}





void SendByte (char byte)
{
	digitalWriteByte(byte);
	digitalWrite (SEL, HIGH) ;

	do {
    		digitalWrite (SEL, HIGH) ;
	} while (digitalRead(ACK) == 0 );

   	digitalWrite (SEL, LOW);

	while (digitalRead(ACK) == 1 );
	digitalWrite (SEL, LOW);
	digitalWriteByte(0);
}

char ReadByte() {

        char byte;
        byte = 0x00;
        while (digitalRead(ACK) == 0); // wait until ack is high

        byte |= (digitalRead(PE) << 7  );
        byte |= (digitalRead(SLCT) << 6);

        digitalWrite(SEL, HIGH);
        while (digitalRead(ACK) == 1); // wait until ack is low

        byte |= (digitalRead(BUSY) << 5);
        byte |= (digitalRead(PE) << 4  );
        byte |= (digitalRead(SLCT) << 3);
        digitalWrite(SEL, LOW);
        while (digitalRead(ACK) == 0); // wait until ack is high
        byte |= (digitalRead(BUSY) << 2);
        byte |= (digitalRead(PE) << 1  );
        byte |= (digitalRead(SLCT) );
        digitalWrite(SEL, HIGH);
        while (digitalRead(ACK) == 1); // wait until ack is low
        digitalWrite(SEL, LOW);

        	
	return byte;
}

void SetupPins() {
	wiringPiSetupGpio () ;  //USE BCM GPIO Numbering 

	pinMode (D0,   OUTPUT);
	pinMode (D1,   OUTPUT);
	pinMode (D2,   OUTPUT);
	pinMode (D3,   OUTPUT);
	pinMode (D4,   OUTPUT);
	pinMode (D5,   OUTPUT);
	pinMode (D6,   OUTPUT);
	pinMode (D7,   OUTPUT);
	pinMode (SEL,  OUTPUT);

	pinMode (SLCT, INPUT);
	pinMode (PE,   INPUT);
	pinMode (BUSY, INPUT);
	pinMode (ACK,  INPUT);

	digitalWrite (SEL, LOW);
	delay(200);
}

void ClearPins() {
	// Basically set all pins as inputs - prevents unwanted behaviour

	wiringPiSetupGpio () ;  //USE BCM GPIO Numbering 

	pinMode (D0, INPUT);
	pinMode (D1, INPUT);
	pinMode (D2, INPUT);
	pinMode (D3, INPUT);
	pinMode (D4, INPUT);
	pinMode (D5, INPUT);
	pinMode (D6, INPUT);
	pinMode (D7, INPUT);
	pinMode (SEL,  INPUT);

	pinMode (SLCT, INPUT);
	pinMode (PE,   INPUT);
	pinMode (BUSY, INPUT);
	pinMode (ACK,  INPUT);

	digitalWrite (SEL, LOW);
	delay(100);
}

void DisplayHelp() {

	printf(" /help                     		this screen\n");
	printf(" /status                   	        check console status\n");
	printf(" /mcstatus*                 	        check memory card status\n");
	printf(" /reboot                   	        reboot console (jump to 0xbfc00000)\n");
	printf(" /freeze*                   	        freeze console\n");
	printf(" /unfreeze*                      	unfreeze console\n");
	printf(" /peek <address>			return byte at <address>\n");
	printf(" /view <address>			view 512bytes of memory beginning at <address> \n");
	printf(" /upload <binfile>			upload <binfile> to <address>\n");
	printf(" /download <outfile> <address> <length>	download <length> from <address> to <outfile>\n");
	printf(" /execbin <binfile> <address>*          upload <binfile> to <address> & execute\n");
	printf(" /exe <psxexe> 			        upload <psxexe> & execute\n");
	printf(" /flash <romfile>			upload <romfile> and flash to eeprom\n\n");
	printf(" *Requires an Xplorer FX v4.00+ ROM\n");
}

