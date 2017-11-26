/* rous-mola-bot: If someone says `Rous' or `Rosa' (like that or in capital
 *         letters) send a message praising Rosa
 * Copyright (C) 2015  _Jaime Mosquera_  Rosa Veiga
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "message.h"
#include "bot_key.h"

/* A constant called  BOT_KEY  (with the key of the bot) is needed, and should
 * be defined in  bot_key.h  (which should be added to  .gitignore  to
 * guarantee privacy) */
#define BOT_ADDRESS "https://api.telegram.org/bot"BOT_KEY

/* The space allocated for the whole address of the read and write commands,
 * including the query commands
 * Both were around ~110 bytes when I tested them; if you suspect a possible
 * buffer overflow, change them */
#define GET_METHOD_READ_SIZE 128
#define GET_METHOD_WRITE_SIZE 128

/* An object of this type is sent to  read_message  from cURL to get updates
 * from the Telegram server */
typedef struct {
    size_t size;
    char* text;
} json_message;


/* Through this function, we store every update to an ever-growing string
 * until we have the whole update and can handle it */
size_t read_message(void* raw_message, size_t size, size_t nmemb, void* dest)
{
    size_t real_size = size * nmemb;
    json_message* dest_message = (json_message*) dest;

    /* We allocate space for the new part of the update */
    dest_message->text = realloc(dest_message->text,
                                 dest_message->size + real_size + 1);
    if (dest_message->text == NULL)
    {
        fprintf(stderr, "Error: Rosa no mola, o no hay memoria suficiente\n");
        return 0;
    }

    /* We copy the new packet into the update string */
    memcpy(&(dest_message->text[dest_message->size]), raw_message, real_size);
    dest_message->size += real_size;
    dest_message->text[dest_message->size] = '\0';

    return real_size;
}


/* This acts as a non-working  read_message,  used to send the message through
 * the HTTP GET method in a query string */
size_t dummy_function(void* no_use, size_t size, size_t nmemb, void* null)
{
    return size * nmemb;
}


int main(int argc, char** argv)
{
    CURL* read_handle = curl_easy_init();
    CURL* write_handle = curl_easy_init();
    CURLcode result;

    json_message in_message = {0, NULL};

    char* GET_write;         /* The query string used to send the message */
    char* GET_read;          /* The query string used to read the update*/

    /* Used to get  chat_id */
    char* chat_obj;
    /* ID of the chat the message is sent to */
    char* chat_id;
    /* Offset of the last update (by default, "0" to get the oldest that is
     * still stored) */
    char* message_id = NULL;
    char* message_text;

    int i;         /* Used to go through  message_id  and add one to it */


    curl_global_init(CURL_GLOBAL_ALL);

    GET_read =  (char*) malloc(GET_METHOD_READ_SIZE * sizeof(char));
    GET_write = (char*) malloc(GET_METHOD_WRITE_SIZE * sizeof(char));

    while (1)
    {
        in_message.text = (char*) malloc(1 * sizeof(char));
        in_message.text[0] = '\0';

        /* If no  message_id  is received, or if the program starts, set to
         * "0", which gets all the updates */
        if (message_id == NULL)
        {
            message_id = (char*) malloc(sizeof(char) * 2);
            sprintf(message_id, "0");
        }

        sprintf(GET_read, "%s/getUpdates?limit=1&offset=%s",
                          BOT_ADDRESS, message_id);
        curl_easy_setopt(read_handle, CURLOPT_URL, GET_read);
        curl_easy_setopt(read_handle, CURLOPT_WRITEFUNCTION, read_message);
        curl_easy_setopt(read_handle, CURLOPT_WRITEDATA, &in_message);
        result = curl_easy_perform(read_handle);


        free(message_id);
        /* Read some fields from the update string */
        json_field(in_message.text, "text", STRING, &message_text);
        json_field(in_message.text, "update_id", INT, &message_id);
        json_field(in_message.text, "chat", OBJECT, &chat_obj);
        json_field(chat_obj, "id", INT, &chat_id);

        /* If  message_id  is available,  add 1 to it */
        if (message_id != NULL)
            for (i = strlen(message_id) - 1; ++(message_id[i]) > '9'; i--)
                message_id[i] = '0';

        /* If "Rosa" or "Rous" is found, send the answer */
        if (search_mention(message_text))
        {
            GET_write[0] = '\0';
            sprintf(GET_write, "%s/sendMessage?chat_id=%s&text=Rous mola",
                               BOT_ADDRESS, chat_id);

            curl_easy_setopt(write_handle, CURLOPT_URL, GET_write);
            curl_easy_setopt(write_handle, CURLOPT_WRITEFUNCTION,
                             dummy_function);
            curl_easy_setopt(write_handle, CURLOPT_READDATA, NULL);

            result = curl_easy_perform(write_handle);

            if (result != CURLE_OK)
                fprintf(stderr, "Error: Ocurrió un error con cURL\n");
        }

        /* Free everything (except  message_id,  which will be used in the
         * next update, and  GET_read  and  GET_write,  which is better to
         * keep instead of freeing and allocating it) */
        in_message.size = 0;
        free(in_message.text);
        free(chat_obj);
        free(chat_id);
        free(message_text);
    }

    return 0;
}
