//-----------------------------------------------------------------------------------------
// Author:    Nelson Neves
// Contact:   nelson.s.neves@gmail.com / www.botdream.com
//
// Objective: Create a C application to parse GPS data from specific module that would run
//            on Bifferboard hardware for several X86 Linux based OSs (OpenWrt, OnFlashSystem, etc).
//            Parsed data will be converted into specific structures making it possible to use
//            these values for calculations, binary export, database inserts, etc.
//            If it's only desired to get access to GPS NMEA strings without having the need
//            for data conversion then it will only be necessary to connect to GPS module through
//            the serial port and fetch data directly in text mode. Please note that this code is
//            not optimized and it was created by the author as a proof of concept.
//            There are some very good C livraries to parse NMEA code: http://dmh2000.com/nmea/nmeap.shtml
//
// Hardware:
// Bifferboard: http://bifferos.bizhat.com
//              http://sites.google.com/site/bifferboard
// GPS Module:  32 Channels LS20031 GPS 5Hz Receiver
//              http://www.sparkfun.com/commerce/product_info.php?products_id=8975
//              http://www.coolcomponents.co.uk/catalog/product_info.php?produtcs_id=210
//
// Tutorial:    http://sites.google.com/site/bifferboard/Home/howto/connect-to-a-serial-gps
//
// Extra
// GPS NMEA DATA:
// http://www.gpsinformation.org/dale/nmea.htm#GGA
// http://www.gpsinformation.org/dale/nmea.htm#RMC
//
// License:     You may use, distribute or change it as you wish. Please don't forget to
//              include a reference to the original author :).
//              Since this also use dietlibc to shrink final binary please read their license
//              information and make sure that you comply with it!
//-----------------------------------------------------------------------------------------
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
//-----------------------------------------------------------------------------------------
#define BAUDRATE1 B4800
#define BAUDRATE2 B38400
#define BAUDRATE3 B57600
#define MODEMDEVICE "/dev/ttyGPS0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
//-----------------------------------------------------------------------------------------
#define FALSE 0
#define TRUE 1
//-----------------------------------------------------------------------------------------
#define NMEA_NA    0
#define NMEA_GPGGA 1
#define NMEA_GPRMC 2
//-----------------------------------------------------------------------------------------
static volatile int exec_end = FALSE;
//-----------------------------------------------------------------------------------------
  char wgetbuf[1024];
  time_t t;

//-----------------------------------------------------------------------------------------
// structure for NMEA parsed commands - stringlist
//-----------------------------------------------------------------------------------------
struct STRINGLIST{
  char data[25];
}stringlist[20];

//-----------------------------------------------------------------------------------------
// structures for $GPGGA and $GPRMC
//-----------------------------------------------------------------------------------------
struct GPGGA{

  double fixedtime;
  double latitude;
  double longitude;
  int qualitycode;
  char quality[20];
  int satellites;
  double hdop;
  double altitude;
  double altitudegeoid;
  double geoid;

}gpgga;
//-----------------------------------------------------------------------------------------

struct GPRMC{

  double fixedtime;
  char warncode;
  char warn[20];
  double latitude;
  double longitude;
  double speed;
  double course;
  unsigned long date;
  double magvar;
  char magvardir;

}gprmc;
//-----------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------
void clear_stringlist(void)
{
  // clear string list
  int j;
  for(j=0; j<20; j++)
    strncpy(stringlist[j].data, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 24);
}
//-----------------------------------------------------------------------------------------

double convert_gps_degminsec_decimal(char *p_data)
{
  int counter;
  char *ptr;
  char latlong[4], decimalminutes[8];
  double hres, lres, result;

  ptr = p_data;
  counter = 0;
  while(*ptr != '\0' && *ptr != '.')
  {
    ptr++;
    counter++;
  }
  if(*ptr != '.')
    return 0.0;

  // extract lat/long
  strncpy(latlong, p_data, counter-2);
//printf("latlong:%s\r\n",latlong);

  // extract decimalminutes
  strncpy(decimalminutes, ptr-2, 7);
//printf("decimalminutes:%s\r\n",decimalminutes);

  hres = atof(latlong);
  lres = atof(decimalminutes)/60;
  result = hres + lres;

//printf("%.6f\r\n",hres);
//printf("%.6f\r\n",lres);
//printf("%.6f\r\n\r\n",result);

  return result;
}
//-----------------------------------------------------------------------------------------

void get_gpgga_quality(int qualitycode, char* p_quality)
{
  if(qualitycode == 0)
    strcpy(p_quality, "Invalid");
  else if(qualitycode == 1)
    strcpy(p_quality, "GPS Fix (SPS)");
  else if(qualitycode == 2)
    strcpy(p_quality, "DGPS Fix");
  else if(qualitycode == 3)
    strcpy(p_quality, "PPS Fix");
  else if(qualitycode == 4)
    strcpy(p_quality, "Real Time Kinematic");
  else if(qualitycode == 5)
    strcpy(p_quality, "Float RTK");
  else if(qualitycode == 6)
    strcpy(p_quality, "Estimated (dead Recko.)");
  else if(qualitycode == 7)
    strcpy(p_quality, "Manaul Input Mode");
  else if(qualitycode == 8)
    strcpy(p_quality, "Simulation Mode");
}
//-----------------------------------------------------------------------------------------

void get_gprmc_warn(char warncode, char* p_warn)
{
  if(warncode == 'V')
    strcpy(p_warn, "Void");
  else if(warncode == 'A')
    strcpy(p_warn, "Active");
}
//-----------------------------------------------------------------------------------------

int parsedata(char *nmeastring)
{
  unsigned short int found_start_cmd, found_end_cmd;
  char *p_init, *p_final;
  int count_stringlist, count_pointers_chars;
  int nmea_cmd_type;

//printf("PARSER: %s", nmeastring);

  // initialize vars
  found_start_cmd = FALSE;
  found_end_cmd = FALSE;
  count_stringlist = 0;
  count_pointers_chars = 0;
  p_init  = nmeastring;
  p_final = nmeastring;
  nmea_cmd_type = NMEA_NA;
  clear_stringlist();

  for(/*already initialized*/;*p_final != '\0';p_final++)
  {
    // checks for NMEA initial char '$'
    if(*p_final == '$')
    {
//printf("Found initial string\r\n");
      found_start_cmd = TRUE;
      p_init = p_final;
      continue;
    }

    // will only allow to continue after receiving NMEA initial char '$'
    if(found_start_cmd == FALSE)
    {
//printf("skip\r\n");
      continue;
    }

    // checks if data is a separator char ',' or end char '\0' or carriage return '\r' or new line char '\n'
    if(found_end_cmd == FALSE && *p_final != ',' && *p_final != '\0' && *p_final != '\r' && *p_final != '\n')
    {
      count_pointers_chars++;
      continue;
    }
    else
    {
//printf("Found end -> %d\r\n", count_pointers_chars);
      found_end_cmd = TRUE;
    }
    //-------------------------------------------------------------------------------------
    // copy NMEA sub-string to stringlist
    //-------------------------------------------------------------------------------------
    if(count_pointers_chars > 0)
      strncpy(stringlist[count_stringlist].data, p_init+1, count_pointers_chars);
    else
      stringlist[count_stringlist].data[0] = '\0';
    //printf("%s\r\n",stringlist[count_stringlist].data);
    count_stringlist ++;

    //-------------------------------------------------------------------------------------
    // sets p_init from p_final last address and resets char counter
    //-------------------------------------------------------------------------------------
    //if(*p_final != '\0') // skips ',' for the next sub-string
    //  p_final++;
    p_init = p_final;
    count_pointers_chars = 0;
    found_end_cmd = FALSE;
  }

//printf("Final stage\r\n");

  /* int i;
  for(i=0; i<count_stringlist; i++)
    printf("%s\r\n", stringlist[i].data); */

  //---------------------------------------------------------------------------------------
  // determinate NMEA command type
  //---------------------------------------------------------------------------------------
  if(strcmp("GPGGA", stringlist[0].data) == 0)
  {
//printf("GGA\r\n");
    nmea_cmd_type = NMEA_GPGGA;
  }
  else if(strcmp("GPRMC", stringlist[0].data) == 0)
  {
//printf("RMC\r\n");
    nmea_cmd_type = NMEA_GPRMC;
  }
  else
  {
//printf("NA\r\n");
    nmea_cmd_type = NMEA_NA;
    return nmea_cmd_type;
  }

  //---------------------------------------------------------------------------------------
  // set structure data
  //---------------------------------------------------------------------------------------
  if(nmea_cmd_type == NMEA_GPGGA)
  {
    short int signal;

    // fixed taken at --:--:-- UTC
    gpgga.fixedtime    = atof(stringlist[1].data);

    // latitude
    if(strcmp("N",stringlist[3].data)==0)
      signal = 1;
    else
      signal = -1;
    gpgga.latitude     = signal*convert_gps_degminsec_decimal(stringlist[2].data);

    // longitude
    if(strcmp("E",stringlist[5].data)==0)
      signal = 1;
    else
      signal = -1;
    gpgga.longitude    = signal*convert_gps_degminsec_decimal(stringlist[4].data);

    // Fix quality
    gpgga.qualitycode   = atoi(stringlist[6].data);
    get_gpgga_quality(gpgga.qualitycode, gpgga.quality);

    // number of satelites being tracked
    gpgga.satellites    = atoi(stringlist[7].data);

    // horizontal dilution position
    gpgga.hdop          =  atoi(stringlist[8].data);

    // altitude, meters above mean sea level
    gpgga.altitude      = atof(stringlist[9].data);

    // height of geoid (mean sea level) above WGS84
    gpgga.altitudegeoid = atof(stringlist[11].data);

//    printf("GPGGA|%f|%.6f|%.6f|%d|%s|%d|%f|%.1f|%.1f|%.1f\n",
//            gpgga.fixedtime,
//            gpgga.latitude,
//            gpgga.longitude,
//            gpgga.qualitycode,
//            gpgga.quality,
//            gpgga.satellites,
//            gpgga.hdop,
//            gpgga.altitude,
//            gpgga.altitudegeoid,
//            gpgga.geoid
//    );
  }
  else if(nmea_cmd_type == NMEA_GPRMC)
  {
    short int signal;

    // fixed taken at --:--:-- UTC
    gprmc.fixedtime    = atof(stringlist[1].data);

    // status A=Active or V=Void
    gprmc.warncode     = stringlist[2].data[0];
    get_gprmc_warn(gprmc.warncode, gprmc.warn);

    // latitude
    if(strcmp("N",stringlist[4].data)==0)
      signal = 1;
    else
      signal = -1;
    gprmc.latitude     = signal*convert_gps_degminsec_decimal(stringlist[3].data);

    // longitude
    if(strcmp("E",stringlist[6].data)==0)
      signal = 1;
    else
      signal = -1;
    gprmc.longitude    = signal*convert_gps_degminsec_decimal(stringlist[5].data);

    // speed over the ground in knots
    gprmc.speed        = atof(stringlist[7].data);

    // track angle in degrees
    gprmc.course       = atof(stringlist[8].data);

    // date DDMMYY
    gprmc.date         = strtoul(stringlist[9].data, NULL, 10);

    // magnetic variation
    gprmc.magvar       = atof(stringlist[10].data);
    gprmc.magvardir    = (char)stringlist[11].data[0];

//    printf("GPRMC|%f|%c|%s|%.6f|%.6f|%.1f|%.1f|%lu|%.1f|%c\n",
//            gprmc.fixedtime,
//            gprmc.warncode,
//            gprmc.warn,
//            gprmc.latitude,
//            gprmc.longitude,
//            gprmc.speed,
//            gprmc.course,
//            gprmc.date,
//            gprmc.magvar,
//            gprmc.magvardir
//    );
  }

  // finalize vars
  found_start_cmd = FALSE;
  found_end_cmd = FALSE;
  count_stringlist = 0;
  count_pointers_chars = 0;
  p_init  = NULL;
  p_final = NULL;
  //nmea_cmd_type = NMEA_NA;

  return nmea_cmd_type;
}
//-----------------------------------------------------------------------------------------

void cleanup(int sig)
{
  exec_end = TRUE;
  return;
}
//-----------------------------------------------------------------------------------------

int main(void /*int argc,char *argv[]*/)
{
  //---------------------------------------------------------------------------------------
  // variables for serial port communication
  //---------------------------------------------------------------------------------------
  int fd,res;
  struct termios oldtio,newtio;
  //---------------------------------------------------------------------------------------
  // aux variables for serial data parsing and grouping
  //---------------------------------------------------------------------------------------
  short int flag_start_cmd, flag_end_cmd;
  int buffer_counter, total_counter;
  char temp_buf[2], buffer[1025];
  char *ptr_endcmd1, *ptr_endcmd2;
  //---------------------------------------------------------------------------------------
  // NMEA command type
  //---------------------------------------------------------------------------------------
  int nmea_cmd_type;
  //---------------------------------------------------------------------------------------

  signal(SIGTERM, cleanup);
  signal(SIGINT, cleanup);
  signal(SIGKILL, cleanup);

  //---------------------------------------------------------------------------------------
  // save current port settings
  tcgetattr(fd,&oldtio);
  //---------------------------------------------------------------------------------------
  // open serial port
  fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
  if(fd<0)
  {
    printf("Error: open serial port\r\n");
    perror(MODEMDEVICE);
    exit(-1);
  }
  //---------------------------------------------------------------------------------------
  // setting serial port configurations
  //bzero(&newtio, sizeof(newtio));
  memset(&newtio, 0, sizeof(newtio));
  newtio.c_cflag = BAUDRATE1 | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;
  newtio.c_lflag = 0;          // set input mode (non-canonical, no echo,...)
  newtio.c_cc[VTIME]    = 0;   // inter-character timer unused
  newtio.c_cc[VMIN]     = 1;   // blocking read until x chars received
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&newtio);
  //---------------------------------------------------------------------------------------
  //write(fd, "Starting GPS Parser!", 20);
  //---------------------------------------------------------------------------------------


  //---------------------------------------------------------------------------------------
  // MAIN LOOP - fetch serial port data and parse gps commands
  //---------------------------------------------------------------------------------------
  buffer_counter = 0;
  total_counter  = 0;
  flag_start_cmd = FALSE;
  flag_end_cmd   = FALSE;
  //---------------------------------------------------------------------------------------
  while(exec_end==FALSE)
  {
    // loop for input
    res = read(fd,temp_buf,1);   // returns after 1 chars have been input (newtio.c_cc[VMIN]=1)
    temp_buf[1]=0;

    // check for errors - abort when receiving code 0x00
    if(temp_buf[0]=='z')
    {
      exec_end=TRUE;
      break;
    }

//printf("[%c]",temp_buf[0]);
//printf("[%x]",temp_buf[0]);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // verify gps command starting char '$'
    if(temp_buf[0]=='$')
    {
//printf("flag_start_cmd\r\n");

      flag_start_cmd = TRUE;
      buffer_counter = 0;
    }

    // if gps command isn't started loop to fetch new char
    if(flag_start_cmd == FALSE)
    {
//printf("skipping_char\r\n");

      continue;
    }

    // verify if gps command is completed ($......\n\n or \r\n)
    // (this point flag_start_cmd is always TRUE)
    // (just need to test if flag_end_cmd is FALSE)
    if(flag_end_cmd == FALSE)
    {
      ptr_endcmd1=strstr(buffer,"\r");
      ptr_endcmd2=strstr(buffer,"\n");

      if(ptr_endcmd1!=NULL || ptr_endcmd2!=NULL)
      {
//printf("found_complete_command\r\n");

        flag_end_cmd = TRUE;
      }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // copy data into main buffer if gps as started but not ended
    if(flag_start_cmd == TRUE && flag_end_cmd == FALSE)
    {
//printf("copy to main buffer\r\n");

      buffer[buffer_counter] = temp_buf[0];
      buffer[buffer_counter+1]='\0';
      buffer_counter++;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // verify if gps command is completed ($......\n\n or \r\n) -> Parse GPS command + reset flags and counters
    if(flag_start_cmd == TRUE && flag_end_cmd == TRUE)
    {
//printf("before_terminated_cmd\r\n");
//printf("%s\r\n",buffer);

      // process buffer and parse command
      nmea_cmd_type = parsedata(buffer);

      buffer[0]=0;
      buffer_counter   = 0;
      flag_start_cmd   = FALSE;
      flag_end_cmd     = FALSE;

      total_counter++;
//printf("terminated_cmd\r\n\r\n");

      //###################################################################################
      // CALL YOUR FUNCTIONS HERE:
      // functions that will use GPS data for calculus, database insert, binary file export,
      //###################################################################################
      // ExportDataToFile(nmea_cmd_type); // not implemented, example only!
      //###################################################################################

      if (gprmc.latitude != 0 && gprmc.longitude != 0) {
        int interval = 60;
        if ( t+interval < time(NULL) ) {
          snprintf(wgetbuf, sizeof(wgetbuf),
//          "wget -q -O - 'http://narodmon.ru/post.php?ID=10FEED84D813&name=FlyGPS-003&lat=%.6f&lng=%.6f&10FEED84D81301=%.0f&10FEED84D81302=%.1f&10FEED84D81303=%.0f' && echo ' - server reply' ",
//          "httping -c 1 -t 5 -g 'http://narodmon.ru/trk/?imei=353778040312393&lat=%.6f&lon=%.6f&ele=%.0f&speed=%.1f&course=%.0f'",
//          "httping -c 1 -t 5 -g 'http://trackme.org.ua/gps/flygps?ver=1&id=flygps001&dat=%lu&tim=%.0f&lat=%.6f&lng=%.6f&alt=%.0f&spd=%.1f&crs=%.0f&sat=%d'",
            "httping -c 1 -t 5 -g 'http://narodmon.ru/post.php?ID=10FEED84D813&name=FlyGPS-003&lat=%.6f&lng=%.6f&10FEED84D81301=%.0f&10FEED84D81302=%.1f&10FEED84D81303=%.0f&10FEED84D81304=%d'",
            gprmc.latitude,
            gprmc.longitude,
            gpgga.altitude,
            gprmc.speed,
            gprmc.course,
            gpgga.satellites
            );
          system(wgetbuf);
          t = time(NULL);
        }
      }

    }

// for limited test only - debug
/* if(total_counter == 100) break; */

  }// end while - main loop
  //---------------------------------------------------------------------------------------

  //---------------------------------------------------------------------------------------
  // close serial port
  close(fd);
  //---------------------------------------------------------------------------------------

  //---------------------------------------------------------------------------------------
  // restore serial port old configurations
  tcsetattr(fd,TCSANOW,&oldtio);
  //---------------------------------------------------------------------------------------

  return 0;
}
//-----------------------------------------------------------------------------------------
