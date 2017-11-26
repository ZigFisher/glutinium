/* rous-mola-bot: If someone says `Rous' or `Rosa' (like that or in capital
 *         letters) send a message praising Rosa
 * message.h: Parse the Telegram API messages
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

/* Search `Rous' or `Rosa' in  full_message */
/* If  full_message  is NULL, return 0 */
char search_mention(char* full_message);

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
void json_field(char* full_message, const char* field, char type,
                char** value);
