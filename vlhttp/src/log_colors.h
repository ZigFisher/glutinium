
#ifdef LIBLOG_COLORS

#define CLEARLINE       "\e[L\e[G"
#define INITTERM        "\e[H\e[2J"
#define ENDTERM         ""
#define SAVE            "\e7"
#define RESTORE         "\e8"
#define HOME            "\e[H"

/* These are the ANSI codes for foreground text colors */
#define ANSI_BOLD       "\e[1m"
#define ANSI_BLACK      "\e[0;30m"
#define ANSI_DRED       "\e[0;31m"
#define ANSI_DGREEN     "\e[0;32m"
#define ANSI_ORANGE     "\e[0;33m"
#define ANSI_DBLUE      "\e[0;34m"
#define ANSI_PURPLE     "\e[0;35m"
#define ANSI_CYAN       "\e[0;36m"
#define ANSI_GREY       "\e[0;37m"
#define ANSI_DGREY      "\e[1;30m"
#define ANSI_RED        "\e[1;31m"
#define ANSI_GREEN      "\e[1;32m"
#define ANSI_YELLOW     "\e[1;33m"
#define ANSI_BLUE       "\e[1;34m"
#define ANSI_PINK       "\e[1;35m"
#define ANSI_LBLUE      "\e[1;36m"
#define ANSI_WHITE      "\e[1;37m"
#define ANSI_RESET      "\e[0m"

/* These are the ANSI codes for blinking foreground text colors */
#define BLINK_BLACK             "\e[0;5;30m"
#define BLINK_DRED              "\e[0;5;31m"
#define BLINK_DGREEN            "\e[0;5;32m"
#define BLINK_ORANGE            "\e[0;5;33m"
#define BLINK_DBLUE             "\e[0;5;34m"
#define BLINK_PURPLE            "\e[0;5;35m"
#define BLINK_CYAN              "\e[0;5;36m"
#define BLINK_GREY              "\e[0;5;37m"
#define BLINK_DGREY             "\e[1;5;30m"
#define BLINK_RED               "\e[1;5;31m"
#define BLINK_GREEN             "\e[1;5;32m"
#define BLINK_YELLOW            "\e[1;5;33m"
#define BLINK_BLUE              "\e[1;5;34m"
#define BLINK_PINK              "\e[1;5;35m"
#define BLINK_LBLUE             "\e[1;5;36m"
#define BLINK_WHITE             "\e[1;5;37m"

/* These are the ANSI codes for background colors */
#define BACK_BLACK      "\e[40m"
#define BACK_DRED       "\e[41m"
#define BACK_DGREEN     "\e[42m"
#define BACK_ORANGE     "\e[43m"
#define BACK_DBLUE      "\e[44m"
#define BACK_PURPLE     "\e[45m"
#define BACK_CYAN       "\e[46m"
#define BACK_GREY       "\e[47m"

/* Other miscelaneous ANSI tags that can be used */
#define ANSI_UNDERLINE  "\e[4m" /* Underline text */
#define ANSI_ITALIC     "\e[6m" /* Italic text */
#define ANSI_REVERSE    "\e[7m" /* Reverse colors */

#else


#define CLEARLINE       ""
#define INITTERM        ""
#define ENDTERM         ""
#define SAVE            ""
#define RESTORE         ""
#define HOME            ""

/* These are the ANSI codes for foreground text colors */
#define ANSI_BOLD       ""
#define ANSI_BLACK      ""
#define ANSI_DRED       ""
#define ANSI_DGREEN     ""
#define ANSI_ORANGE     ""
#define ANSI_DBLUE      ""
#define ANSI_PURPLE     ""
#define ANSI_CYAN       ""
#define ANSI_GREY       ""
#define ANSI_DGREY      ""
#define ANSI_RED        ""
#define ANSI_GREEN      ""
#define ANSI_YELLOW     ""
#define ANSI_BLUE       ""
#define ANSI_PINK       ""
#define ANSI_LBLUE      ""
#define ANSI_WHITE      ""
#define ANSI_RESET      ""

/* These are the ANSI codes for blinking foreground text colors */
#define BLINK_BLACK             ""
#define BLINK_DRED              ""
#define BLINK_DGREEN            ""
#define BLINK_ORANGE            ""
#define BLINK_DBLUE             ""
#define BLINK_PURPLE            ""
#define BLINK_CYAN              ""
#define BLINK_GREY              ""
#define BLINK_DGREY             ""
#define BLINK_RED               ""
#define BLINK_GREEN             ""
#define BLINK_YELLOW            ""
#define BLINK_BLUE              ""
#define BLINK_PINK              ""
#define BLINK_LBLUE             ""
#define BLINK_WHITE             ""

/* These are the ANSI codes for background colors */
#define BACK_BLACK      ""
#define BACK_DRED       ""
#define BACK_DGREEN     ""
#define BACK_ORANGE     ""
#define BACK_DBLUE      ""
#define BACK_PURPLE     ""
#define BACK_CYAN       ""
#define BACK_GREY       ""

/* Other miscelaneous ANSI tags that can be used */
#define ANSI_UNDERLINE  "" /* Underline text */
#define ANSI_ITALIC     "" /* Italic text */
#define ANSI_REVERSE    "" /* Reverse colors */

#endif


