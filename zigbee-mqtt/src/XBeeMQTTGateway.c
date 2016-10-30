/*
 * XBee_MQTT_Gateway
 *
 *  Daemon that runs on the OpenWRT router and captures ZigBee Coordinator API Frames to publish to MQTT broker.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "mosquitto.h"
#include "rjwserial.h"

#define TRUE 1
#define FALSE 0
#define USB_PORT_NAME "/dev/ttyUSB0"                    // USB port for XBee adapter
#define MAX_FRAME_LENGTH 255
 
const char c_cFrameMarker = 0x7E;                       // byte indicating frame start
const int c_nIndexFrameLengthHiByte = 1;                // byte offset of Hi length
const int c_nIndexFrameLengthLoByte = 2;                // byte offset of Lo length
const int c_nFrameOverheadBytes = 4;                    // marker(1) + length(2) + checkdigit(1)
const char c_szProgramName[ ] = "XBeeMQTTGateway";
const intc_nValidChecksum = 0xFF;
const int  c_nApiIdentifierReceivePacket = 0x90;        // only capturing this frame type
const int c_nApiIdentifierIndex = 3;                    // index within frame for API Identifier

static char cPublishBuffer[ 32 + MAX_FRAME_LENGTH ];    // work buffer includes room for time
static int s_bShutdown = FALSE;

int startSerialInterface( struct mosquitto *pMosq );    // forward reference

void catchSignal(int nSignal )
{
    printf("Signal %d caught! %s exiting\n", nSignal, c_szProgramName );
    s_bShutdown = TRUE;                                 // set the shutdown flag
    signal( nSignal, SIG_IGN );                         // and ignore the signal otherwise
}

void connectCallback(struct mosquitto *pMosq, void *udata, int res)
 {
}

void messageCallback(struct mosquitto *pMosq, void *obj, const struct mosquitto_message *pMsg)
{
 }

void mainProcess( )
{
    struct mosquitto *pMosq = NULL;
    int keepalive = 60;
    int rc = 0;
    bool bDisableHostNameChecking = false;              // set true if host name doesn't match site name

    mosquitto_lib_init();

    pMosq = mosquitto_new("XBeeMQTTGateway",true, NULL);

    if ( pMosq ) {
	mosquitto_connect_callback_set( pMosq, connectCallback);
	mosquitto_message_callback_set( pMosq, messageCallback );
	rc = mosquitto_username_pw_set( pMosq, "myMQTTAccount", "myMQTTPassword");
	rc = mosquitto_tls_set(pMosq, "/etc/cacert.crt", NULL, NULL, NULL, NULL);
	rc = mosquitto_tls_insecure_set( pMosq, bDisableHostNameChecking );
	rc = mosquitto_connect_bind(pMosq, "192.168.2.138", 1883, keepalive, NULL);
	rc = mosquitto_loop_start( pMosq );
	startSerialInterface( pMosq );
	mosquitto_disconnect( pMosq );
	mosquitto_loop_stop( pMosq, false );
	mosquitto_destroy(pMosq);
	pMosq = 0;
    }
    mosquitto_lib_cleanup();
}

bool isValidChecksum(char *pcBuffer, int nLength  )
{
    int nCheckDigit = 0, i = 0;

    for ( i = c_nIndexFrameLengthLoByte + 1; i < nLength; i++ )
    {
        nCheckDigit += pcBuffer[i];
    }
    return ( nCheckDigit & 0xFF) == c_nValidChecksum;
}

int processReceivePacketFrame( char *pcBuffer, int nLength, struct mosquitto *pMosq  )
{
    int rc = 0, i = 0, nMid = 0, nOffset=0, nPublishLen = 0;
    char szTopic[256] = "XBeeData/", szTemp[8]="";
    for ( i = 4; i < 12; i++ )
    {
	sprintf(szTemp, "%02X", (int)(pcBuffer[i] & 0xFF));
	strcat( szTopic, szTemp );
    }
    sprintf( cPublishBuffer, "%ld,", (unsigned long)time(NULL) );
    nOffset = strlen( cPublishBuffer );
    memcpy( &cPublishBuffer[nOffset], pcBuffer+15, nLength-16 );
    nPublishLen = nOffset + nLength - 16;
    rc = mosquitto_publish( pMosq, &nMid, szTopic, nPublishLen, cPublishBuffer , 0,false);
    if ( MOSQ_ERR_NO_CONN == rc ) // try once to reconnect
    {
	rc = mosquitto_reconnect( pMosq );
	if ( MOSQ_ERR_SUCCESS == rc )
	{
	    rc = mosquitto_publish( pMosq, &nMid, szTopic, nPublishLen, cPublishBuffer , 0,false);
	}
    }
    return rc;
}

int processFrame( char *pcBuffer, int nLength, struct mosquitto *pMosq  )
{
    int rc = 0;
    if ( pcBuffer && (nLength > 4) && isValidChecksum(pcBuffer, nLength ) )
    {
	switch ( pcBuffer[ c_nApiIdentifierIndex ] & 0xFF  )
	{
	case 0x90:
	    rc = processReceivePacketFrame( pcBuffer, nLength, pMosq);
	    break;
	default:   // ignore the rest
	    break;
	}
    }
    return rc;
}
 
int startSerialInterface( struct mosquitto *pMosq )
{
    SERIALHANDLE handle = SERIALHANDLE_INVALID;
    int rc = 0;
    char cRawBuffer[256];
    char cFrameBuffer[ MAX_FRAME_LENGTH ];
    int nFrameBytes = 0;
    int nExpectedFrameLength = 0;
    bool bInFrame = false;

    handle = serial_open( USB_PORT_NAME, "r" );

    if ( SERIALHANDLE_INVALID != handle )
    {
	bool bLoop = true;
	int i = 0;

	serial_set_timeout( 1000, handle );

	rc = serial_config( handle, SERIALBAUDRATE_115200, SERIALDATABITS_8, SERIALSTOPBITS_1 , SERIALPARITY_None, SERIALFLOWCONTROL_CtsDtr  );

	while ( bLoop && !s_bShutdown )
	{
	    rc = serial_read ( cRawBuffer, sizeof(cRawBuffer), handle );

	    if ( rc > 0 )
	    {
		for ( i = 0; i < rc; i++ )
		{
		    if ( bInFrame )
		    {
			cFrameBuffer[nFrameBytes++] = cRawBuffer[i];
 
			if ( c_nIndexFrameLengthLoByte + 1 == nFrameBytes ) // now long enough to determine expected frame length
			{
			    nExpectedFrameLength = cFrameBuffer[c_nIndexFrameLengthHiByte];
			    nExpectedFrameLength <<= 8;
			    nExpectedFrameLength += cFrameBuffer[c_nIndexFrameLengthLoByte];
			    nExpectedFrameLength += c_nFrameOverheadBytes;
			}
			else if ( nExpectedFrameLength == nFrameBytes )		// frame should now be complete
			{
			    rc = processFrame( cFrameBuffer, nFrameBytes, pMosq );
			    bInFrame = false;
			}
		    }
		    else
		    {
			if ( c_cFrameMarker == cRawBuffer[i] )	// only interested if frameMarker else discard
			{
			    cFrameBuffer[0] = c_cFrameMarker;
			    nExpectedFrameLength = 0;
			    bInFrame = true;
			    nFrameBytes = 1;
			}
		    }
		}
	    }
	    usleep( 1 );
	}
	serial_close( handle );
	handle = SERIALHANDLE_INVALID;
    }
    return 0;
}
 
int main( int argc, char *argv[] )
{
    signal(SIGALRM, catchSignal );
    signal(SIGTERM, catchSignal );
    signal(SIGQUIT, catchSignal );
    signal(SIGINT, catchSignal );
    printf("%s\n", c_szProgramName);
    mainProcess();
    return 0;
}
