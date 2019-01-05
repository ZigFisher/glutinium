#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <telebot.h>

#define SIZE_OF_ARRAY(array) (sizeof(array)/sizeof(array[0]))
#define TOKEN_SIZE 128
#define FILENAME_TOKEN ".token"
#define FILENAME_USERID ".userid"

struct {       
    char *token;
    long long user_id;
    char *imgfile;
    char *videofile;
    char *docfile;
    char *audiofile;
    char *comment;
    char *path;
    char *text;
    char *proxy_addr;
    int isRemove;
    int isWeakConfig;
    int useFileConfig;
} sstgbconf;


void printHelp(void) {
printf(
		"-----------------------------------------------------------------------------\n"
		"| sstgb: simply sender telegram bot                                      v1.2|\n"
		"-----------------------------------------------------------------------------\n"
		"\n"
		"This telegram bot sends text and pictures according to command line paramaters\n"
		"(c) 2018-2019 Denis G Dugushkin, dentall@mail.ru, phone +7-952-242-42-88\n"
		"\n"
		"Usage: sstgb <command> <parameter> [options]\n"
		"\n"
		"Commands:\n"
		"--sendtext  <text>       Send text to user ID\n"
		"--sendpic   <filename>   Send picture to user ID\n"
		"--senddoc   <filename>   Send document to user ID\n"
		"--sendvideo <filename>   Send video to user ID\n"
		"--sendaudio <filename>   Send audio to user ID\n"
		"\n"
		"Options:\n"
		"--token <token>          Bot token\n"
		"--userid <id>            User ID\n"
		"--fileconfigs            Read bot token and userid from files .token and .userid accordingly\n"
		"--path                   Working path for --fileconfigs option (last slash required!)\n"
                "--proxy                  Use libcurl proxy. Examples: socks5://addr.org:8564 or http://addr.org:8564\n"
		"--comment                Comment for picture/audio/video\n"
		"--remove                 Remove(!) file after use for --sendpic, --sendvideo, --sendaudio, --senddoc\n"
		"--weakconfig             Simplified command line parameters pre-check for debug\n"
		"\n"
		"To use --fileconfigs there are should be two files:\n"
		"1) .token  - text file where first string is telegram bot token\n"
		"2) .userid - text file where first string is user ID\n"
		"\n"
);

}

void initConfig(void) {
	memset(&sstgbconf, 0, sizeof(sstgbconf));
}

void printConfig(void) {
	printf(
			"Current config:\n"
			"Bot token...............%s\n"
			"User ID.................%lld\n"
			"Image filename..........%s\n"
			"Video filename..........%s\n"
			"Audio filename..........%s\n"
			"Doc filename............%s\n"
			"Comment.................%s\n"
			"Text to send............%s\n"
			"Remove file after use...%d\n"
			"Use file config.........%d\n"
			"Working path............%s\n"
			"Use weak config.........%d\n"
			"Proxy...................%s\n"
			"\n",
			sstgbconf.token,
			sstgbconf.user_id,
			sstgbconf.imgfile,
			sstgbconf.videofile,
			sstgbconf.audiofile,
			sstgbconf.docfile,
			sstgbconf.comment,
			sstgbconf.text,
			sstgbconf.isRemove,
			sstgbconf.useFileConfig,
			sstgbconf.path,
			sstgbconf.isWeakConfig,
			sstgbconf.proxy_addr
);
}

int readFileConfig(void) {

	char *tokenfilename, *useridfilename;

	if (sstgbconf.path == NULL) {
		tokenfilename = strdup(FILENAME_TOKEN);
		useridfilename = strdup(FILENAME_USERID);

	} else {
		tokenfilename = (char*)calloc(strlen(sstgbconf.path) + sizeof(FILENAME_TOKEN) + 1, sizeof(char));
		strcpy(tokenfilename, sstgbconf.path);
		strcat(tokenfilename, FILENAME_TOKEN);

		useridfilename = (char*)calloc(strlen(sstgbconf.path) + sizeof(FILENAME_USERID) + 1, sizeof(char));
		strcpy(useridfilename, sstgbconf.path);
		strcat(useridfilename, FILENAME_USERID);
	}

	FILE *fp = fopen(tokenfilename, "r");
	if (fp == NULL) {
		printf("Failed to open .token file\n");
		return -1;
	}

	sstgbconf.token = (char*)calloc(TOKEN_SIZE, sizeof(char));
	if (fscanf(fp, "%s", sstgbconf.token) == 0) {
		printf("Reading token failed\n");
		fclose(fp);
		return -1;
	}
	fclose(fp);

	fp = fopen(useridfilename, "r");
	if (fp == NULL) {
		printf("Failed to open .userid file\n");
	    return -1;
	}

	if (fscanf(fp, "%lld", &sstgbconf.user_id) == 0) {
		printf("Reading user ID failed\n");
		fclose(fp);
		return -1;
	}
	
	free(tokenfilename);
	free(useridfilename);
	fclose(fp);
	return 0;
}

int checkConfig(void) {
	if(sstgbconf.token == NULL) return -1;
	if(sstgbconf.user_id == 0) return -1;
	if((sstgbconf.imgfile == NULL) &&
			(sstgbconf.text == NULL) &&
			(sstgbconf.videofile == NULL) && 
			(sstgbconf.docfile == NULL) && 
			(sstgbconf.audiofile == NULL)) return -1;
	if(!sstgbconf.isWeakConfig && 
			(sstgbconf.imgfile == NULL) && 
			(sstgbconf.videofile == NULL) && 
			(sstgbconf.docfile == NULL) && 
			(sstgbconf.audiofile == NULL) &&
			(sstgbconf.isRemove == 1)) return -1;
	return 0;
}

int main(int argc, char *argv[])
{
	int j, more;
	
	initConfig();
	
	for (j = 1; j < argc; j++) {
		more = ((j + 1) < argc);
		if (!strcmp(argv[j],"--remove")) {
			sstgbconf.isRemove = 1;
		}
		else if (!strcmp(argv[j],"--weakconfig")) {
					sstgbconf.isWeakConfig = 1;
		}
		else if (!strcmp(argv[j],"--sendpic") && more) {
			sstgbconf.imgfile = strdup(argv[++j]);
		}
		else if (!strcmp(argv[j],"--sendvideo") && more) {
			sstgbconf.videofile = strdup(argv[++j]);
		}
		else if (!strcmp(argv[j],"--sendtext") && more) {
			sstgbconf.text = strdup(argv[++j]);
		}
		else if (!strcmp(argv[j],"--senddoc") && more) {
					sstgbconf.docfile = strdup(argv[++j]);
		}
		else if (!strcmp(argv[j],"--sendaudio") && more) {
					sstgbconf.audiofile = strdup(argv[++j]);
		}
		else if (!strcmp(argv[j],"--fileconfigs")) {
			sstgbconf.useFileConfig = 1;
		}
		else if (!strcmp(argv[j],"--token") && more) {
			sstgbconf.token = strdup(argv[++j]);
		}
		else if (!strcmp(argv[j],"--userid") && more) {
			sstgbconf.user_id = atoll(argv[++j]);
		}
		else if (!strcmp(argv[j],"--path") && more) {
					sstgbconf.path = strdup(argv[++j]);
		}
		else if (!strcmp(argv[j],"--comment") && more) {
					sstgbconf.comment = strdup(argv[++j]);
		}
		else if (!strcmp(argv[j],"--proxy") && more) {
					sstgbconf.proxy_addr = strdup(argv[++j]);
		}
		else if (!strcmp(argv[j],"--help")) {
			printHelp();
			return 0;
		}
		else {
			printHelp();
			printConfig();
			printf("Unknown or not enough arguments for option '%s'.\n\n", argv[j]);
			return -1;
		}
	}

	if (sstgbconf.useFileConfig) {
		if (readFileConfig() != 0) {
			printf("Failed to read config files. Checking command line config...\n");
		}
	}

	if(checkConfig() != 0) {
		printHelp();
		printConfig();
		printf("Configuration error. Nothing to do.\n\n");
		return -1;
	}


    telebot_handler_t handle;
    if (telebot_create(&handle, sstgbconf.token) != TELEBOT_ERROR_NONE) {
        printf("Telebot create failed (bad token?)\n\n");
        return -1;
    }

    // Now all pre-checks are passed
    // Main work routine
    
    telebot_error_e ret;

    if (sstgbconf.proxy_addr != NULL) {
    	ret = telebot_use_proxy(handle, sstgbconf.proxy_addr);
    	if (ret != TELEBOT_ERROR_NONE) {
    		printf("Failed to init proxy: %d \n", ret);
    	}
    }

    if (sstgbconf.text != NULL) {
    	ret = telebot_send_message(handle, sstgbconf.user_id, sstgbconf.text, "", false, false, 0, "");
    	if (ret != TELEBOT_ERROR_NONE) {
    		printf("Failed to send text message: %d \n", ret);
    	}
    }
 
    if ((sstgbconf.imgfile != NULL) && !access(sstgbconf.imgfile, R_OK)) {
    	ret = telebot_send_photo(handle, sstgbconf.user_id, sstgbconf.imgfile, true, sstgbconf.comment, false, 0, NULL);
    	if (ret != TELEBOT_ERROR_NONE) {
    		printf("Failed to send picture: %d \n", ret); 
    	}
    	if (sstgbconf.isRemove) remove(sstgbconf.imgfile);
    }
    
    if ((sstgbconf.videofile != NULL) && !access(sstgbconf.videofile, R_OK)) {
    	ret = telebot_send_video(handle, sstgbconf.user_id, sstgbconf.videofile, true, 0, 0, 0, sstgbconf.comment, false, 0, NULL);
    	if (ret != TELEBOT_ERROR_NONE) {
    		printf("Failed to send video: %d \n", ret); 
    	}
    	if (sstgbconf.isRemove) remove(sstgbconf.videofile);
    }
    
    if ((sstgbconf.audiofile != NULL)  && !access(sstgbconf.audiofile, R_OK)) {
    	ret = telebot_send_audio(handle, sstgbconf.user_id, sstgbconf.audiofile, true, 0, NULL, sstgbconf.comment, false, 0, NULL);
    	if (ret != TELEBOT_ERROR_NONE) {
    		printf("Failed to send audio: %d \n", ret); 
    	}
    	if (sstgbconf.isRemove) remove(sstgbconf.audiofile);
    }

    if ((sstgbconf.docfile != NULL)  && !access(sstgbconf.docfile, R_OK)) {
    	ret = telebot_send_document(handle, sstgbconf.user_id, sstgbconf.docfile, true, false, 0, NULL);
    	if (ret != TELEBOT_ERROR_NONE) {
    		printf("Failed to send document: %d \n", ret); 
    	}
    	if (sstgbconf.isRemove) remove(sstgbconf.docfile);
    }    
    
    // Free resources
    telebot_destroy(handle);
	free(sstgbconf.token);
	free(sstgbconf.text);
	free(sstgbconf.imgfile);
	free(sstgbconf.videofile);
	free(sstgbconf.audiofile);
	free(sstgbconf.docfile);
	free(sstgbconf.path);
	free(sstgbconf.comment);
	free(sstgbconf.proxy_addr);

	
    return 0;
}
