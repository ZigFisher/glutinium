/* 
 * upschk 
 * sivann _AT_ softlab.ece.ntua.gr 2004
 * ver 1.3 
 *
 */

//#define PORT  "/dev/ups"   /* Set to _your_ port */
#define PORT  "/dev/ttyUSB0"   /* port B (solaris 7, ultra 10)*/

#define UPSRUNTIME 200 //seconds to run without power before shutdown
#define DOWNSCRIPT "/etc/upschk-down.sh" //script to run for shutdown
#define UPSCRIPT "/etc/upschk-up.sh" //script to run when power is restored

#define LOGSCRIPT 1 //log shutdown script errors to syslog
//if LOGSCRIPT is defined,use logger; if logger isn't available
//you must define LOGSCRIPT 0
#define LOGGER "/usr/bin/logger" 

#define LOGDATAINTERVAL 600 //log normal voltages every LOGDATAINTERVAL seconds

/*
 * Works with Advance-Intelligent UPSes which support the command set below, 
 * such as X power Micronet, Megatec, Falcon SG, et.c
 * Tested only with the X Power Micronet
 *
 * Windows softwares which support the same protocol: Commander Pro, WinPower
 *
 * UPS Protocol Summary:
 *
 * Rs232 : 2400N81
 * ITEM	COMMAND   DESCRIPTION              	
 * 1a  	  D       Status Inquiry           *disable         	
 * 1  	  Q1      Status Inquiry                    	
 * 2  	  T       10 Seconds Test                   	
 * 3  	  TL      Test until Battery Low            	
 * 4  	  T<n>    Test for Specified Time Period    	
 * 5  	  Q       Turn On/Off beep                  	
 * 6  	  S<n>    Shut Down Command                 	
 * 7  	S<n>R<m>  Shut Down and Restore Command     	
 * 8  	  C       Cancel Shut Down Command          	
 * 9  	  CT      Cancel Test Command               	
 * 10	   I	  UPS Information Command	
 * 11	   F	  UPS Rating Information	
 *
 * DSR is connected to DTR inside the UPS
 *
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termio.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <syslog.h>
#include <fcntl.h>
//#include <asm/termios.h>


//globals
static struct termios pots;
int  fd=0;
char buf[256];
char BUF[256];
char cmd[64];
char str[1024];
const int tiocm_dtr = TIOCM_DTR;
const int tiocm_rts = TIOCM_RTS;
int verbose=0,daemonmode=0;
int shutdown_started=0;
int restore_started=0;

time_t PowerFailStart=0,PowerFailDuration=0;
time_t LastDataT=0,LastValidDataT=0,nowT=0,LastLoggedStatusT=0;
time_t MaxPowerFailDuration=UPSRUNTIME; //seconds without power until shutdown


//UPS returned Info
float IPVolts,IPFaultVolts,OPVolts,IPFreq,BatVolts,Temp;
int OPCurr;
unsigned char PowerFail,BatLow,BypassActive,UPSFailed,UPSisSTBY,TestActive;
unsigned char ShutdownActive,BeeperOn;

void printdata(void);
int makedecisions(void);

void msg(char * str,int priority);

void msg(char * str,int priority)
{
  if (verbose)
    fprintf(stderr,"MSG:%s\n",str);

  syslog (priority, "%s",str);
}

//controlling terminal doesn't wait for return
void no_wait ()
{
    int fd;
    struct termio test;

    fd = open (ttyname (0), O_WRONLY);
    ioctl (fd, TCGETA, &test);

    test.c_cc[VMIN] = 1;
    test.c_lflag = ISIG;

    ioctl (fd, TCSETA, &test);
    close (fd);
}


/* atexit handler */
void atexit_hdl()
{
    if (fd) tcsetattr(fd, TCSANOW, &pots);
    closelog();
    if (verbose)
      printf("Closed Log\n");
    exit(0);
}

void send()
{
  char cmd[30];

  sprintf(cmd,"Q1\r");
  write(fd,&cmd,strlen(cmd));
}

//parse the Q1 response
//(227.0 227.0 227.0 022 49.9 27.6 37.8 00001000
// I(V) IFailV O(V)  Load Frq Temp BattV
//
// RETURNS: 0 on success
//         <0 otherwise
int parsedata()
{
  int r;

  if (BUF[0]!='(' && strlen(BUF)!=47 && BUF[47]!='\r') {
    sprintf(str,"invalid ups data"); msg(str,LOG_INFO);
    return -1;
  }

  IPVolts=IPFaultVolts=OPVolts=OPCurr=IPFreq=BatVolts=Temp=
  PowerFail=BatLow=BypassActive=UPSFailed=UPSisSTBY=TestActive=
  ShutdownActive=BeeperOn=0;

  if (verbose) printf("parsing buffer\n");

  r=sscanf(BUF,"(%f%f%f%d%f%f%f %c%c%c%c%c%c%c%c",
      &IPVolts,&IPFaultVolts,&OPVolts,&OPCurr,&IPFreq,&BatVolts,&Temp,
      &PowerFail,&BatLow,&BypassActive,&UPSFailed,&UPSisSTBY,&TestActive,
      &ShutdownActive,&BeeperOn);

  PowerFail-='0'; BatLow-='0'; BypassActive-='0'; UPSFailed-='0';
  UPSisSTBY-='0'; TestActive-='0'; ShutdownActive-='0'; BeeperOn-='0';

  if (r<14) {
    if (verbose) printf("parsedata: short on data (%d<14)\n",r);
    //clear bogus partial info
    PowerFail-='0'; BatLow-='0'; BypassActive-='0'; UPSFailed-='0';
    UPSisSTBY-='0'; TestActive-='0'; ShutdownActive-='0'; BeeperOn-='0';
    return -2;
  }
  if (verbose) {printf("done parsing\n");};
  LastValidDataT=nowT;
  return 0;
}

void logdata()
{
  syslog (LOG_INFO, 
  "time(0):%ld I/V:%3.1f IF/V:%3.1f O/V:%3.1f Load%%:%d Freq:%2.2f"
  " BatV:%2.1f Temp:%2.1f Pwr Fail:%d"
  " BatLo:%d Bypass:%d UPSFail:%d UPStype:%d Test:%d Shutdn:%d Beeper:%d",
  nowT,IPVolts,IPFaultVolts,OPVolts,OPCurr,IPFreq,BatVolts,Temp,
  PowerFail,BatLow,BypassActive,UPSFailed,UPSisSTBY,TestActive,
  ShutdownActive,BeeperOn);
  LastLoggedStatusT=nowT;

}


void printdata()
{
  printf("------------------------------------------------\n");
  printf("Input Voltage:\t%3.1f\nInp.FaultVolts:\t%3.1f\n"
   "Output Voltage:\t%3.1f\nOutputCurrent%%:\t%d\nInput Freq.:\t%2.2f\n"
   "BatteryVoltage:\t%2.1f\nTemperatyre:\t%2.1f\nPower Failure:\t%d\n"
   "Battery Low:\t%d\nBypass Active:\t%d\nUPS Failed:\t%d\n"
   "UPS typ StdBy:\t%d\n"
   "Test in Progr.:\t%d\nShutdn Active:\t%d\nBeeper On:\t%d\n",
  IPVolts,IPFaultVolts,OPVolts,OPCurr,IPFreq,BatVolts,Temp,
  PowerFail,BatLow,BypassActive,UPSFailed,UPSisSTBY,TestActive,
  ShutdownActive,BeeperOn);
  printf("------------------------------------------------\n");

}

int get()
{
int r;

  r=read(fd,&buf,250);
  //printf("got %d data\n",i);
  buf[r]=0;
  //if (buf[r-1]=='\r') buf[r-1]=0;
  return r;
}

//check if DSR is connected
int checkDSR(int fd)
{
  int c=-1;
  int r=0;

  ioctl(fd, TIOCMGET, &c);

  r=(c&TIOCM_DSR);

  if (r) {
    if (verbose) printf("DSR active\n");
  }
  else {
    sprintf(str,"Warning: DSR not active, cable disconnected?");
    msg(str,LOG_ERR);
  }
    
  if (r) return 0;
  else return -1;
}

//runs when power is restored
void runRestoredScript()
{
  char s[256];
  int r;

  if (restore_started) {
    sprintf(str,"restore script already called"); msg(str,LOG_CRIT);
  }
  else{
    sprintf(str,"calling restore script"); msg(str,LOG_CRIT);
#ifdef LOGSCRIPT
    sprintf(s,"%s  2>&1| %s -i -t '%s' -p 'daemon.crit'",
	UPSCRIPT,LOGGER,UPSCRIPT);
#else
    sprintf(s,"%s",UPSCRIPT);
#endif
    restore_started=1;
    r=system(s);
    if (!r) {
      sprintf(str,"executed %s, something could be wrong with your script. "
	  "If LOGSCRIPT was defined, you should have more details above",s); 
      msg(str,LOG_CRIT);
    }
    else{
      sprintf(str,"executed %s, success",s);
      msg(str,LOG_CRIT);
    }
  }
}

//runs when MaxPowerFailDuration time expires
void doshutdown()
{
  char s[256];
  int r;

  if (shutdown_started) {
    sprintf(str,"shutdown script already called"); msg(str,LOG_CRIT);
  }
  else{
    sprintf(str,"initiating shutdown"); msg(str,LOG_CRIT);
#ifdef LOGSCRIPT
    sprintf(s,"%s  2>&1| %s -i -t '%s' -p 'daemon.crit'",
	DOWNSCRIPT,LOGGER,DOWNSCRIPT);
#else
    sprintf(s,"%s",DOWNSCRIPT);
#endif
    shutdown_started=1;
    r=system(s);
    if (!r) {
      sprintf(str,"executed %s, something could be wrong with your script. "
	  "If LOGSCRIPT was defined, you should have more details above",s); 
      msg(str,LOG_CRIT);
    }
    else{
      sprintf(str,"executed %s, success",s);
      msg(str,LOG_CRIT);
    }
  }
}

//make decisions on what actions to take do with the new ups data
int makedecisions()
{
  if (UPSFailed) {
    sprintf(str,"UPS Failed"); 
    msg(str,LOG_CRIT);
    return -1;
  }
  if (!PowerFail) {
    if (PowerFailStart) {
      sprintf(str,"Power Restored"); 
      msg(str,LOG_CRIT);
      runRestoredScript();
    }
    PowerFailStart=0;
    PowerFailDuration=0;
    return 0;
  }


  //if we are here we have a power failure (PowerFail==1)
  //check PowerFailDuration and take actions
  //or wait either for power to return or for
  //PowerFailDuration to surpass the MaxPowerFailDuration

  //first time without power
  PowerFailDuration=nowT-PowerFailStart;
  if (PowerFailStart==0) { //no power failStart time recorded
    restore_started=0;
    PowerFailStart=nowT;
    sprintf(str,"New Power Failure Detected"); msg(str,LOG_CRIT);
    return 1;
  }
  //time to shutdown
  else if (PowerFailDuration>=MaxPowerFailDuration) {
    sprintf(str,
	"No power for %ld seconds ,starting shutdown procedure",
	PowerFailDuration);
    msg(str,LOG_CRIT);
    doshutdown();
  }
  else {
    sprintf(str,"Power Failure in Progress for %ld seconds",
	PowerFailDuration);
    msg(str,LOG_CRIT);
  }
  return 0;
  
}

int main(int argc, char *argv[]) {
    struct  termios pts;  /* termios settings on port */
    int r,count,bufcnt=0;
    struct   timeval tout; 
    struct   timeval * ptout; 
    fd_set inout_fds;
    int interactive=0;
    int ch,i;
    int error=0;

    //parse options
    while ((ch = getopt(argc, argv, "vdi")) != EOF) {
      switch (ch) {
	case 'v':
		verbose++;
		break;
	case 'd':
		daemonmode=1;
		break;
	case 'i':
		interactive=1;
		break;
	case '?':
		error++;
      }
      if (error) {
	fprintf(stderr, "usage: upschk  [ -vdi ]\n");
	fprintf(stderr, "\tv:verbose mode\n");
	fprintf(stderr, "\td:daemon mode (forks in bg)\n");
	fprintf(stderr, "\ti:interactive mode: send cmds to UPS\n\n");
	exit(2);                        
      }
    }//while getopt

    atexit(atexit_hdl);
    openlog ("upschk", LOG_CONS | LOG_PID |LOG_NDELAY, LOG_DAEMON);
    msg("Starting upschk daemon",LOG_INFO);

    //become verbose
    if (daemonmode) {
      switch(fork()) {
	case 0: /* Child */
	  chdir("/");
	  break;

	case -1: /* Error */
	  sprintf(str,"fork error:%s",strerror(errno)); msg(str,LOG_ERR);
	  exit(1);

	default: /* Parent */
	  exit(0);
      }    
    }

    /* Section to configure PORT */
    fd = open( PORT, O_RDWR);

    if (fd < 0) {
      sprintf(str,"%s:%s",PORT,strerror(errno));
      msg(str,LOG_ERR);
      exit(2); 
    }

    /* modify the port configuration */
    tcgetattr(fd, &pts);
    pots = pts;
    pts.c_lflag &= ~ICANON;
    pts.c_lflag &= ~(ECHO | ECHOCTL | ECHONL);
    //pts.c_lflag &= IEXTEN;
    pts.c_cflag |= HUPCL|CS8|CREAD|CLOCAL;
    pts.c_cflag &= ~PARENB;
    pts.c_cflag &= ~PARODD;
    pts.c_cc[VMIN] = 1;
    pts.c_cc[VTIME] = 0;
    pts.c_oflag &= ~(ONLCR|OCRNL|ONLRET|OFILL|OFDEL|OLCUC|OPOST);
    pts.c_iflag &= ~ICRNL;
    pts.c_iflag &= IGNPAR;


    r=cfsetospeed(&pts, B2400);   /* All this sets the port to UPS defaults */

    r=tcsetattr(fd, TCSANOW, &pts);
    if (r<0) {
      sprintf(str,"setting line attributes:%s",strerror(errno));
      msg(str,LOG_ERR);
      exit(1);
    }

    r=checkDSR(fd);

    //send some data to clear previous status
    for (i=0;i<2;i++) {
      if (write (fd, "C\r", 2) < 1) { 
	sprintf(str,"send data to ups (0):%s",strerror(errno)); 
	msg(str,LOG_ERR);
      }
      if (verbose) printf("written request (C)\n");
      sleep(1);
    }

    r = tcflush(fd, TCIOFLUSH); if (r == -1) { 
      sprintf(str,"tcflush (0):%s",strerror(errno)); 
      msg(str,LOG_ERR);
      close(fd); 
      exit(-1); 
    }

    BUF[0]=0;

    if (interactive){
      no_wait();
    }
    if (interactive)
      printf("\n1: Q1 Status Inquiry\n"
	"2: T  10 Seconds Test\n"
	"3: TL Test until Battery Low \n"
	"4: Q  Turn On/Off beep\n"
	"5: C  Cancel Shut Down Command\n"
	"6: CT Cancel Test Command\n"
	"7: I  UPS Information Command\n"
	"8: F  UPS Rating Information\n"
	"q: Quit\n"
	"Command:");fflush(stdout);

    //start query loop
    for(;;) {
      FD_ZERO(&inout_fds);
      FD_SET(fd,&inout_fds);

      if (interactive){
	ptout=0;
	FD_SET(0,&inout_fds);//stdin
      }
      else {
	ptout=&tout;
	tout.tv_sec=2;
      }

      tout.tv_usec=0;
      buf[0]=0;
      cmd[0]=0;

      if ((count = select (6, &inout_fds,(fd_set *)0,(fd_set *)0, ptout)) < 0) {
        sprintf(str,"select:%s",strerror(errno)); msg(str,LOG_ERR);
      }

      nowT=time(0); //update current time

      if (FD_ISSET(fd,&inout_fds)) { //Response from UPS
	if (verbose) printf("Got Data\n");
	sleep(1);  // optional sleep to avoid multiple reads
	r=get(); bufcnt+=r;
	if (bufcnt<256) strcat(BUF,buf);
	if (verbose) printf("[%s]|",buf); fflush(stdout); //print partial reads

	if (!interactive && (bufcnt==47)){
	  if (verbose) printf("\nBUF[%d,%d] was [%s]\n",strlen(BUF),bufcnt,BUF);
	  r=parsedata();
	  LastDataT=nowT;
	  if (r==0) { //success
	    if (verbose) printdata();

	    //log all ups data every LOGDATAINTERVAL seconds
	    if ((!LastLoggedStatusT)|| 
		((nowT-LastLoggedStatusT)>LOGDATAINTERVAL)) {
	      logdata();
	    }

	    //makedecisions
	    makedecisions();
	  }
	} 
	else if (interactive){ //interactive mode, show commands
	  BUF[strlen(BUF)-1]=0; //get rid of \r
	  printf("\nResponse:[%d,%d] was [%s]\n",strlen(BUF),bufcnt,BUF);
	  r=parsedata();
	  if (!r&&verbose) printdata();
	  printf("\n1: Q1 Status Inquiry\n"
	    "2: T  10 Seconds Test\n"
	    "3: TL Test until Battery Low \n"
	    "4: Q  Turn On/Off beep\n"
	    "5: C  Cancel Shut Down Command\n"
	    "6: CT Cancel Test Command\n"
	    "7: I  UPS Information Command\n"
	    "8: F  UPS Rating Information\n"
	    "q: Quit\n"
	    "Command:");fflush(stdout);
	}
	bufcnt=0;
	BUF[0]=0;
      }
      else if (FD_ISSET(0,&inout_fds)) { //user input
	r=read(0,&cmd,64);

	if ((r>0) && (cmd[0]=='1')) sprintf(cmd,"Q1\r"); 
	else if ((r>0) && (cmd[0]=='1')) sprintf(cmd,"Q1\r"); 
	else if ((r>0) && (cmd[0]=='2')) sprintf(cmd,"T\r"); 
	else if ((r>0) && (cmd[0]=='3')) sprintf(cmd,"TL\r"); 
	else if ((r>0) && (cmd[0]=='4')) sprintf(cmd,"Q\r"); 
	else if ((r>0) && (cmd[0]=='5')) sprintf(cmd,"C\r"); 
	else if ((r>0) && (cmd[0]=='6')) sprintf(cmd,"CT\r"); 
	else if ((r>0) && (cmd[0]=='7')) sprintf(cmd,"I\r"); 
	else if ((r>0) && (cmd[0]=='8')) sprintf(cmd,"F\r"); 
	else if ((r>0) && (cmd[0]=='q')) exit(0);
	else
	  cmd[0]=0;

	//send command
	if (cmd[0]) {
	  if (write (fd, cmd,strlen(cmd)) < 1) {
	    sprintf(str,"send data to ups (1):%s",strerror(errno)); 
	    msg(str,LOG_INFO);
	  }
	  cmd[strlen(cmd)-1]=0; //get rid of \r
	  printf("Sent[%d]:%s\\r\n",strlen(cmd),cmd);sleep(1);
	}
      }
      else if (!interactive) { //timeout, never here in interactive mode
	if (verbose) {printf("\nNothing on buffer, querying ");fflush(stdout);}
	if (write (fd, "Q1\r", 3) < 1) { 
	  sprintf(str,"send data to ups (2):%s",strerror(errno)); 
	  msg(str,LOG_INFO);
	}

	//Connection Failure
	if (LastValidDataT &&((nowT-LastValidDataT)>10)) {
	  r=checkDSR(fd);
	  if (verbose) 
	    printf("%ld seconds without valid data\n",nowT-LastValidDataT);
	}

	//Connection Failure
	if (LastDataT &&((nowT-LastDataT)>10)) {
	  r=checkDSR(fd);
	  if (verbose) 
	    printf("%ld seconds without data\n",nowT-LastDataT);
	}

      }
      else {
        sprintf(str,"select:%s",strerror(errno)); msg(str,LOG_ERR);
	sleep(1);
      }
    }//for

    return(0);
}
