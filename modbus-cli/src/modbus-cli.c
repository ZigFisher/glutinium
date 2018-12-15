#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <modbus.h>

#define BUFSIZE 16

int main(int argc, char **argv)
{
	modbus_t *ctx;
	uint16_t tab_reg[100];
	int rc;
	int i;

    int opt;
	int rflag = 0;
	int tflag = 0;
	char port[BUFSIZE+1];
	char addr[BUFSIZE+1];
	char slave[BUFSIZE+1];
	char start[BUFSIZE+1];
	char num[BUFSIZE+1];
	char device[BUFSIZE+1];
	char baudrate[BUFSIZE+1];
	char function[BUFSIZE+1];
	char value[BUFSIZE+1];
	
	// Exit with error code if the user does not provide any arguments
	if( argc < 2) {
		fprintf(stderr, "%s: try '%s -h' for more information\n", argv[0], argv[0]);
        exit(EXIT_FAILURE);
	}

	// Parse the arguments and flags parsed from the command line
    while ((opt = getopt(argc, argv, "rtd:b:i:p:f:s:a:n:v:h")) != -1) {
        switch (opt) {
		case 'r':
			rflag = 1;
			break;
		case 't':
			tflag = 1;
			break;
        case 'd':
			snprintf( device, BUFSIZE, "%s", optarg );
            break;
		case 'b':
			snprintf( baudrate, BUFSIZE, "%s", optarg );
            break;
		case 'i':
			snprintf( addr, BUFSIZE, "%s", optarg );
            break;
		case 'p':
			snprintf( port, BUFSIZE, "%s", optarg );
			break;
		case 'f':
			snprintf( function, BUFSIZE, "%s", optarg );
			break;
		case 's':
			snprintf( slave, BUFSIZE, "%s", optarg );
			break;
		case 'a':
			snprintf( start, BUFSIZE, "%s", optarg );
			break;
		case 'n':
			snprintf( num, BUFSIZE, "%s", optarg );
			break;
		case 'v':
			snprintf( value, BUFSIZE, "%s", optarg );
			break;
		case 'h':
			fprintf(stdout, "Usage: %s [options...]\n", argv[0]);
			fprintf(stdout, "Examples:\n");
			fprintf(stdout, "\t %s -r -d /dev/ttyS1 -b 9600 -f 4 -s 1 -a 0 -n 20\n", argv[0]);
			fprintf(stdout, "\t %s -t -i 192.168.1.1 -p 502 -f 4 -s 1 -a 0 -n 20\n", argv[0]);
			fprintf(stdout, "Options: (R) means Modbus RTU, (T) means Modbus TCP\n");
			fprintf(stdout, " -r,\t Establish a Modbus RTU connection\n");
			fprintf(stdout, " -t,\t Establish a Modbus TCP connection\n");
			fprintf(stdout, " -d,\t Define the device, default is /dev/ttyS1 (R)\n");
			fprintf(stdout, " -b,\t Define the baudrate, default is 9600 (R)\n");
			fprintf(stdout, " -i,\t Define the IP address (T)\n");
			fprintf(stdout, " -p,\t Define the port (T)\n");
			fprintf(stdout, " -f,\t Define the function code.\n");
			fprintf(stdout, "\t\t 3=Read Holding Registers\n");
			fprintf(stdout, "\t\t 4=Read Input Registers\n");
			fprintf(stdout, "\t\t 5=Write Single Coil\n");
			fprintf(stdout, "\t\t 6=Write Single Register\n");
			fprintf(stdout, " -s,\t Define the slave ID to send the Modbus request to\n");
			fprintf(stdout, " -a,\t Define the start address\n");
			fprintf(stdout, " -n,\t Define the number of registers (for -f = 3 or 4)\n");
			fprintf(stdout, " -v,\t Define the value to write TRUE or FALSE for (for -f = 5 or 6)\n");
			exit(EXIT_SUCCESS);
        default: /* '?' */
            fprintf(stderr, "%s: try '%s -h' for more information\n", argv[0], argv[0]);
            exit(EXIT_FAILURE);
        }
    }

	if ( tflag == 1 ) {
		ctx = modbus_new_tcp(addr, atoi(port));		
	}
	else if ( rflag == 1 ) {
		ctx = modbus_new_rtu(device, atoi(baudrate), 'N', 8, 1);
	}
	
	if (ctx == NULL) {
		fprintf(stderr, "Unable to allocate libmodbus context\n");
		return -1;
	}
	if (modbus_connect(ctx) == -1) {
		fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
		modbus_free(ctx);
		return -1;
	}
	modbus_set_slave(ctx, atoi(slave));

	if ( atoi(function) == 3 ) {		
		rc = modbus_read_registers(ctx, atoi(start), atoi(num), tab_reg);
	}
	else if ( atoi(function) == 4 ) {
		rc = modbus_read_input_registers(ctx, atoi(start), atoi(num), tab_reg);
	}
	else if ( atoi(function) == 5 ) {
		rc = modbus_write_bit(ctx, atoi(start), value);
	}
	else if ( atoi(function) == 6 ) {
		rc = modbus_write_register(ctx, atoi(start), atoi(value));
	}
	else {
		fprintf(stderr, "Function code %s not implemented.\n", function);
	}
	
	if (rc == -1) {
		fprintf(stderr, "%s\n", modbus_strerror(errno));
		return -1;
	}

	if ( tflag == 1 ) {
		printf("Connected to %s:%s\n", addr, port);
	}
	else if ( rflag == 1 ) {
		printf("Connected to %s @ %sbps\n", device, baudrate);
	}
	printf("Slave: %d\n", atoi(slave));
	printf("Start address: %d\n", atoi(start));
	printf("Num of registers: %d\n", atoi(num));
	
	if ( atoi(function) == 3 || atoi(function) == 4 ) {
		for (i=0; i < rc; i++) {
			printf("reg[%2d]=%6d \t (0x%X)\n", (i+atoi(start)), tab_reg[i], tab_reg[i]);
		}
	}	

	modbus_close(ctx);
	modbus_free(ctx);

    exit(EXIT_SUCCESS);
}