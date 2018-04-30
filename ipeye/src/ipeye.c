#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<errno.h>


#if defined(__APPLE__) && defined(__MACH__)
#define PLATFORM_NAME 1
#include <sys/types.h>
#include <net/if_dl.h>
#include <ifaddrs.h>
#else
#define PLATFORM_NAME 2
#include <sys/ioctl.h>
#include <net/if.h>
#endif
pthread_mutex_t mutex;


struct arg_struct {
    int* arg1;
    int* arg2;
    int* arg3;
};
typedef struct thread {
    pthread_t thread_id;
    int       thread_num;
    char*     CloudServer;
    int*      CloudPort;
    char*     CloudID;
    char*     CameraIP;
    int*      CameraPort;
    char*     Login;
    char*     Password;
    char*     Uri;
    char*     Model;
    char*     Vendor;
    char*     DevKey;
    char*     Status;
} ThreadData;

int  count = 1;
int  EnableDubug = 0;
int EnableDubugVAL = 0;

char* GetCloudID(char* ConfigDir, char* APIServer, int* APIPort, int EnableMAC, char *MACString );
int GetCloudServerAndPort(char* APIServer, int* APIPort, char* CloudID, char** CloudServer, int* CloudPort, char* Vendor, char* Model);
int CloudStart(int* sock, char* server_reply, unsigned long server_reply_size, char* CameraIP, int* CameraPort);
void *CloudLoop(void *arguments);
void *ChLoop(void *arguments);
void removeChar(char *str, char garbage);
char* substring(char* str, size_t begin, size_t len);
char* GetBallancerCloudID(char* APIServer, int* APIPort);
char* GetFsCloudID(char* ConfigDir);
char *strndup(const char *__s1, size_t __n);
int countSubstr(char string[], char substring[]);
void *StatusSave(void *arguments);
char* getJsonString(void *arguments);
char * getHtmlString(void *arguments, char* Vendor, int HTTPCameraMode, char *HTTPLogoText, char *HTTPRegSite);
int ConnectToServer(char *ip_address,int port);

char JsonTable[4096*32];
char JsonComp[4096*32];
char HtmlOut[256+4096*32];
char HtmlTable[4096*32];
char *ConfigDir  = "";

#if PLATFORM_NAME == 1
int GetMac(char *macaddrstr) {
    struct ifaddrs *ifap, *ifaptr;
    unsigned char *ptr;
    if (getifaddrs(&ifap) == 0) {
        for(ifaptr = ifap; ifaptr != NULL; ifaptr = (ifaptr)->ifa_next) {
            if (!strcmp((ifaptr)->ifa_name, "en0") && (((ifaptr)->ifa_addr)->sa_family == AF_LINK)) {
                ptr = (unsigned char *)LLADDR((struct sockaddr_dl *)(ifaptr)->ifa_addr);
                sprintf(macaddrstr, "%02x:%02x:%02x:%02x:%02x:%02x", *ptr, *(ptr+1), *(ptr+2), *(ptr+3), *(ptr+4), *(ptr+5));
                break;
            }
        }
        freeifaddrs(ifap);
        return ifaptr != NULL;
    } else {
        return 0;
    }
    return 0;
}
#else
int GetMac(char *macaddrstr) {
    int fd;
    struct ifreq ifr;
    char *iface = "eth0";
    unsigned char *mac = NULL;
    memset(&ifr, 0, sizeof(ifr));
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
    if (0 == ioctl(fd, SIOCGIFHWADDR, &ifr)) {
        mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
        sprintf(macaddrstr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    close(fd);
    return mac != NULL;
}
#endif
//main
int main(int argc , char *argv[]) {
    int x;
    int APIPort                = 8111;
    char *APIServer            = "171.25.232.2";
    char *Vendor               = "cdemo";
    char *DevKey               = "secret_key_demo";
    char *Model                = "dm321";
    int Sleep                  = 0;
    int HTTPCameraMode         = 0;
    int EnableHTTP             = 1;
    int EnableMAC              = 0;
    char *MACString            = "";
    int HTTPPort               = 8282;
    int HTTPDisableAdd         = 0;
    char *HTTPLogoText         = "IPEYE";
    char *HTTPRegSite          = "https://ipeye.ru";

    char *Streams = "rtsp://admin:admin@127.0.0.1/mpeg4,rtsp://admin:admin@127.0.0.1/mpeg4cif";
    for (x=0; x<argc; x++) {
        if (strstr(argv[x], "-http_disable_add=") != NULL) {
            HTTPDisableAdd = atoi(substring(argv[x], strlen("-http_disable_add="), strlen(argv[x])-strlen("-http_disable_add=")));
        } else if (strstr(argv[x], "-enable_http=") != NULL) {
            EnableHTTP = atoi(substring(argv[x], strlen("-enable_http="), strlen(argv[x])-strlen("-enable_http=")));
        } else if (strstr(argv[x], "-enable_debug=") != NULL) {
            EnableDubug = atoi(substring(argv[x], strlen("-enable_debug="), strlen(argv[x])-strlen("-enable_debug=")));
        } else if (strstr(argv[x], "-enable_mac=") != NULL) {
            EnableMAC = atoi(substring(argv[x], strlen("-enable_mac="), strlen(argv[x])-strlen("-enable_mac=")));
        } else if (strstr(argv[x], "-http_port=") != NULL) {
            HTTPPort = atoi(substring(argv[x], strlen("-http_port="), strlen(argv[x])-strlen("-http_port=")));
        } else if (strstr(argv[x], "-sleep=") != NULL) {
            Sleep = atoi(substring(argv[x], strlen("-sleep="), strlen(argv[x])-strlen("-sleep=")));
        } else if (strstr(argv[x], "-http_reg_site=") != NULL) {
            HTTPRegSite = substring(argv[x], strlen("-http_reg_site="), strlen(argv[x])-strlen("-http_reg_site="));
            removeChar(HTTPRegSite, '"');
        } else if (strstr(argv[x], "-mac_string=") != NULL) {
            MACString = substring(argv[x], strlen("-mac_string="), strlen(argv[x])-strlen("-mac_string="));
            removeChar(MACString, '"');
        } else if (strstr(argv[x], "-http_camera_mode=") != NULL) {
            HTTPCameraMode = atoi(substring(argv[x], strlen("-http_camera_mode="), strlen(argv[x])-strlen("-http_camera_mode=")));
        } else if (strstr(argv[x], "-model=") != NULL) {
            Model = substring(argv[x], strlen("-model="), strlen(argv[x])-strlen("-model="));
            removeChar(Model, '"');
        } else if (strstr(argv[x], "-vendor=") != NULL) {
            Vendor = substring(argv[x], strlen("-vendor="), strlen(argv[x])-strlen("-vendor="));
            removeChar(Vendor, '"');
        } else if (strstr(argv[x], "-http_logo_text=") != NULL) {
            HTTPLogoText = substring(argv[x], strlen("-http_logo_text="), strlen(argv[x])-strlen("-http_logo_text="));
            removeChar(HTTPLogoText, '"');
        } else if (strstr(argv[x], "-config_dir=") != NULL) {
            ConfigDir = substring(argv[x], strlen("-config_dir="), strlen(argv[x])-strlen("-config_dir="));
            removeChar(ConfigDir, '"');
        } else if (strstr(argv[x], "-dev_key=") != NULL) {
            DevKey = substring(argv[x], strlen("-dev_key="), strlen(argv[x])-strlen("-dev_key="));
            removeChar(DevKey, '"');
        } else if (strstr(argv[x], "-streams=") != NULL) {
            Streams = substring(argv[x], strlen("-streams="), strlen(argv[x])-strlen("-streams="));
            removeChar(Streams, '"');
        } else if (x != 0) {
            printf("Main: arg not support options %s\r\n", argv[x]);
        }
    }
    if (EnableDubug == 1) {
        printf("Main: Service Started\r\n");
    }
    if (Sleep > 0) {
        if (EnableDubug == 1) {
            printf("Main: Sleep %i second\r\n", Sleep);
        }
        sleep(Sleep);
    }


    char *CloudID = GetCloudID(ConfigDir, APIServer, &APIPort,EnableMAC,MACString);
    if (EnableDubug == 1) {
        printf("Main: Service recive CloudID = %s\r\n", CloudID);
    }
    int CloudPort;
    char * CloudServer;
    GetCloudServerAndPort(APIServer, &APIPort, CloudID, &CloudServer, &CloudPort, Vendor, Model);
    if (EnableDubug == 1) {
        printf("Main: Service recive CloudServer = %s CloudPort = %d\r\n", CloudServer, CloudPort);
        printf("Main: Cloud param: APIServer = %s APIPort = %d CloudID = %s CloudServer = %s CloudPort = %d\r\n", APIServer, APIPort, CloudID, CloudServer, CloudPort);
    }
    count = countSubstr(Streams, ",")+1;
    int i = 0;
    ThreadData thread[count];
    char *ch;
    ch = strtok(Streams, ",");
    while (ch != NULL) {
        int succ_parsing = 0;
        char ip[100];
        char user[100];
        char pass[100];
        int port = 554;
        char *page = calloc(sizeof(char),200);
        if (sscanf(ch, "rtsp://%99[^:]:%99[^@]@%99[^:]:%i/%199[^\n]", user, pass, ip, &port, page) == 5) {
            succ_parsing = 1;
        } else if (sscanf(ch, "rtsp://%99[^:]:%99[^@]@%99[^/]/%199[^\n]", user, pass, ip, page) == 4) {
            succ_parsing = 1;
        } else  if (sscanf(ch, "rtsp://%99[^:]:%99[^@]@%99[^:]:%i[^\n]", user, pass, ip, &port) == 4) {
            succ_parsing = 1;
        } else if (sscanf(ch, "rtsp://%99[^:]:%i/%199[^\n]", ip, &port, page) == 3) {
            succ_parsing = 1;
        } else if (sscanf(ch, "rtsp://%99[^/]/%199[^\n]", ip, page) == 2) {
            succ_parsing = 1;
        } else if (sscanf(ch, "rtsp://%99[^:]:%i[^\n]", ip, &port) == 2) {
            succ_parsing = 1;
        } else if (sscanf(ch, "rtsp://%99[^\n]", ip) == 1) {
            succ_parsing = 1;
        }
        if (succ_parsing == 1) {
            thread[i].thread_num=i;
            thread[i].CloudServer = CloudServer;
            thread[i].CloudPort = &CloudPort;
            thread[i].CloudID = CloudID;
            thread[i].CameraIP = ip;
            thread[i].CameraPort = &port;
            thread[i].Model = Model;
            thread[i].Vendor = Vendor;
            thread[i].Login = "";
            thread[i].Password = "";
            thread[i].Uri = page;
            thread[i].DevKey = DevKey;
            thread[i].Status = "0";
            pthread_create(&(thread[i].thread_id), NULL, ChLoop, (void *)(thread+i));
            i++;
        } else {
            if (EnableDubug == 1) {
                printf("Main: Parse Url Error\r\n");
            }
        }
        ch = strtok(NULL, ",");
    }
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, StatusSave, &thread)) {
        if (EnableDubug == 1) {
            perror("Main: create thread error");
        }
        return 1;
    }
    if (EnableHTTP == 1) {
        int listenfd = 0, connfd = 0;
        struct sockaddr_in serv_addr;
        char sendBuff[100];

        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        memset(&serv_addr, '0', sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(HTTPPort);
        int enable = 1;
        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
            if (EnableDubug == 1) {
                perror("Main: http setsockopt error");
            }
        }
        if (bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            if (EnableDubug == 1) {
                perror("Main: http bind error");
            }
        }
        listen(listenfd, 2048);
        while(1) {
            connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
            if (connfd < 0) {
                if (EnableDubug == 1) {
                    perror("Main: http accept error");
                }
                sleep(1);
                continue;
            }
            recv(connfd , sendBuff , 2048, 0);
            if (strstr(sendBuff, "GET /status/json") != NULL) {
                char * String = getJsonString(&thread);
                char result[256+strlen(String)];
                snprintf(result, sizeof result,"HTTP/1.1 200 OK\r\nServer: Golang\r\nCache-Control: no-cache\r\nContent-Type: application/json; charset=utf-8\r\nContent-Length: %lu\r\nConnection: Closed\r\n\r\n%s", (unsigned long)strlen(String), String);
                write(connfd,result, strlen(result));
            } else if (HTTPDisableAdd != 1) {
                char * String = getHtmlString(&thread, Vendor, HTTPCameraMode, HTTPLogoText, HTTPRegSite);
                char result[256+strlen(String)];
                snprintf(result, sizeof result,"HTTP/1.1 200 OK\r\nServer: Golang\r\nCache-Control: no-cache\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: %lu\r\nConnection: Closed\r\n\r\n%s", (unsigned long)strlen(String), String);
                write(connfd,result, strlen(result));
            }
            close(connfd);
        }
    } else {
        while (1) {
            sleep(5);
        }
    }
    if (EnableDubug == 1) {
        printf("Main: exit!\n");
    }
    return 0;
}
int ConnectToServer(char *ip_address,int port) {
    struct sockaddr_in sin;
    int s;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(ip_address);
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) <0) {
        perror("ConnectToServer: socket");
        return -1;
    }
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) <0) {
        perror("ConnectToServer: connect");
        close(s);
        return -1;
    }
    return s;
}
void *StatusSave(void *arguments) {
    char patch[256];
    snprintf(patch, sizeof patch, "%sstatus.cloud",  ConfigDir);
    while (1) {
        char * String = getJsonString(arguments);
        if (strcmp(String, JsonComp) == 0) {
            //printf("Entered strings are equal.\n");
        } else {
            FILE *fp;
            fp = fopen(patch, "w+");
            if (fp == NULL) {
                continue;
            }
            fputs(String, fp);
            fclose(fp);
        }
        strcpy(JsonComp, String);
        sleep(5);
    }
}

char * getJsonString(void *arguments) {
    ThreadData *thread  = (ThreadData*)arguments;
    int i;
    for (i = 0; i < count; i++) {
        if (i == 0) {
            snprintf(JsonTable, sizeof JsonTable,"{\"chanels\": [\r\n{\"chanel\":\"%i\",\"real_chanel\":\"/%s\",\"cloud_id\":\"%s/%i\",\"camera_login\":\"%s\",\"camera_password\":\"%s\",\"cloud_server\":\"%s\",\"cloud_port\":\"%d\",\"cloud_url\":\"rtsp://%s:%d/%s/%i\",\"status\":\"%s\"},\r\n", thread[i].thread_num+1,thread[i].Uri,thread[i].CloudID,thread[i].thread_num+1,thread[i].Login,thread[i].Password,thread[i].CloudServer, *thread[i].CloudPort, thread[i].CloudServer,*thread[i].CloudPort,thread[i].CloudID,thread[i].thread_num+1,thread[i].Status);
        } else if (i == count-1) {
            snprintf(JsonTable, sizeof JsonTable,"%s{\"chanel\":\"%i\",\"real_chanel\":\"/%s\",\"cloud_id\":\"%s/%i\",\"camera_login\":\"%s\",\"camera_password\":\"%s\",\"cloud_server\":\"%s\",\"cloud_port\":\"%d\",\"cloud_url\":\"rtsp://%s:%d/%s/%i\",\"status\":\"%s\"}\r\n]}", JsonTable, thread[i].thread_num+1,thread[i].Uri,thread[i].CloudID,thread[i].thread_num+1,thread[i].Login,thread[i].Password,thread[i].CloudServer, *thread[i].CloudPort, thread[i].CloudServer,*thread[i].CloudPort,thread[i].CloudID,thread[i].thread_num+1,thread[i].Status);
        } else {
            snprintf(JsonTable, sizeof JsonTable,"%s{\"chanel\":\"%i\",\"real_chanel\":\"/%s\",\"cloud_id\":\"%s/%i\",\"camera_login\":\"%s\",\"camera_password\":\"%s\",\"cloud_server\":\"%s\",\"cloud_port\":\"%d\",\"cloud_url\":\"rtsp://%s:%d/%s/%i\",\"status\":\"%s\"},\r\n", JsonTable, thread[i].thread_num+1,thread[i].Uri,thread[i].CloudID,thread[i].thread_num+1,thread[i].Login,thread[i].Password,thread[i].CloudServer, *thread[i].CloudPort, thread[i].CloudServer,*thread[i].CloudPort,thread[i].CloudID,thread[i].thread_num+1,thread[i].Status);
        }
    }
    return JsonTable;
}
char * getHtmlString(void *arguments, char* Vendor, int HTTPCameraMode, char *HTTPLogoText, char *HTTPRegSite) {
    ThreadData *thread  = (ThreadData*)arguments;
    char * String = getJsonString(arguments);
    if (HTTPCameraMode == 1) {
        snprintf(HtmlOut, sizeof HtmlOut,"<!DOCTYPE html>\r\n\
<html lang=\"en\">\r\n\
<head>\r\n\
  <meta charset=\"utf-8\">\r\n\
  <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n\
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n\
  <meta name=\"description\" content=\"\">\r\n\
  <meta name=\"author\" content=\"\">\r\n\
  <title>Cloud IP Camera %s</title>\r\n\
  <script src=\"https://code.jquery.com/jquery-1.12.4.min.js\" integrity=\"sha256-ZosEbRLbNQzLpnKIkEdrPv7lOy9C27hHQ+Xp8a4MxAQ=\" crossorigin=\"anonymous\"></script>\r\n\
  <link href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\">\r\n\
</head>\r\n\
<style>\r\n\
body {\
padding-top: 20px;\
padding-bottom: 20px;\
}\
.header,\
.marketing,\
.footer {\
padding-right: 15px;\
padding-left: 15px;\
}\
.header {\
padding-bottom: 20px;\
border-bottom: 1px solid #e5e5e5;\
}\
.header h3 {\
margin-top: 0;\
margin-bottom: 0;\
line-height: 40px;\
}\
.footer {\
padding-top: 19px;\
color: #777;\
border-top: 1px solid #e5e5e5;\
}\
@media (min-width: 768px) {\
.container {\
  max-width: 730px;\
}\
}\
.container-narrow > hr {\
margin: 30px 0;\
}\
.jumbotron {\
border-bottom: 1px solid #e5e5e5;\
}\
.jumbotron .btn {\
padding: 14px 24px;\
font-size: 21px;\
}\
.marketing {\
margin: 40px 0;\
}\
.marketing p + h4 {\
margin-top: 28px;\
}\
@media screen and (min-width: 768px) {\
.header,\
.marketing,\
.footer {\
  padding-right: 0;\
  padding-left: 0;\
}\
.header {\
  margin-bottom: 30px;\
}\
.jumbotron {\
  border-bottom: 0;\
}\
}\
</style>\
<script>\
function add_cloud() {\r\n\
    $(\"#result\").show();\r\n\
    $(\"#result\").empty();\r\n\
    $('#addbutton').attr(\"disabled\", 'true');\r\n\
    var jdata = %s;\r\n\
    var dataToSend = {\r\n\
        'action':'addcamera',\r\n\
        'login':$(\"#login\").val(),\r\n\
        'password':$(\"#password\").val(),\r\n\
        'login_camera':$(\"#clogin\").val(),\r\n\
        'password_camera':$(\"#cpassword\").val(),\r\n\
        'data': JSON.stringify(jdata)\r\n\
    };\
    $.ajax({\r\n\
      type: 'POST',\
      url: '%s',\r\n\
      data: dataToSend,\r\n\
      success: function(data) {\r\n\
        rjdata = JSON.parse(data)\r\n\
        if (rjdata.status == 1) {\r\n\
          $(\"#result\").html(\"<font color='#00cc66'><b>\"+rjdata.message+\"</b></font>\").hide(10000);\r\n\
        }else{\r\n\
          $(\"#result\").html(\"<font color='#ff0066'><b>Ошибка Добавления</b></font>:\" + rjdata.message).hide(5000);\r\n\
        }\r\n\
      },\r\n\
      error: function(data) {\r\n\
        $(\"#result\").html(\"<font color='ff0066'><b>Ошибка Добавления</b></font>:\" + data).hide(5000);\r\n\
      }\r\n\
  });\r\n\
  $(\"#addbutton\").attr(\"disabled\", false);\r\n\
}\r\n\
</script>\
<body>\
  <div class=\"container\">\
    <div class=\"header clearfix\">\
      <h3 class=\"text-muted\">Cloud Camera %s</h3>\
    </div>\
    <div class=\"jumbotron\">\
       <h2>Добавлениe камеры в облако</h2>\
       <form>\
        <div class=\"form-group\">\
          <label for=\"login\">Логин от облака</label>\
          <input type=\"text\" class=\"form-control\" id=\"login\" aria-describedby=\"emailHelp\" placeholder=\"Логин от облака\">\
          <small id=\"loginHelp\" class=\"form-text text-muted\">Для регистрации в облаке перейдите по ссылке <a href=\"%s\">%s</a>.</small>\
        </div>\
        <div class=\"form-group\">\
          <label for=\"password\">Пароль от облака</label>\
          <input type=\"password\" class=\"form-control\" id=\"password\" placeholder=\"Пароль от облака\">\
        </div>\
        <div class=\"form-group\">\
          <label for=\"clogin\">Логин от камеры</label>\
          <input type=\"text\" class=\"form-control\" id=\"clogin\" aria-describedby=\"emailHelp\" placeholder=\"Логин от камеры\">\
        </div>\
        <div class=\"form-group\">\
          <label for=\"cpassword\">Пароль от камеры</label>\
          <input type=\"password\" class=\"form-control\" id=\"cpassword\" placeholder=\"Пароль от камеры\">\
        </div>\
        <p><button id=\"addbutton\" type=\"button\" class=\"btn btn-lg btn-success\" onclick=\"add_cloud();\">Добавить в Облако</button></p>\
      </form>\
    <div id=\"result\">\
      </div>\
    </div>\
    <footer class=\"footer\">\
      <p>&copy; 2016 <a href=\"` + *HTTPRegSite + `\">%s</a> Company, Inc.</p>\
    </footer>\
  </div>\
</body>\
</html>",HTTPLogoText,String,"http://ipeye.ru/addcamera.php",HTTPLogoText,HTTPRegSite,HTTPLogoText,HTTPLogoText);
    } else {
        HtmlTable[0] = '\0';
        int i;
        for (i = 0; i < count; i++) {
            char* status;
            if (strncmp(thread[i].Status, "1", 1) == 0) {
                status = "<td bgcolor=\"#00FFFF\"  class=\"text-center\">Wait</td>";
            } else if (strncmp(thread[i].Status, "2", 2) == 0) {
                status = "<td bgcolor=\"#00FA9A\"  class=\"text-center\">Online</td>";
            } else {
                status = "<td bgcolor=\"#FF4500\" class=\"text-center\">Offline</td>";
            }
            snprintf(HtmlTable, sizeof HtmlTable,"%s<tr><td>%s/%d</td><td>rtsp://%s:%d/%s</td>%s<td class=\"text-center\"><div id=d%d><button id=b%d type=\"button\" class=\"btn btn-primary\" onclick=add(%d)>Добавить</button></div></td></tr>", HtmlTable, thread[i].CloudID,thread[i].thread_num+1, thread[i].CameraIP, *thread[i].CameraPort, thread[i].Uri, status,thread[i].thread_num+1,thread[i].thread_num+1,thread[i].thread_num+1);
        }
        snprintf(HtmlOut, sizeof HtmlOut,"<html lang=\"en\">\r\n\
  <head>\r\n\
  <link href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\">\r\n\
  <script src=\"https://code.jquery.com/jquery-2.2.4.min.js\" integrity=\"sha256-BbhdlvQf/xTY9gja0Dq3HiwQF8LaCRTXxZKRutelT44=\"crossorigin=\"anonymous\"></script>\r\n\
  <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\" integrity=\"sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa\" crossorigin=\"anonymous\"></script>\r\n\
  <script src=\"https://cdn.rawgit.com/google/code-prettify/master/loader/run_prettify.js?skin=sunburst\"></script>\r\n\
  <meta charset=\"utf-8\">\r\n\
  <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">\r\n\
  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n\
  <meta name=\"description\" content=\"\">\r\n\
  <meta name=\"author\" content=\"\">\r\n\
  <title>Добавление потоков в облако</title>\r\n\
  <script>\r\n\
  function add(actionid){\r\n\
    $(\"#b\"+actionid).attr(\"disabled\", true);\r\n\
    jdata = %s\r\n\
    var dataToSend = {\r\n\
        'action':'add',\r\n\
        'login':$(\"#login\").val(),\r\n\
        'password':$(\"#password\").val(),\r\n\
        'data': JSON.stringify(jdata.chanels[actionid-1])\r\n\
    };\r\n\
    $.ajax({\r\n\
      type: 'POST',\r\n\
      url: '%s',\r\n\
      data: dataToSend,\r\n\
      success: function(data) {\r\n\
        jdata = JSON.parse(data)\r\n\
        if (jdata.status == 1) {\r\n\
          $(\"#d\"+actionid).html(jdata.message)\r\n\
        }else{\r\n\
          $(\"#b\"+actionid).attr(\"disabled\", false);\r\n\
          alert(\"Ошибка Добавления \" + jdata.message)\r\n\
        }\r\n\
      },\r\n\
      error: function(data) {\r\n\
        $(\"#b\"+actionid).attr(\"disabled\", false);\r\n\
        alert(\"Ошибка Добавления \" + data)\r\n\
      }\r\n\
    });\r\n\
\r\n\
  }\r\n\
  </script>\r\n\
  <div align=center>\r\n\
  <h2>Добавление потоков в облако</h2>\r\n\
  <form class=\"form-inline\">\r\n\
    <div class=\"form-group\">\r\n\
      <label for=\"login\">Login</label>\r\n\
      <input id=login type=\"text\" class=\"form-control\" id=\"login\" placeholder=\"Login\">\r\n\
    </div>\r\n\
    <div class=\"form-group\">\r\n\
      <label for=\"password\">Password</label>\r\n\
      <input id=password type=\"password\" class=\"form-control\" id=\"password\" placeholder=\"Password\">\r\n\
    </div>\r\n\
  </form>\r\n\
  <table  class=\"table table-striped\" style=\"width: 800\">\r\n\
  <tr><th>CloudID</th><th>Поток</th><th class=\"text-center\">Статус</th><th class=\"text-center\">Облако</th></tr>\r\n\
  %s</table></div>", String, "http://ipeye.ru/addcamera.php", HtmlTable);
    }
    return HtmlOut;
}
void *ChLoop(void *arguments) {
    ThreadData *readParams  = (ThreadData*)arguments;
    int sock;
    char server_reply[256];
    while (1) {
        readParams->Status = "0";
        sock = ConnectToServer(readParams->CloudServer, *readParams->CloudPort);
        if (sock == -1)  {
            if (EnableDubug == 1) {
                printf("ChLoop: ConnectToServer not create socket\n");
            }
            sleep(1);
            continue;
        }
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
            if (EnableDubug == 1) {
                perror("ChLoop: setsockopt failed\n");
            }
        }
        if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0) {
            if (EnableDubug == 1) {
                perror("ChLoop: setsockopt failed\n");
            }
        }
        char result[512];
        snprintf(result, sizeof result, "REGISTER={\"cloudid\":\"%s/%d\",\"login\":\"%s\",\"password\":\"%s\",\"uri\":\"/%s\",\"model\":\"%s\",\"vendor\":\"%s\",\"devkey\":\"%s\"}", readParams->CloudID,readParams->thread_num+1, readParams->Login, readParams->Password, readParams->Uri, readParams->Model, readParams->Vendor, readParams->DevKey);
        if (EnableDubug == 1) {
            printf("ChLoop: Send server string =  %s\n", result);
        }
        if( send(sock , result , strlen(result) , 0) < 0) {
            if (EnableDubug == 1) {
                perror("ChLoop: Send register failed");
            }
            close(sock);
            sleep(1);
            continue;
        }
        timeout.tv_sec = 120;
        timeout.tv_usec = 0;
        if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
            if (EnableDubug == 1) {
                perror("ChLoop: setsockopt failed\n");
            }
        }
        if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0) {
            if (EnableDubug == 1) {
                perror("ChLoop: setsockopt failed\n");
            }
        }
        readParams->Status = "1";
        while (1) {
            int rs = recv(sock , server_reply , 256, 0);
            if(rs <= 0)  {
                if (EnableDubug == 1) {
                    perror("ChLoop: Server close socket");
                }
                break;
            }
            if (server_reply[0] ==  '\r' && server_reply[1] ==  '\n' &&  server_reply[2] ==  '\r' && server_reply[3] ==  '\n') {
                if (EnableDubug == 1) {
                    printf("ChLoop: Recive ACK from Server chanel = %i uri = /%s\r\n", readParams->thread_num+1, readParams->Uri);
                }
                if( send(sock , "ok" , 2 , 0) < 0) {
                    if (EnableDubug == 1) {
                        perror("ChLoop: ACK responce write failed");
                    }
                    break;
                }
            } else if (server_reply[0] ==  'O' && server_reply[1] ==  'P' &&  server_reply[2] ==  'T' && server_reply[3] ==  'I') {
                readParams->Status = "2";
                if (EnableDubug == 1) {
                    puts("ChLoop: Recive hello from Server");
                }
                CloudStart(&sock, server_reply, rs, readParams->CameraIP, readParams->CameraPort);
                break;
            } else {
                if (EnableDubug == 1) {
                    puts("ChLoop: bad hello");
                    puts(server_reply);
                }
            }
            sleep(1);
        }
        close(sock);
        sleep(1);
    }
    if (EnableDubug == 1) {
        printf("ChLoop: exit!\n");
    }
    return NULL;
}
char* GetCloudID(char* ConfigDir, char* APIServer, int* APIPort, int EnableMAC, char *MACString ) {
    char* CloudID = "";
    if (EnableMAC == 1) {
        if (strlen(MACString) == 17) {
            CloudID = MACString;
        } else {
            char macaddrstr[17];
            if (GetMac(macaddrstr)) {
                char *buff = malloc(17);
                strncat (buff,macaddrstr, 17);
                CloudID = buff;
            } else {
                if (EnableDubug == 1) {
                    printf("GetCloudID: Mac Address != 17 == %s\n", MACString);
                }
                if (EnableDubug == 1) {
                    printf("GetCloudID: exit!\n");
                }
                exit(EXIT_FAILURE);
            }
        }
    } else {
        CloudID = GetFsCloudID(ConfigDir);
    }
    if (strlen(CloudID) < 10 || strlen(CloudID) > 50) {
        if (EnableDubug == 1) {
            printf("GetCloudID: CloudID bad < 10 || > 50 CloudID = %s\r\n", CloudID);
        }
        CloudID = GetBallancerCloudID(APIServer, APIPort);
        FILE *fp;
        char result[256];
        snprintf(result, sizeof result, "%scloud.id",  ConfigDir);
        while (1) {
            fp = fopen(result, "w+");
            if (fp != NULL) {
                break;
            }
            if (EnableDubug == 1) {
                printf("GetCloudID: Error opening file!\n");
            }
            sleep(2);
        }
        fputs(CloudID, fp);
        fclose(fp);
    }
    return CloudID;
}
char* GetFsCloudID(char* ConfigDir) {
    char result[256];
    char buffer[36];
    snprintf(result, sizeof result, "%scloud.id",  ConfigDir);
    FILE *fp;
    int i = 0;
    while (1) {
        fp = fopen(result, "r");
        if (fp != NULL) {
            break;
        }
        if (i >= 10) {
            return "";
        }
        if (EnableDubug == 1) {
            printf("GetFsCloudID: Error opening file! wait mount 10 sec %u\n", i);
        }
        i++;
        sleep(1);
    }
    int readet = fread(buffer, 1, 36, fp);
    char *buff = malloc(readet+1);
    strncat (buff,buffer, 36);
    return buff;
}
char* GetBallancerCloudID(char* APIServer, int* APIPort) {
    int sock;
    struct sockaddr_in server;
    char server_reply[256];
    while (1) {
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)  {
            if (EnableDubug == 1) {
                perror("GetBallancerCloudID: create socket error");
            }
            continue;
        }
        struct timeval timeout;
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
            if (EnableDubug == 1) {
                perror("GetBallancerCloudID: setsockopt failed");
            }
        }
        if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0) {
            if (EnableDubug == 1) {
                perror("GetBallancerCloudID: setsockopt failed");
            }
        }
        server.sin_addr.s_addr = inet_addr(APIServer);
        server.sin_family = AF_INET;
        server.sin_port = htons(*APIPort);
        if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
            if (EnableDubug == 1) {
                perror("GetBallancerCloudID: GetCloudServerAndPort connect failed. Error");
            }
            sleep(5);
            close(sock);
            continue;
        }
        char result[512];
        snprintf(result, sizeof result, "GET /balancer/uuid HTTP/1.1\r\nHost: %s\r\nContent-Type: text/xml\r\nContent-Length: 0\r\n\r\n", APIServer);
        if( send(sock , result , strlen(result) , 0) < 0) {
            if (EnableDubug == 1) {
                perror("GetBallancerCloudID: GetCloudServerAndPort Send register failed");
            }
            sleep(5);
            continue;
        }
        int rs = recv(sock , server_reply , 256, 0);
        if (EnableDubug == 1) {
            puts(server_reply);
        }
        if (rs > 0 && strstr(server_reply, "\"code\":200") != NULL) {
            char *token;
            token = strtok( server_reply, "\r\n\r\n" );
            while( token != NULL ) {
                if (strstr(token, "message") != NULL) {
                    char *tokenr;
                    tokenr = strtok( token, "," );
                    while( tokenr != NULL ) {
                        if (strstr(tokenr, "message") != NULL) {
                            char *tokenrr;
                            tokenrr = strtok( tokenr, ":" );
                            while( tokenrr != NULL ) {
                                if (strstr(tokenrr, "message") == NULL) {
                                    removeChar(tokenrr, '"');
                                    char *str3 = malloc(strlen(tokenrr) + 1);
                                    strcpy (str3,tokenrr);
                                    return str3;
                                }
                                tokenrr = strtok( NULL, "," );
                            }
                        }
                        tokenr = strtok( NULL, "," );
                    }
                }
                token = strtok( NULL, "\r\n\r\n" );
            }
        }
        sleep(5);
    }
    return 0;
}

int GetCloudServerAndPort(char* APIServer, int* APIPort,  char* CloudID,  char** CloudServer, int* CloudPort, char* Vendor, char* Model) {
    int sock;
    struct sockaddr_in server;
    char server_reply[256];
    while (1) {
        sock = socket(AF_INET , SOCK_STREAM , 0);
        if (sock == -1)  {
            if (EnableDubug == 1) {
                printf("GetCloudServerAndPort: GetCloudServerAndPort not create socket\r\n");
            }
            continue;
        }
        struct timeval timeout;
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
            if (EnableDubug == 1) {
                perror("GetCloudServerAndPort: setsockopt failed\n");
            }
        }
        if (setsockopt (sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0) {
            if (EnableDubug == 1) {
                perror("GetCloudServerAndPort: setsockopt failed\n");
            }
        }
        server.sin_addr.s_addr = inet_addr(APIServer);
        server.sin_family = AF_INET;
        server.sin_port = htons(*APIPort);
        if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
            if (EnableDubug == 1) {
                perror("GetCloudServerAndPort: create socket error");
            }
            sleep(5);
            close(sock);
            continue;
        }
        char result[512];
        snprintf(result, sizeof result, "GET /balancer/server/%s?vendor=%s&model=%s HTTP/1.1\r\nHost: %s\r\nContent-Type: text/xml\r\nContent-Length: 0\r\n\r\n", CloudID, Vendor, Model, APIServer);
        if( send(sock , result , strlen(result) , 0) < 0) {
            if (EnableDubug == 1) {
                perror("GetCloudServerAndPort: send register error");
            }
            sleep(5);
            continue;
        }
        int rs = recv(sock , server_reply , 256, 0);
        if (rs > 0 && strstr(server_reply, "\"code\":200") != NULL) {
            char *token;
            token = strtok( server_reply, "\r\n\r\n" );
            while( token != NULL ) {
                if (strstr(token, "message") != NULL) {
                    char *tokenr;
                    tokenr = strtok( token, "," );
                    while( tokenr != NULL ) {
                        if (strstr(tokenr, "message") != NULL) {
                            char *tokenrr;
                            tokenrr = strtok( tokenr, ":" );
                            while( tokenrr != NULL ) {
                                if (strstr(tokenrr, "|") != NULL) {
                                    removeChar(tokenrr, '"');
                                    char *tokenJsonTable;
                                    tokenJsonTable = strtok( tokenrr, "|" );
                                    while( tokenJsonTable != NULL ) {
                                        if (strstr(tokenJsonTable, ".") != NULL) {
                                            char *str3 = malloc(strlen(tokenJsonTable) + 1);
                                            strcpy (str3,tokenJsonTable);
                                            *CloudServer = str3;
                                        } else {
                                            char *str3 = malloc(strlen(tokenJsonTable) + 1);
                                            strcpy (str3,tokenJsonTable);
                                            *CloudPort   = atoi(str3);
                                        }
                                        tokenJsonTable = strtok( NULL, "|" );
                                    }
                                    return 0;
                                }
                                tokenrr = strtok( NULL, "," );
                            }
                        }
                        tokenr = strtok( NULL, "," );
                    }
                }
                token = strtok( NULL, "\r\n\r\n" );
            }
        }
        sleep(5);
    }
    return 0;
}

int CloudStart(int* sock, char* server_reply, unsigned long server_reply_size, char* CameraIP, int* CameraPort) {
    printf("now connect to camera\r\n");
    int cam_sock = ConnectToServer(CameraIP, *CameraPort);
    if (cam_sock == -1)  {
        if (EnableDubug == 1) {
            printf("CloudStart: not create socket\r\n");
        }
        return 0;
    }
    if (EnableDubug == 1) {
        puts("CloudStart: Socket created");
    }
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if (setsockopt (cam_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        if (EnableDubug == 1) {
            perror("CloudStart: setsockopt failed\n");
        }
    }
    if (setsockopt (cam_sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,sizeof(timeout)) < 0) {
        if (EnableDubug == 1) {
            perror("CloudStart: setsockopt failed\n");
        }
    }
    if( send(cam_sock , server_reply , server_reply_size , 0) <= 0) {
        if (EnableDubug == 1) {
            perror("CloudStart: Camera hello write failed");
        }
        return 1;
    }
    int ch = 1;
    int ch2 = 0;
    struct arg_struct readParams;
    readParams.arg1 = &cam_sock;
    readParams.arg2 = sock;
    readParams.arg3 = &ch;
    pthread_t thread_id;
    if (pthread_create(&thread_id, NULL, CloudLoop, &readParams)) {
        if (EnableDubug == 1) {
            perror("CloudStart: not create thread");
        }
        return 1;
    }
    struct arg_struct readParams2;
    readParams2.arg1 = sock;
    readParams2.arg2 = &cam_sock;
    readParams2.arg3 = &ch2;
    CloudLoop( &readParams2);
    close(cam_sock);
    close(*sock);
    sleep(5);
    if (EnableDubug == 1) {
        puts("CloudStart: Server thread exit");
    }
    return 0;
}
/*
socket read and write loop
*/
void *CloudLoop(void *arguments) {
    pthread_detach(pthread_self());
    struct arg_struct *args = arguments;
    if (*args->arg3 == 1) {
        if (EnableDubug == 1) {
            puts("CloudLoop: Camera thread start");
        }
    } else {
        if (EnableDubug == 1) {
            puts("CloudLoop: Server thread start");
        }
    }
    char server_reply[8096];
    while (1) {
        int rs = recv(*args->arg1 , server_reply , 8096, 0);
        if( rs <= 0)  {
            if (EnableDubug == 1) {
                perror("CloudLoop: Close Read Loop");
            }
            break;
        }
        if( send(*args->arg2 , server_reply , rs , 0) <= 0) {
            if (EnableDubug == 1) {
                perror("CloudLoop: Close write Loop");
            }
            break;
        }
    }
    if (*args->arg3 == 1) {
        if (EnableDubug == 1) {
            puts("CloudLoop: Camera thread exit");
        }
        pthread_exit(NULL);
    }
    return NULL;
}
void removeChar(char *str, char garbage) {
    char *src, *dst;
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src;
        if (*dst != garbage) dst++;
    }
    *dst = '\0';
}

char* substring(char* str, size_t begin, size_t len) {
    if (str == 0 || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin+len)) {
        return 0;
    }
    return strndup(str + begin, len);
}
int countSubstr(char string[], char substring[]) {
    int subcount = 0;
    size_t sub_len = strlen(substring);
    if (!sub_len) return 0;
    size_t i;
    for (i = 0; string[i];) {
        size_t j = 0;
        size_t count = 0;
        while (string[i] && string[j] && string[i] == substring[j]) {
            count++;
            i++;
            j++;
        }
        if (count == sub_len) {
            subcount++;
            count = 0;
        } else {
            i = i - j + 1; /* no match, so reset to the next index in 'string' */
        }
    }
    return subcount;
}
