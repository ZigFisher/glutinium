/* rous-mola-bot: If someone says `Rous' or `Rosa' (like that or in capital
 *         letters) send a message praising Rosa
 * message.c: Parse the Telegram API messages
 * Copyright (C) 2015  Jaime Mosquera  _Rosa Veiga_
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

#include <string.h>
#include <stdlib.h>

/* Search `Rous' or `Rosa' in  full_message */
/* If  full_message  is NULL, return 0 */

char search_mention(char* full_message)
{
    size_t i = 0, searchable_length;
    char aux;

    if (full_message == NULL)
        return 0;

    searchable_length = strlen(full_message) - 3;

    for(i = 0; i < searchable_length && full_message[i] != '\0'; i++)
    {
        aux = full_message[i + 4];
        full_message[i + 4] = '\0';
        if (!(strcmp(&full_message[i], "Rosa") && strcmp(&full_message[i], "Rous")) ||
            !(strcmp(&full_message[i], "ROSA") && strcmp(&full_message[i], "ROUS")))
        {
            full_message[i + 4] = aux;
            return 1;
        }
        full_message[i + 4] = aux;
    }

    return 0;
}

#define INT 0
#define STRING 1
#define OBJECT 2

/* Save the value in the field  field  (allocated by this function); the value
 * may be an integer, a string or an object, as read from the JSON-formatted
 * message  full_message */
/* Warning: undefined behavior if  field  and/or  value  are NULL (not if
 * *value is NULL, though) */
/* Note: if type is not in the above list, the program does search the field,
 * but doesn't read any value for it */
void json_field(char* full_message, const char* field, char type, char** value)
{
    size_t i = 0, j = 0, nest_lv = 0, len = strlen(field);

    if (full_message == NULL || strlen(full_message) <= len)
    {
        *value = NULL;
        return;
    }

    /* Go through the message searching  field */
    for (i = 0; full_message[len + i] != '\0'; i++)
        if (full_message[i] == '\\' && full_message[i + 1] != '\0')
            i++;

        else if (full_message[i] == '"' && full_message[i + 1] != ':')
        {
            i++;   /* `"' isn't useful anymore; we can advance safely */

            if (full_message[len + i] == '"')
            {
                full_message[len + i] = '\0';
                if (strcmp(&full_message[i], field) != 0)
                    full_message[len + i] = '"';
                else
                {
                    full_message[len + i] = '"';
                    if (full_message[len + i + 1] == ':')
                        break;
                }
            }
        }          /* if full_message[i-1:i] not in (':"', '":') */

    
    if (full_message[len + i] == '\0') /* If the field was not found */
        *value = NULL;

    else                               /* If it was found */
       if (type == INT)
       {
           j = 0;
           if (full_message[len + i + 2] == '-')
               j++;

           while (1)
               if ((full_message[len + i + 2 + j] >= '0') && (full_message[len + i + 2 + j] <= '9'))
                   j++;
               else
                   break;

           /* If no proper integer was found (or if it consisted of only a
            * dash), return as if nothing had been found */
           if (j == 0 || (full_message[len + i + 2] == '-' && j == 1))
           {
               *value = NULL;
               return;
           }

           *value = (char*) malloc(sizeof(char) * (j + 1));
           strncpy(*value, &full_message[len + i + 2], j);
           (*value)[j] = '\0';
       }           /* if (type == INT) */

       else if (type == STRING)
       {
           if (full_message[len + i + 2] != '"')
           {
               *value = NULL;
               return;
           }

           for (j = 0; full_message[len + i + j + 3] != '"'; j++)
               if (full_message[len + i + j + 3] == '\\')
                   j++;

           *value = (char*) malloc(sizeof(char) * (j + 1));
           strncpy(*value, &full_message[len + i + 3], j);
           (*value)[j] = '\0';
       }           /* if (type == STRING) */

       else if (type == OBJECT)
       {
           do
           {
               if (full_message[len + i + 2 + j] == '{')
                   nest_lv++;
               else if (full_message[len + i + 2 + j] == '}')
                   nest_lv--;
               j++;
           } while (nest_lv > 0);

           if (j == 0)
               *value = NULL;
           else
           {
               *value = malloc(sizeof(char) * (j + 1));
               strncpy(*value, &full_message[len + i + 2], j);
               (*value)[j] = '\0';
           }
       }           /* if (type == OBJECT) */
    /* End of else */

    return;
}
