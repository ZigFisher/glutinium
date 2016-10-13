/*
	Created: Jun 2012
	by ihsan Kehribar <ihsan@kehribar.me>
	
	Pic24f series programming with limited device support.
	Overall program memory reading and verifying doesn't work.
	Tested with bus pirate firmware and worked well. 
	
	<ignore>
		// YOU HAVE TO PUT SOME KIND OF LEVEL CONVERSION CIRCUIT BETWEEN
		// LITTLE WIRE AND THE TARGET PIC!
	</ignore>
	
	=> Pic24f series are apperently 5V tolerant in digital pins. You 
	don't have to put voltage level conversion but it is advised to 
	do so. 
	
	pin2 -> MCLR
	pin0 -> PGC
	pin1 -> PGD
	
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "littleWire.h"
#include "littleWire_util.h"
#include <time.h>

char lineBuff[64];
unsigned char rx[4];
unsigned char version;
int myCount=0;
FILE *fp;	
FILE *inputFile;
char *name = "./log.txt";
int myIndex=0;
unsigned long instructions[64];
unsigned long bigMemory[0x0157FF];
int deviceIndex;
unsigned long config1;
unsigned long config2;
int configOverride;

// http://ww1.microchip.com/downloads/en/DeviceDoc/39768d.pdf

char* deviceList[] = 
{
	"PIC24FJ16GA002",	"PIC24FJ16GA004",	"PIC24FJ32GA002", 	"PIC24FJ32GA004",	"PIC24FJ48GA002", 
	"PIC24FJ48GA004",	"PIC24FJ64GA002",	"PIC24FJ64GA004", 	"PIC24FJ64GA006",	"PIC24FJ64GA008",
	"PIC24FJ64GA010",	"PIC24FJ96GA006",	"PIC24FJ96GA008",	"PIC24FJ96GA010",	"PIC24FJ128GA006",
	"PIC24FJ128GA008",	"PIC24FJ128GA010"
};

unsigned long deviceID[] = 
{
	0x0444,		0x044C,		0x0445,		0x044D,		0x0446,
	0x044E,		0x0447,		0x044F,		0x0405,		0x0408,
	0x040B,		0x0406,		0x0409,		0x040C,		0x0407,
	0x040A,		0x040D
};

unsigned long config1Location[] =
{
	0x002BFE,	0x002BFE,	0x0057FE,	0x0057FE,	0x0083FE,
	0x0083FE,	0x00ABFE,	0x00ABFE,	0x00ABFE,	0x00ABFE,
	0x00ABFE,	0x00FFFE,	0x00FFFE,	0x00FFFE,	0x0157FE,
	0x0157FE,	0x0157FE
};

unsigned long config2Location[] =
{
	0x002BFC,	0x002BFC,	0x0057FC,	0x0057FC,	0x0083FC,
	0x0083FC,	0x00ABFC,	0x00ABFC,	0x00ABFC,	0x00ABFC,
	0x00ABFC,	0x00FFFC,	0x00FFFC,	0x00FFFC,	0x0157FC,
	0x0157FC,	0x0157FC
};


littleWire *lw = NULL;
int i,k,t,q;
struct timeval start, end;

#define sendCommand(cmd) customMessage(lw,rx,52,12,cmd,cmd,0);

void setMCLR() 
{
	pinMode(lw,PIN2,OUTPUT);
	digitalWrite(lw,PIN2,HIGH);
}
	
void clearMCLR() 
{
	pinMode(lw,PIN2,OUTPUT);
	digitalWrite(lw,PIN2,LOW);
}

inline void sendSix(unsigned long code,unsigned char nopCount)
{
	unsigned char tx[4];
	
	tx[0] = (code & 0x000000FF)>>0; 
 	tx[1] = (code & 0x0000FF00)>>8; 
	tx[2] = (code & 0x00FF0000)>>16;
 	
	customMessage(lw,rx,53,nopCount,tx[0],tx[1],tx[2]);
}

unsigned long sendRegout() 
{
	unsigned long value;
	
	// REGOUT
	sendCommand(0x01); 
	
	// waste 8 bits
	customMessage(lw,rx,52,13,8,8,0);
	
	// read the 2 byte value
	customMessage(lw,rx,52,15,2,2,0);
	customMessage(lw,rx,40,0,0,0,0);
	
	value = rx[0] | (rx[1]<<8) ;
	return value;
}

void enterICSP() 
{
	unsigned char icsp_code[4]; // 0x8A12C2B2;
	
	icsp_code[3] = 0x8A;
	icsp_code[2] = 0x12;
	icsp_code[1] = 0xC2;
	icsp_code[0] = 0xB2;
	
	pinMode(lw,PIN1,OUTPUT);
	pinMode(lw,PIN4,OUTPUT);	
	
	setMCLR();
	delay(5);
	clearMCLR();
	
	// send the special command
	customMessage(lw,rx,0xD4,icsp_code[0],icsp_code[1],icsp_code[2],icsp_code[3]);
	
	setMCLR();
	
	// waste 5 bits
	customMessage(lw,rx,52,13,5,5,0);
	
	// send NOP command
	sendSix(0x000000,0);
	sendSix(0x040200,1);
}

void exitICSP() 
{
	clearMCLR();
		delay(10);
	setMCLR();
}

unsigned long readAddress(unsigned long address24bit)
{
	unsigned long value1=0x00;
	unsigned long value2=0x00;
	unsigned long value3=0x00;
	
	unsigned long temp;
	
	temp = address24bit;

	sendSix(0x200000|((temp & 0x00FF0000)>>12),0);
	sendSix(0x880190,0);
	sendSix(0x200006|((temp & 0x0000FFFF)<<4),0);
	sendSix(0x207847,1);
	
	sendSix(0xBA0B96,2);
	value1 = sendRegout();
	
	// sendSix(0xBADBB6,2);
	// sendSix(0xBAD3D6,2);
	// value2 = sendRegout();
	
	// sendSix(0xBA0BB6,2);
	// value3 = sendRegout();
	
	sendSix(0x040200,1);
	
	return value1 | ((value2&0xFF)<<16);
}

void eraseFlash() 
{
	sendSix(0x2404F0,0);
	sendSix(0x883B00,0);

	sendSix(0x200000,0);
	sendSix(0x200001,0);

	sendSix(0x880191,0);
	sendSix(0xBB0800,2);
	sendSix(0xA8E761,2);

	delay(1000);

	sendSix(0x000000,0);
	sendSix(0x040200,1);
}

void writeInstruction(unsigned long address,unsigned long* buf)
{
#if 1
	int index;
	int latch;
	
	// Step 2: Set the NVMCON to program 64 instr. words
	sendSix(0x24001A,0);
	sendSix(0x883B0A,0);

	// Step 3:
	unsigned long temp ;
	
	temp = 0x200000 | (address >> 12 & 0xFF0);
	sendSix(temp,0);
	
	sendSix(0x880190,0);
	
	temp = 0x200007 | (address << 4 & 0xFFFF0);
	sendSix(temp,0);

   // Step 4-6: Write 64 (16*4 packed) instructions
   for (index = 0; index < 64; index += 4)
   {
       sendSix(0x200000 | (buf[index+0]&0x00FFFF)<<4,0);
       sendSix(0x200001 | (((buf[index+1]&0xFF0000)>>4) | ((buf[index+0]&0xFF0000)>>12)),0);
       sendSix(0x200002 | (buf[index+1]&0x00FFFF)<<4,0);
       sendSix(0x200003 | (buf[index+2]&0x00FFFF)<<4,0);
       sendSix(0x200004 | (((buf[index+3]&0xFF0000)>>4) | ((buf[index+2]&0xFF0000)>>12)),0);
       sendSix(0x200005 | (buf[index+3]&0x00FFFF)<<4,0);
	   
       sendSix(0xEB0300,1);
       for (latch = 0; latch < 2; latch++)
       {
           sendSix(0xBB0BB6,2);
           sendSix(0xBBDBB6,2);
           sendSix(0xBBEBB6,2);
           sendSix(0xBB1BB6,2);
       }
   }

   // Step 7: Initiate write cycle
   sendSix(0xA8E761,2);

   // Step 8: Poll WR bit   
   unsigned long nvmcon;
   do
   {
       sendSix(0x040200,1);
       sendSix(0x803B02,0);
       sendSix(0x883C22,1);
       nvmcon = sendRegout();
       sendSix(0x000000,0);	   
   } while ((nvmcon & 0x8000));
   
   // Reset device
   sendSix(0x040200,1);
#endif
}

void emptyBuff()
{
	int q;
	for(q=0;q<64;q++) lineBuff[q]=0;
}

int main (int argc, char *argv[])
{
	unsigned int value;
	lw = littleWire_connect();
	
	if(lw == NULL){
		printf("Little Wire could not be found!\n");
		exit(EXIT_FAILURE);
	}

	version = readFirmwareVersion(lw);
	printf("-----------------------------------------\n");
	printf("> Little Wire firmware version: %d.%d <\n",((version & 0xF0)>>4),(version&0x0F));
	printf("-----------------------------------------\n\n");
	
	if(version==0x00)
		printf("> This program requires the latest firmware. PLease update soon.");
	
	if(argc<2)
	{
		printf("\n\tWhere is the hex file name?\n");
		printf("\n\nExample: 'pic24f main.hex'\n");
		return 0;
	}
	else if(argc==2)
	{
		printf("-----------------------------------------\n");
		printf("- Programming parameters \n");
		printf("-----------------------------------------\n");
		printf("> File name: %s\n",argv[1]);
		printf("> Config registers will be read from the file\n");
		printf("-----------------------------------------\n");
		configOverride=0;
	}
	else if(argc==4)
	{
		printf("-----------------------------------------\n");
		printf("- Programming parameters \n");
		printf("-----------------------------------------\n");
		printf("> File name: %s\n",argv[1]);
		sscanf(argv[2],"%lX",&config1);
		sscanf(argv[3],"%lX",&config2);
		printf("> Config1: %lX\n",config1);
		printf("> Config2: %lX\n",config2);
		printf("-----------------------------------------\n");
		configOverride=1;
	}
	else
	{
		printf("\n\tWrong parameters!");
		printf("\n\nExample: 'pic24f main.hex 0x3F7F 0xF9DF'\n");
		printf("Example: 'pic24f main.hex'\n");
		return 0;
	}
	
	inputFile=fopen(argv[1],"r");
	fp = fopen(name, "w");
	
	
	unsigned int byteCount;
	unsigned int address;
	unsigned int cheksum;
	unsigned int type;
	unsigned char endFile = 0;
	unsigned long addressBuff[32];
	unsigned char dataBuff[32];
	unsigned int seg = 0;
	unsigned int hadr = 0;

	i=0;	
	
	spi_updateDelay(lw,0);
	
	enterICSP();
	
		printf("\n");
		printf("-----------------------------------------\n");
		printf("- Read device information\n");
		printf("-----------------------------------------\n");
		value = readAddress(0xFF0000);
		printf("> DEVID:\t\t0x%4X\n",value);
		
		deviceIndex=0xDEAD;
		
		for(i=0;i<sizeof(deviceID);i++)
		{
			if(deviceID[i]==value)
				deviceIndex=i;
		}
	
		if(deviceIndex==0xDEAD)
		{
			printf("> Target Device is not compatible with this program!\n");
			exitICSP();
			return 0;
		}
		else
			printf("> Device name:\t\t%s\n",deviceList[deviceIndex]);
		
		value = readAddress(0xFF0002);
		printf("> REVID:\t\t0x%4X\n",value);				
		value = readAddress(config1Location[deviceIndex]);		
		printf("> Config #1:\t\t0x%4X\n",value);	
		value = readAddress(config2Location[deviceIndex]);
		printf("> Config #2:\t\t0x%4X\n",value);
		
		printf("-----------------------------------------\n");
	
	// exitICSP();
	
#if 1
	// enterICSP();
		printf("\n");
		printf("-----------------------------------------\n");
		printf("> Erasing the flash ... \n");
		printf("-----------------------------------------\n");
		eraseFlash();
	// exitICSP();
	
	/* prefill the memory map */
	for(q=0;q<config1Location[deviceIndex]+1;q+=2)
		bigMemory[q]=0x00FFFF;
	
	printf("\n");
	printf("-----------------------------------------\n");
	printf("> Begin reading the input file ... \n");
	printf("-----------------------------------------\n");
	
	/* hex file encoding */
	while(endFile==0)
	{			
		do{			
			emptyBuff();
			if(fread(lineBuff,1,1,inputFile)==0)
				endFile = 1;
		} while(lineBuff[0]!=':' && endFile==0);			
		
		if(endFile==0)
		{	
			emptyBuff();
			fread(lineBuff,1,2,inputFile); // byteCount
			sscanf(lineBuff,"%X",&byteCount);

			emptyBuff();
			fread(lineBuff,1,4,inputFile); // address
			sscanf(lineBuff,"%X",&address);

			emptyBuff();
			fread(lineBuff,1,2,inputFile); // type
			sscanf(lineBuff,"%X",&type);								
			
			switch(type)
			{			
				case 0: // Data byte
				{
					address += (seg << 4) + (hadr << 16);					
					
					for(i=0;i<byteCount;i++)
					{
						addressBuff[i]=(address++)>>1;								
						emptyBuff();
						fread(lineBuff,1,2,inputFile); // dataByte
						sscanf(lineBuff,"%X",dataBuff+i);										
					}						

					if(byteCount==16) // 4 x 32bit -> 4 instruction
					{		
						bigMemory[addressBuff[0]] = dataBuff[0] | (dataBuff[1]<<8) | (dataBuff[2]<<16) | (dataBuff[3]<<24) ;											
						bigMemory[addressBuff[4]] = dataBuff[4] | (dataBuff[5]<<8) | (dataBuff[6]<<16) | (dataBuff[7]<<24) ;						
						bigMemory[addressBuff[8]] = dataBuff[8] | (dataBuff[9]<<8) | (dataBuff[10]<<16) | (dataBuff[11]<<24) ;												
						bigMemory[addressBuff[12]] = dataBuff[12] | (dataBuff[13]<<8) | (dataBuff[14]<<16) | (dataBuff[15]<<24) ;									
					}
					else if(byteCount==8) // 2 x 32bit -> 2 instruction
					{
						bigMemory[addressBuff[0]] = dataBuff[0] | (dataBuff[1]<<8) | (dataBuff[2]<<16) | (dataBuff[3]<<24) ;											
						bigMemory[addressBuff[4]] = dataBuff[4] | (dataBuff[5]<<8) | (dataBuff[6]<<16) | (dataBuff[7]<<24) ;												
					}
					else if(byteCount==4) // 1 x 32bit -> 1 instruction
					{
						bigMemory[addressBuff[0]] = dataBuff[0] | (dataBuff[1]<<8) | (dataBuff[2]<<16) | (dataBuff[3]<<24) ;
					}

					break;
				}
				case 2:
				{	
					emptyBuff();
					fread(lineBuff,1,4,inputFile); // dataByte
					sscanf(lineBuff,"%X",&seg);						
					break;
				}
				case 4:
				{		
					emptyBuff();
					fread(lineBuff,1,4,inputFile); // dataByte
					sscanf(lineBuff,"%X",&hadr);						
					break;
				}
				case 1:
				{
					printf("\n");
					printf("-----------------------------------------\n");
					printf("> Size: %f kBytes\n",(float)(addressBuff[0]/1024.0));
					printf("-----------------------------------------\n");
					break;
				}
			}					
		}			
	}	

	for(i = 0; i< config1Location[deviceIndex]+1; i=i+16)
		fprintf(fp,"%6X : --> :\t%6lX\t%6lX\t%6lX\t%6lX\t%6lX\t%6lX\t%6lX\t%6lX\n",i,bigMemory[i],bigMemory[i+2],bigMemory[i+4],bigMemory[i+6],bigMemory[i+8],bigMemory[i+10],bigMemory[i+12],bigMemory[i+14]);
	
	if(configOverride)
	{
		bigMemory[config1Location[deviceIndex]]=0xFF0000|config1;
		bigMemory[config2Location[deviceIndex]]=0xFF0000|config2;
	}
	
	// enterICSP();
	gettimeofday(&start, NULL);
	/* setup memory packets and send the 'valuable' ones */
	printf("\n");
	printf("-----------------------------------------\n");
	printf("- Start sending packets\n");
	printf("-----------------------------------------\n");	
	for(i=0;i<config1Location[deviceIndex]+1;i=i+128)
	{
		int isNull = 1;		
		
		for(q=0;q<64;q++)
		{
			instructions[q] = bigMemory[i+(q*2)];		
		}
		
		for(q=0;q<64;q++)
		{
			if((instructions[q] != 0xFFFFFF))
				isNull = 0;
		}		
		
		if(!isNull)
		{			
			isNull = 1;	
			
			for(q=0;q<64;q++)
			{
				if((instructions[q] != 0x00FFFF))
				{
					isNull = 0;
				}	
			}
		}
		
		if(!isNull) /* Does it worth it ? */
		{			
			printf("> Current address:\t0x%4X\n",i);
			writeInstruction(i,instructions);
		}
	}
	gettimeofday(&end, NULL);		  
	
	printf("-----------------------------------------\n");
	printf("> Finished writing!\n");
	printf("> Elapsed time: %f seconds\n",((end.tv_sec + (end.tv_usec/1000000.0))-(start.tv_sec + (start.tv_usec/1000000.0))));
	printf("-----------------------------------------\n");
	
	// exitICSP();	
	
#endif

	// enterICSP();
		printf("\n");
		printf("-----------------------------------------\n");
		printf("- Read config registers again\n");
		printf("-----------------------------------------\n");
		value = readAddress(config1Location[deviceIndex]);		
		printf("> Config #1:\t\t0x%4X\n",value);	
		value = readAddress(config2Location[deviceIndex]);
		printf("> Config #2:\t\t0x%4X\n",value);
		printf("-----------------------------------------\n");
	exitICSP();

	printf("\n");
	printf("-----------------------------------------\n");
	printf("> Little Wire done. Thanks. \n");
	printf("-----------------------------------------\n");
	
	return 0;
}
