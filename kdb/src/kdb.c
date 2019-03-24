/***************************************************************************
 *
 * Copyright (c) 2005,2007    Vladislav Moskovets (webface-dev(at)vlad.org.ua)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 *************************************************************************** */

/* TODO: 
 *	* multiline import (export)
 *	* raw block partition support for storage
 *	* types
 *	*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include <error.h>
#include <stdarg.h>
#include <syslog.h> // openlog(), syslog()

// #define DEBUG
#define MAX_LINES 2048
#define MAX_LINE_SIZE 1024
#define READ_BUFFER_LEN (1024*32)
#define WRITE_BUFFER_LEN READ_BUFFER_LEN
#define HEADER_LINE "KDB\n"
#define FOOTER_LINE "KDB END\n"
#define true 1
#define false 0
#define COUNTS_NAME "kdb_lines_count"
#define NEXT_NAME "kdb_next_name"
#define ENV_STRING "%ENV"
#define WARN

#define FLOCK(f) {if(flock(fileno(f), LOCK_EX)) { perror("flock"); syslog(LOG_ERR, "flock() fail, error code %d", errno); }; };
#define FUNLOCK(f) flock(fileno(f), LOCK_UN);

#define WILDCARD_LOOP(i, iname) for( i=0; i < db_lines_count; i++ ) \
								if ( match_wildcard(iname, db_lines[i].name) )

char *str_escape(const char *source);
char *str_unescape(const char *source);
int match_wildcard(const char *pattern, const char *string);
int is_wildcarded(const char* str);

typedef struct {
	char *name;
	char *value;
} _db_record;

char db_filename[255];
FILE *db_file=NULL;
_db_record db_lines[MAX_LINES];
char db_header[MAX_LINE_SIZE];
int db_lines_count;
int quotation;
int make_local;
char local_str[]="local ";
int make_export;
int need_print_count;
int need_write;
int db_already_readed;

#ifdef DEBUG
void debug(char *format, ...){
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
}
#else
inline void _debug(char* format, ...){}
#define debug if(0)_debug
#endif

#ifdef WARN
void warn(char *format, ...){
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	vsyslog(LOG_WARNING, format, ap);
	va_end(ap);
}
#else
inline void warn(char* format, ...){}
#endif


void show_usage(char *name)
{
    printf("Usage: %s [OPTIONS] ARG [: ARG] \n", name);
	printf("where  OPTIONS:= l|q|qq|e|c\n");
    printf("       ARG := { set pattern=value |\n");
    printf("        rm pattern | del pattern |\n");
    printf("        isset key |\n");
    printf("        list [pattern] | ls [pattern] |\n");
    printf("        slist key | sls key |\n");
    printf("        klist pattern | kls pattern |\n");
    printf("        listrm key | lrm key |\n");
    printf("        listadd key_=value | ladd key_=value |\n");
    printf("        rename oldkey newkey | rn oldkey newkey |\n");
    printf("        create [filename] |\n");
    printf("        import [filename] |\n");
    printf("        edit }\n");
    return;
};

char *get_dbfilename()
{
	if(strlen(db_filename))
		return db_filename;

    if (getenv("KDB"))
        strcpy(db_filename, getenv("KDB"));
    else if(!getuid())
        strcpy(db_filename, "./kdb");
    else 
        sprintf(db_filename, "%s/.kdb", getenv("HOME"));

    return  db_filename;
}

char *set_dbfilename(const char* filename)
{
	strcpy(db_filename, filename);
	return db_filename;
}


int db_open()
{
	debug("DEBUG: Using %s as database\n", get_dbfilename());
	if ( db_file )
		return true;
	db_file = fopen(get_dbfilename(), "r+");
	if (!db_file) {
		fprintf(stderr, "fopen '%s' %s\n", get_dbfilename(), strerror(errno));
		syslog(LOG_ERR, "fopen '%s' %s\n", get_dbfilename(), strerror(errno));
		return false;
	};
	// lock the file
	FLOCK(db_file);
	return true;
}

int db_close()
{
	debug("db_close()\n");
	if (db_file) {
		FUNLOCK(db_file);
		fclose(db_file);
	}
	db_file = NULL;
	return true;
};

int kdbinit()
{
	db_already_readed=0;
    db_lines_count=0;
    quotation=0;
	make_export=0;
	make_local=0;
	need_print_count=0;
    need_write=false;
    int i;
	db_filename[0]='\0';
    for(i=0; i<MAX_LINES; i++)
        db_lines[i].name=NULL, db_lines[i].value=NULL;

    return 0;
}

inline int db_add(const char* name, const char* value, int dup)
{
	if ( db_lines_count >= MAX_LINES ) {
		warn("Warning: db_add(): MAX_LINES %d reached\n", MAX_LINES);
		return false;
	}

	debug("DEBUG: db_add(name='%s', value='%s', dup=%d): db_lines[%d]\n", name, value, dup, db_lines_count);

	db_lines[db_lines_count].name=dup?strdup(name):(char*)name;
	db_lines[db_lines_count].value=dup?strdup(value):(char*)value;
	db_lines_count++;
	return true;
}

// Parse str "name=value" and put 'name' to *name and 'value' to *value
int parse_pair(const char* str, char* name, char* value)
{
    int len = strlen(str);
    if (!len)
        return false;
    char s[len+10];

    strcpy(s, str);
	if (s[len-1]=='\n')
		s[len-1]='\0';

    char *eq=strchr(s, '=');
    if (!eq || eq==s) {
        syslog(LOG_ERR, "Error: can't parse: %s", s);
        return false;
    }
    eq[0]='\0';
    strcpy(name, s );
    strcpy(value, eq+1);

    return true;
}

int parse_header(char *header)
{
    if (strncmp(HEADER_LINE, header, 3)){
        syslog(LOG_ERR, "bad file format\n");
        return false;
    }

    return true;
}

int db_unserialize(const char *buf)
{
	int len=strlen(buf);
    char name[len+10], value[len+10];
	debug("DEBUG: db_unserialize ('%s')\n", buf);
	if ( parse_pair(buf, name, value) ) 
		return db_add(str_unescape(name), str_unescape(value), false);
	else
		return false;
}

int db_serialize(int index, char *buf)
{
	char *ename=str_escape(db_lines[index].name);
	char *evalue=str_escape(db_lines[index].value);
	int len = sprintf(buf, "%s=%s\n", ename, evalue);
	debug("DEBUG: db_serialize (%d, '%s=%s')\n", index, ename, evalue);
	free(ename);
	free(evalue);
	return len;
}



int db_read()
{
    int result=true;
    char *buf=NULL;
	char *pointer=NULL;
	char head_buf[sizeof(HEADER_LINE)+10];

	// check if file already readed
	if (db_already_readed)
		return true;

	if (! db_open() )
		return false;

	// read header
	if (! fgets(head_buf, sizeof(head_buf), db_file) ) {
		if ( ferror(db_file) ) {
			perror("fgets");
			return false;
		}
	}

	// check header
    if (! parse_header(head_buf) )
        return false;

    if (! (buf=(char*) calloc(READ_BUFFER_LEN, 1)) ) {
        perror("calloc");
        return false;
    }

	while ( result=fgets(buf, READ_BUFFER_LEN, db_file) && result != EOF ) {
		if ( ! strncmp(buf, FOOTER_LINE, sizeof(FOOTER_LINE)) )
			break;
		if ( ! db_unserialize(buf) )
			return false;
	}

	db_already_readed = true;

    if (buf)
        free(buf);
    return true;
}


int sort_func(const void *pa, const void *pb){
	_db_record *a=(_db_record*)pa;
	_db_record *b=(_db_record*)pb;
	
	debug("DEBUG: sort() compare '%s' and '%s'\n", a->name, b->name);
	return strcmp(a->name, b->name);
}

int db_sort(){
	qsort(db_lines, db_lines_count, sizeof(_db_record), sort_func);
}

inline int print_pair(const char* name, const char* value)
{
	char *prefix="";
	if (make_local)
		prefix="local ";
	else if (make_export)
		prefix="export ";
	//debug("DEBUG: print_pair(): %s%s=%s\n", prefix, name, value);
	switch(quotation) {
		case 0: 
			if (name)
				printf("%s%s=%s\n", prefix, name, value);
			else
				printf("%s\n", value);
			break;
		case 1:
			if (name)
				printf("%s%s=\"%s\"\n", prefix, name, value);
			else
				printf("\"%s\"\n", value);
			break;
		default:
			if (name)
				printf("%s%s='%s'\n", prefix, name, value);
			else
				printf("'%s'\n", value);
			break;
	}

    return true;
}

int db_set_index(int index, const char *name, const char *value, int dup)
{
	if ( index < 0 || index >=db_lines_count ) {
		warn("Error: db_set_index(): index %d is out of bound\n", index);
		return false;
	}
	debug("DEBUG: db_set_index(): sets db_lines[%d]: name=%s, value=%s\n", index, name, value);

	if (name) {
		// release memory
		if ( db_lines[index].name )
			free(db_lines[index].name);
		// replace values
		db_lines[index].name = dup?strdup(name):(char*)name;
	}
	if (value) {
		// release memory
		if ( db_lines[index].value )
			free(db_lines[index].value);
		// replace values
		db_lines[index].value = dup?strdup(value):(char*)value;
	};
	need_write++;
	return true;
}

int find_key(const char *key)
{
    int i;

    if ( !key )
        return false;

    for(i=0; i<db_lines_count; i++) 
        if (!strcmp(key, db_lines[i].name)) 
            return i;

    return -1;
}

int db_set(const char *name, const char* value)
{
    int result=false;
    int index;
    if ( ! db_read() )
		return false;

    index = find_key(name);
    if (index != -1) {
        result=db_set_index(index, name, value, true);
    } else {
		result=db_add(name, value, true);
    }
    if (result)
        need_write++;

    return result;
}

int db_del_index(int index)
{
	int i=index;
    if (index < 0 || index >= db_lines_count)
		return false;

	// release memory
	if ( db_lines[index].name )
		free(db_lines[index].name);
	if ( db_lines[index].value )
		free(db_lines[index].value);

	while( i < db_lines_count-1 ) {
		db_lines[i].name=db_lines[i+1].name;
		db_lines[i].value=db_lines[i+1].value;
		i++;
	}
	debug(" DEBUG: deleted db_lines[%d]\n", index);
	db_lines_count--;
	need_write=true;
	return true;
}

int db_write()
{
    int i, len, written;
    int result=true;
	char *buf, *p;
	// sorts the data
	debug("DEBUG: sorts data\n");
	db_sort();
	rewind(db_file);
	ftruncate(fileno(db_file), 0);

	// alloc
    if (! (buf=(char*) calloc(READ_BUFFER_LEN, 1)) ) {
        perror("calloc");
        return false;
    }
	p=buf;

	p+=sprintf(buf, HEADER_LINE);

	// write all db_lines
    for(i=0; i<db_lines_count; i++) 
		p+=db_serialize(i, p);
	// append footer
	strcat(p, FOOTER_LINE);
	p+=sizeof(FOOTER_LINE)-1;

	debug("DEBUG: writing data to database\n");
	fwrite(buf, 1, p-buf, db_file);
	
    return result;
}

void print_count(int count)
{
    char ls_count[sizeof(COUNTS_NAME)+20];
	if (need_print_count) {
		sprintf(ls_count, "%d", count);
		print_pair(COUNTS_NAME, ls_count);
	};
}

/****************
 *
 *  kdb commands
 *
 ****************/

int import(const char *filename)
{
    FILE *file;
    int result=true;
    char buf[MAX_LINE_SIZE];
    char name[MAX_LINE_SIZE], value[MAX_LINE_SIZE];
    if (!filename || (!strlen(filename)) || (!strcmp("-", filename)) )
        file=stdin;
    else {
        file = fopen(filename, "r+");
        if (!file) {
            perror("fopen");
            result=false;
        };
    };
	if (! result )
		return false;

	db_file = file;
	db_already_readed = false;
	if ( ! db_read() )
		return false;
    fclose(file);

	db_file = NULL;
	db_open();
	db_write();
	need_write++;

    return result;
}

int export(const char *filename)
{
    FILE *file, *tmp;
    int result=true;
    char buf[MAX_LINE_SIZE];
    char name[MAX_LINE_SIZE], value[MAX_LINE_SIZE];
    if (!filename || (!strlen(filename)) || (!strcmp("-", filename)) )
        file=stdout;
    else {
        file = fopen(filename, "w+");
        if (!file) {
            perror("fopen");
            result=false;
        };
    };
    if (! result )
		return false;

	db_read();
	tmp = db_file;
	db_file = file;
	db_write();
    fclose(file);
	db_file = tmp;
    return true;
}

int edit(const char* me)
{
    int result=true;
    char *tmpname=tmpnam(NULL);
    char editor[255];
    char buf[255];
    if (!tmpname)
        return false;
    if (getenv("EDITOR"))
        sprintf(editor, "%s %s", getenv("EDITOR"), tmpname) ;
    else
        sprintf(editor, "vi %s", tmpname);

    
    sprintf(buf, "%s export > %s", me, tmpname);
    if (system(buf))
        return false;
    
    if (! system(editor))  {
        result=import(tmpname);
        remove(tmpname);
    }

    return result;
}

int rename(const char *oldkey, const char* newkey)
{
	db_read();
	// first - remove newkey
    int index = find_key(newkey);
	if ( index != -1 ) 
		db_del_index(index);
	// replace oldkey name to new name
    index = find_key(oldkey);
	if ( index == -1 ) 
		return false;
	db_set_index(index, newkey, NULL, true);
	return ++need_write;
}

int get(const char *key)
{
	int i;
    db_read();

	WILDCARD_LOOP(i, key) 
		print_pair(NULL, db_lines[i].value);

	return true;
}

int isset(const char *key)
{
    db_read();
    return (find_key(key) !=-1);
}

// set with wildcard matching
int set(const char *str)
{
	int len=strlen(str);
	char iname[len+10], ivalue[len+10];
	int i;

	db_read();
	if (!parse_pair(str, iname, ivalue)) 
		return false;

	if (! strcmp (ivalue, ENV_STRING) ) {
		char *value;
		const char *name=iname;

		if ( !strncmp(name, "FORM_", 5) ) // 5 - sizeof("FORM_")
			name+=5;
		value=getenv(name);

		return db_set( name, value?value:"" );
	} else if ( is_wildcarded(iname) ) {
		WILDCARD_LOOP(i, iname) {
			db_set_index(i, NULL, ivalue, true);
			need_write++;
		}
	} else {
		return db_set( iname, ivalue );
	}

	return true;
}

// delete with wildcard matching
int del(const char *str)
{
	int i=0;

    db_read();
	while ( i != db_lines_count ) {
		WILDCARD_LOOP(i, str) {
			db_del_index(i);
			need_write++;
			break;
		}
	}
	return true;
}


int sublist(const char *key)
{
    int result=true;
    int i,count=0;
    int len=strlen(key);
    char name[len+10], value[len+10];

    db_read();
    for( i=0; i<db_lines_count; i++ ) {
        if( key && len ){
            if ( !strncmp(key, db_lines[i].name, len) ) {
                char *s=db_lines[i].name+len;
                print_pair(s, db_lines[i].value);
				count++;
            }
        } 
    }
	print_count(count);

    return result;
}

// keylist with wildcard matching
int keylist(const char *key)
{
    int i=0, count=0;

    db_read();

	WILDCARD_LOOP(i, key) {
		print_pair(NULL, db_lines[i].name);
	   	count++;
	}

	print_count(count);

	return true;
}

// sub sub keylist with wildcard matching
// finds patter in 'name=value'
// and cutoff cut_prefix and cut_suffix
// Example: 
// # kdb ls
// sys_iface_dsl0_valid=1
// sys_iface_dsl1_valid=0
// sys_iface_eth0_valid=1
// sys_iface_eth1_valid=1
// # kdb sskls sys*valid=1 sys_iface_ _valid
// dsl0
// eth0
// eth1
// 
int sskeylist(const char* pattern, const char *cut_prefix, const char* cut_suffix)
{
    int i=0, count=0;
    int result=true;
    int len=strlen(cut_prefix);
	char *tail;
	char strbuf[MAX_LINE_SIZE];

    db_read();

    for( i=0; i<db_lines_count; i++ ) {
        if( len ){
			snprintf(strbuf, sizeof(strbuf), "%s=%s", db_lines[i].name, db_lines[i].value);
			if ( (match_wildcard(pattern, strbuf)) &&  ( !strncmp(cut_prefix, db_lines[i].name, len) ) ) {
                char *s=db_lines[i].name+len;
				strncpy(strbuf, s, sizeof(strbuf));
				tail = strstr(strbuf, cut_suffix);
				if (tail)
					tail[0]='\0';
                print_pair(NULL, strbuf);
            }
        } 
    }

	return true;
}

int listrm(const char *key)
{
    int result=true;
    int i, count=0, index;
    char name[MAX_LINE_SIZE], value[MAX_LINE_SIZE], prefix_name[MAX_LINE_SIZE], tmps[MAX_LINE_SIZE], new_name[MAX_LINE_SIZE];
	char *s;
	_db_record local_lines[MAX_LINES];
	int local_lines_count=0;
    db_read();

	s=strrchr(key, '_');
	if(!s) {
		return false;
	};
	strncpy(prefix_name, key, s-key+1);
	debug("DEBUG: prefix_name=%s\n", prefix_name);
	
	// finds all key_[0-9]+ keys, and move it to local_lines array
	i=0;
	while(true) {
		snprintf(tmps, sizeof(tmps), "%s%d", prefix_name, i);
		index=find_key(tmps);
		if ( index >= 0 ) {
			debug( "DEBUG: db_lines[%d]='%s' \n", index, db_lines[index]);
			// if index != rm_index - add pair to temp local_lines array
			if ( strcmp(db_lines[index].name, key) ) {
				strcpy(name, db_lines[index].name);
				strcpy(value, db_lines[index].value);
				// finds last '_'
				s=strrchr(name, '_');
				// truncate name after last '_'
				s[1]='\0';
				snprintf(new_name, sizeof(new_name), "%s%d", name, local_lines_count);
				debug( "  DEBUG: new_name=%s value=%s \n", new_name, value);
				local_lines[local_lines_count].name=strdup(new_name);
				local_lines[local_lines_count++].value=strdup(value);
				debug( "  DEBUG: added to local_lines '%s=%s' \n", local_lines[local_lines_count-1].name, local_lines[local_lines_count-1].value);
			};
			db_del_index(index);
		} else {
			break;
		};
		i++;
	}
	// yes, we have some keys=values
	if (i) {
		i=0;
		for(i=0; i<local_lines_count; i++) {
			db_set(local_lines[i].name, local_lines[i].value);
			if (local_lines[i].name)
				free(local_lines[i].name), free(local_lines[i].value);
		}
	}
	return result;
}

int list_getnextindex(const char *name)
{
	int i, index;
    char s[MAX_LINE_SIZE];
	for ( i=0; i < MAX_LINES; i++) {
		snprintf(s, sizeof(s), "%s%d", name, i);
		index=find_key(s);
		if ( index == -1 )
			return i;
	};
	return -1;
}

// UNFINISHED!!!
int list_getnext(const char *str) 
{
    int result=true;
	int i, index;
	const int max_index=MAX_LINES/2;
    char name[MAX_LINE_SIZE], value[MAX_LINE_SIZE], s[MAX_LINE_SIZE];
    db_read();
	
	parse_pair(str, name, value);

	for ( i=0; i < strlen(name); i++ )
		if ( name[i] = '%' ) {
			name[i]='\0';
			break;
		}
	
	/*
	for ( index=0; index < max_index; index++ ) {
		sprintf(s, "%s%d_", name, index);
		for ( i=0; i < db_lines_count; i++ ) {
			if ( ! strncmp (name, db_lines[i].name ) )
				break;

		}
	}*/
	return true;
}

int listadd(const char* str)
{
    int result=true;
	int i,index;
    char name[MAX_LINE_SIZE], value[MAX_LINE_SIZE], s[MAX_LINE_SIZE];
    db_read();
	
	parse_pair(str, name, value);
	
	// check for '_' at the end of string
	if ( name[strlen(name)-1] != '_' ) {
		syslog(LOG_ERR, "Error: key should ends with '_'\n");
		return false;
	};
		
	snprintf(s, sizeof(s), "%s%d", name, list_getnextindex(name));

	if (! strcmp (value, ENV_STRING) ) {
		char *qvalue;
		const char *qname=name;

		if ( !strncmp(qname, "FORM_", 5) ) // 5 - sizeof("FORM_")
			qname+=5;
		qvalue=getenv(qname);

		need_write++;
		return db_add( s, qvalue?qvalue:"", true );
	} else {
		need_write++;
		return db_add(s, value, true);
	}
}

// list with wildcard matching
int list(const char *key)
{
    int i=0, count=0;
	const char *wkey;

	// use '*' if key is empty 
	if (key && key[0])
		wkey=key;
	else
		wkey="*";

    db_read();

	WILDCARD_LOOP(i, wkey) {
		print_pair(db_lines[i].name, db_lines[i].value);
	   	count++;
	};

	print_count(count);

	return true;
}

int createdb(const char *filename)
{
    if (strlen(filename))
		set_dbfilename(filename);

	// create or truncate file
	FILE *f=fopen(get_dbfilename(), "w");
	if (!f) {
		perror("fopen");
		return false;
	}
	fclose(f);

	db_open();
	need_write=true;
   
    return true;
};

#ifdef SHELL
int kdbcmd(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
    int ch, i;
    int result=false;
    char cmd[MAX_LINE_SIZE];
    char param[MAX_LINE_SIZE];

    kdbinit();
	optind=1;

    while ((ch = getopt(argc, argv, "qlecf:")) != -1){
        switch(ch) {
            case 'f': set_dbfilename(optarg);
                     break;
            case 'q': quotation++;
                     break;
            case 'l': make_local++;
                     break;
            case 'e': make_export++;
                     break;
            case 'c': need_print_count++;
                     break;
            default: show_usage(argv[0]);
                     break;
        }
    }

	openlog("kdb", LOG_PERROR, LOG_USER);

    if ( argc <= optind ) 
        show_usage(argv[0]);

	while(true) {
		if ( optind >= argc ) 
			break;

		strcpy(cmd, argv[optind]);
		optind++;
		// if cmd == ':' then go to next cmd
		if ( cmd[0]==':' )
			continue;


		if ( (optind >= argc) || (strcmp(":", argv[optind])==0)) {
			strcpy(param, "");
		} else {
			strcpy(param, argv[optind++]);
		}

		if ( (!strcmp(cmd, "list")) || (!strcmp(cmd, "ls")) )
			result = list(param);
		else if (!strcmp(cmd, "set"))
			result = set(param);
		else if (!strcmp(cmd, "get"))
			result = get(param);
		else if ( (!strcmp(cmd, "sls")) || (!strcmp(cmd, "slist")) || (!strcmp(cmd, "sublist")) )
			result = sublist(param);
		else if ( (!strcmp(cmd, "listrm")) || (!strcmp(cmd, "lrm")) )
			result = listrm(param);
		else if ( (!strcmp(cmd, "listadd")) || (!strcmp(cmd, "ladd")) )
			result = listadd(param);
		else if (!strcmp(cmd, "isset"))
			result = isset(param);
		else if ( (!strcmp(cmd, "klist")) || (!strcmp(cmd, "kls")) )
			result = keylist(param);
		else if (!strcmp(cmd, "sskls"))  {
			if ( (optind+2) <= argc ) {
				char *pref=argv[optind];
				char *suff=argv[++optind];
				result = sskeylist(param, pref, suff);
				optind+=1;
			} else
				result = false; 
		}
		
		else if ( (!strcmp(cmd, "del")) || (!strcmp(cmd, "rm")) )
			result = del(param);
		else if ( !strcmp(cmd, "edit") )
			result = edit(argv[0]);
		else if ( !strcmp(cmd, "rename") || !strcmp(cmd, "rn") )
			result = rename(param, argv[optind++]);
		else if ( !strcmp(cmd, "create") )
			result = createdb(param);
		else if ( !strcmp(cmd, "import") )
			result = import(param);
		else if ( !strcmp(cmd, "export") )
			result = export(param);
		else 
			show_usage(argv[0]);
		if (!result)
			break;
		optind++;
	}

	debug("result=%d, need_write=%d\n", result, need_write);
    if (result && need_write)
		db_write();
	db_close();
	return !result;
}






// copyright (c) 2003-2005 chisel <storlek@chisel.cjb.net>
/* adapted from glib. in addition to the normal c escapes, this also escapes the comment character (#)
 *  * as \043.  */
char *str_escape(const char *source)
{
	const char *p = source;
	/* Each source byte needs maximally four destination chars (\777) */
	char *dest = (char*)calloc(4 * strlen(source) + 1, sizeof(char));
	char *q = dest;

	while (*p) {
		switch (*p) {
		case '\a':
			*q++ = '\\';
			*q++ = 'a';
		case '\b':
			*q++ = '\\';
			*q++ = 'b';
			break;
		case '\f':
			*q++ = '\\';
			*q++ = 'f';
			break;
		case '\n':
			*q++ = '\\';
			*q++ = 'n';
			break;
		case '\r':
			*q++ = '\\';
			*q++ = 'r';
			break;
		case '\t':
			*q++ = '\\';
			*q++ = 't';
			break;
		case '\v':
			*q++ = '\\';
			*q++ = 'v';
			break;
		case '\\': case '"': case '\'': 
			*q++ = '\\';
			*q++ = *p;
			break;
		default:
			if ((*p <= ' ') || (*p >= 0177) || (*p == '=') || (*p == '#')) {
				*q++ = '\\';
				*q++ = '0' + (((*p) >> 6) & 07);
				*q++ = '0' + (((*p) >> 3) & 07);
				*q++ = '0' + ((*p) & 07);
			} else {
				*q++ = *p;
			}
			break;
		}
		p++;
	}

	*q = 0;

	return dest;
}

/* opposite of str_escape. (this is glib's 'compress' function renamed more clearly)
 * TODO: it'd be nice to handle \xNN as well... */
char *str_unescape(const char *source)
{
	const char *p = source;
	const char *octal;
	char *dest = (char*)calloc(strlen(source) + 1, sizeof(char));
	char *q = dest;

	while (*p) {
		if (*p == '\\') {
			p++;
			switch (*p) {
			case '0'...'7':
				*q = 0;
				octal = p;
				while ((p < octal + 3) && (*p >= '0') && (*p <= '7')) {
					*q = (*q * 8) + (*p - '0');
					p++;
				}
				q++;
				p--;
				break;
			case 'a':
				*q++ = '\a';
				break;
			case 'b':
				*q++ = '\b';
				break;
			case 'f':
				*q++ = '\f';
				break;
			case 'n':
				*q++ = '\n';
				break;
			case 'r':
				*q++ = '\r';
				break;
			case 't':
				*q++ = '\t';
				break;
			case 'v':
				*q++ = '\v';
				break;
			default:		/* Also handles \" and \\ */
				*q++ = *p;
				break;
			}
		} else {
			*q++ = *p;
		}
		p++;
	}
	*q = 0;

	return dest;
}


int is_wildcarded(const char *str)
{
	int i, len=strlen(str);
	for ( i=0; i < len; i++ )
		if ( str[i] == '*' )
			return true;
	return false;
}




/* Wildcard code from ndtpd */
/*
 * Copyright (c) 1997, 98, 2000, 01  
 *    Motoyuki Kasahara
 *    ndtpd-3.1.5
 */

/*
 * Do wildcard pattern matching.
 * In the pattern, the following characters have special meaning.
 * 
 *   `*'    matches any sequence of zero or more characters.
 *   '\x'   a character following a backslash is taken literally.
 *          (e.g. '\*' means an asterisk itself.)
 *
 * If `pattern' matches to `string', 1 is returned.  Otherwise 0 is
 * returned.
 */
int match_wildcard(const char *pattern, const char *string)
{
    const char *pattern_p = pattern;
    const char *string_p = string;

    while (*pattern_p != '\0') {
	if (*pattern_p == '*') {
	    pattern_p++;
	    if (*pattern_p == '\0')
		return 1;
	    while (*string_p != '\0') {
		if (*string_p == *pattern_p
		    && match_wildcard(pattern_p, string_p))
		    return 1;
		string_p++;
	    }
	    return 0;
	} else {
	    if (*pattern_p == '\\' && *(pattern_p + 1) != '\0')
		pattern_p++;
	    if (*pattern_p != *string_p)
		return 0;
	}
	pattern_p++;
	string_p++;
    }

    return (*string_p == '\0');
}

