/*

    Little Wire Button Daemon

	This daemon configures the Little Wire USB device as input and treats
	each pin as active-low input. If an pin is held low, the deamon runs a
	command / script. For each of the four pins you can provide a separate
	command. The pins mus be release/go high before a command is run a
	second time.

	Created: July 2012

	by Markus Baertschi <markus@markus.org>

	compile with:
	gcc `libusb-config --cflags` `libusb-config --libs` -Ilibrary -O -g -D LINUX -o lwbuttond lwbuttond.c opendevice.o littleWire.o littleWire_util.o littleWire_servo.o `libusb-config --libs`
*/

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h>
#include "littleWire.h"
#include "littleWire_util.h"

#define DEBOUNCE 100		// Debounce, miliseconds, unsed by delay()
#define LWBDVERS "1.1"		// Version string

char pintable[5] = { 0,1,2,5,0 }; 	// mapping from pin-name to pin, 1st element not used
char pinactive[5] = {0,0,0,0,0};	// =1 if pin is actively monitored
char pinpressed[5] = {0,0,0,0,0};	// debouncing, pin must go high between activations
int debug = 0;						// debug flag
int verbose = 0;					// verbose flag

/*
	display a usage message
*/
void usage () {
	printf ("Usage: lwbuttond [-d] [-v] <command1> <command2> <command3> <command4>\n");
	printf ("       -d         debug, prevent backgrounding\n");
	printf ("       -v         verbose, print progress information\n");
	printf ("       <command>  command line to be executed for a button press\n");
	printf ("                  must be quoted if it contains white space: \"cmd arg\"\n");
	printf ("                  to skip a button provide an empty string: \"\"\n");
	printf ("Example:\n");
	printf ("    lwbuttond \"beep -f 300\" \"\" \"beep -f 400\" \"\"\n");
	printf ("        run the beep command for buttons 1 and 3\n");
	printf ("        buttons 2 and 4 are not used\n");
}

/*
	send the program to the background (daemonize)
*/
void background() {
	int fd;

	fd = open("/dev/null", O_WRONLY|O_APPEND);
	dup2(fd, 0);
	dup2(fd, 1);
	dup2(fd, 2);
	close(fd);

	signal(SIGHUP, SIG_IGN);
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGTSTP,SIG_IGN);

	if (fork())	exit(0);

	return;
}

/*
    Run a command for a button press
*/
void run_command (int pin, char * cmd) {
	int rc;
	syslog(LOG_INFO,"Button %d pressed. Running command '%s'",pin,cmd);
	if (verbose) printf("Button %d pressed. Running command: '%s'\n",pin,cmd);
	if (!fork()) {
		rc=system(cmd);
		if (verbose) printf("Command '%s' completed (Button %d), rc=%d\n",cmd,pin,rc);
		syslog(LOG_INFO,"Command '%s' completed (Button %d), rc=%d",cmd,pin,rc);
		exit (0);
	}
}

#define BUFSIZE 160
int main(int argc, char *argv[]) {
	littleWire *myLittleWire = NULL;
	unsigned char version;
	int rc;		// return code from system()
	int i;		// index for various loops
	int pin;	// index for input number
	char c;
	char buf[BUFSIZE];   // buffer for syslog output

	/*
		work the command line switches
	*/
	while (--argc > 0 && (*++argv)[0] == '-') {
        while (c = *++argv[0]) {
            switch (c) {
                case 'd':
                   	debug = 1;
                   	break;
               	case 'h':
                   	usage();
                   	exit (0);
                   	break;
               	case 'v':
                   	verbose = 1;
                   	break;
               	default:
                   	printf("lwbuttond: Illegal command flag -%c\n", c);
                   	usage();
                   	exit (1);
                }
        }
    }

	if (verbose) printf("lwbuttond: Little Wire button daemon version %s starting\n",LWBDVERS);

	/*
		check for the other command line arguments -> 1 argument per button
	*/
    if (argc < 1) {
        usage();
        exit (1);
    } else {
   		for(i=0; i<argc; i++) {
   			if (*argv[i]!='\0') {
	   			if (verbose) printf("lwbuttond: Button %d command: %s\n",i+1,argv[i]);
	   			pinactive[i+1] = 1;
   			} else {
   				if (verbose) printf("lwbuttond: Button %d is inactive (no command set)\n",i+1);
   			}
   		}
   	}

	/*
		initialize syslog
	*/
	if (verbose) printf("lwbuttond: Setting up syslog\n");
	openlog("lwbuttond",LOG_PID,LOG_USER);
	syslog(LOG_INFO,"Little Wire button daemon version %s starting\n",LWBDVERS);

	/*
		daemonize, if not debug
	*/
	if (!debug) {
		if (verbose) printf("lwbuttond: Backgrounding\n");
		verbose=0;		// no point writing to a closed stdout
		background();
	}

	while (1) {
        /*
            initialize connection to Little Wire
        */
        if (verbose) printf("lwbuttond: Connecting to Litte Wire device\n");
        myLittleWire = littleWire_connect();

        if(myLittleWire == NULL) {
            perror("lwbuttond: Failed to connect to Little Wire device, retry in 10s");
            syslog(LOG_ERR,"Failed to connect to Little Wire device, retry in 10s\n");
            sleep(10);
        } else {

            version = readFirmwareVersion(myLittleWire);

            if (rc=littleWire_error()) {
                printf("lwbuttond: Error reading firmware version: '%s' (%d)\n",littleWire_errorName(),rc);
                snprintf(buf,BUFSIZE,"Error reading firmware version: '%s' (%d)\n",littleWire_errorName(),rc);
                syslog(LOG_INFO,buf);
            }

            rc=snprintf(buf,BUFSIZE,"Connected to Little Wire device, firmware version %d.%d\n",((version & 0xF0)>>4),(version&0x0F));
            syslog(LOG_INFO,buf);

            if (verbose) {
                printf("lwbuttond: Little Wire firmware version: %d.%d\n",((version & 0xF0)>>4),(version&0x0F));
            }

            /*
                configure Little Wire device
            */
            if (verbose) printf("lwbuttond: Setting up pins for input\n");
            for(pin=1; pin<=4; pin++) {
                if (pinactive[pin]) {
                    pinMode(myLittleWire, pintable[pin], INPUT);
                    internalPullup(myLittleWire, pintable[pin], ENABLE);
                }
            }

            /*
                read the device / read pins / execute commands forever
            */
            while(1){
                for(pin=1; pin<=4; pin++) {		// loop through the 4 pins
                    if (pinactive[pin]) {		// only look at active pins
                        if ( digitalRead(myLittleWire, pintable[pin]) == LOW ){
                            delay(DEBOUNCE);    // wait a bit for debouncing
                            if( digitalRead(myLittleWire, pintable[pin]) == LOW ){
                                if (pinpressed[pin]!=1) {   // check
                                    run_command(pin,argv[pin-1]);
                                    pinpressed[pin]=1;		// save status to prevent command repetition
                                }
                            }
                        } else pinpressed[pin]=0;			// reset status after pin is back high
                    }
                }
                if (rc=littleWire_error()) {
                    fprintf(stderr,"lwbuttond: Error: littleWire error '%s' (%d), retrying\n",littleWire_errorName(),rc);
                    syslog(LOG_ERR,"Error: littleWire error '%s' (%d), retrying",littleWire_errorName(),rc);
                    break;
                }
                delay(DEBOUNCE);
            }
        }
	}
}

