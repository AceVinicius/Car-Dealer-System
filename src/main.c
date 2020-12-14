/**
 * @author Vinícius Ferreira Aguiar (acevinicius@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2020-12-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include <mysql/mysql.h>

#define EXIT               'E'
#define SIGN_OUT           'O'
#define SIGN_IN            'I'
#define REGISTER_NEW_CAR   'N'
#define SELL               'S'
#define TRADE              'T'
#define SCHEDULE_REVISION  'R'
#define MANAGEMENT         'M'
#define NEW_FUNC           'F'
#define NEW_SECTOR         'S'

#define RUNNING true
#define STOP    false

#define PADDING 6
















const char *k_menu_sign_in[ ] = {
    "Sign [I]n",
    "[E]xit"
};
const int k_menu_sign_in_size = sizeof( k_menu_sign_in ) / sizeof( char * );
const int k_menu_sign_in_height = k_menu_sign_in_size + 6;


const char *k_menu_admin[ ] = {
    "Register [N]ew Car",
    "[S]ell",
    "[T]rade",
    "Schedule [R]evision",
    "[M]anagement",
    "Sign [O]ut"
};
const int k_menu_admin_size  = sizeof( k_menu_admin ) / sizeof( char * );
const int k_menu_admin_height = k_menu_admin_size + PADDING;


const char *k_menu_admin_management[ ] = {
    "New [S]ector",
    "New [F]unctionary",
    "[B]ack"
};
const int k_menu_admin_management_size  = sizeof( k_menu_admin_management ) / sizeof( char * );
const int k_menu_admin_management_height = k_menu_admin_management_size + PADDING;


const char *k_menu_user[ ] = {
    "Register [N]ew Car",
    "[S]ell",
    "[T]rade",
    "Schedule [R]evision",
    "Sign [O]ut"
};
const int k_menu_user_size  = sizeof( k_menu_user ) / sizeof( char * );
const int k_menu_user_height = k_menu_user_size + PADDING;


const char k_program[ ] = {
    " Car Dealer "
};
const int k_program_size = sizeof( k_program ) / sizeof( char );


const char k_creator[ ] = {
    " Vinicius F. Aguiar "
};
const int k_creator_size = sizeof( k_creator ) / sizeof( char );


const int k_width_min = 50;


const char k_question_database[ ] = {
    "No database was given. Do you want to specify one?"
};
const char k_question_admin[ ] = {
    "Are you an Admin?"
};
// const char question_database[ ] = {
//     "No database was given. Do you want to specify one?"
// };
const char k_fatal_error[ ] = {
    "\033[31;1mfatal error:\033[0;0m"
};






















inline void error_api_mysql( MYSQL *connection );
void close_api_mysql( MYSQL *connection );
MYSQL *init_api_mysql( const char *database_name );
static char *get_db_name_from_user( void );
char get_yn_from_user( const char *question );
inline bool is_admin( void );
char get_char_from_user( void );
























MYSQL *
init_api_mysql( const char *database_name )
{
    MYSQL *new_connection = mysql_init(NULL);
    if (new_connection == NULL)
    {
        error_api_mysql(new_connection);
        close_api_mysql(new_connection);
        return NULL;
    }

    if (mysql_real_connect(new_connection, "localhost", "root", "Appetite#1987", NULL, 0, NULL, 0) == NULL)
    {
        error_api_mysql(new_connection);
        close_api_mysql(new_connection);
        return NULL;
    }
    
    return new_connection;
}

inline void error_api_mysql( MYSQL *connection ) { fprintf(stderr, "%s\n", mysql_error(connection)); }

void
close_api_mysql( MYSQL *connection )
{
    if (connection != NULL)
    {
        mysql_close(connection);
        connection = NULL;
    }
}

int
init_api_ncurses( void )
{
    if (has_colors())
    {
        fprintf(stderr, "dictionary: %s your terminal does not support colors\n", k_fatal_error);
        return EXIT_FAILURE;
    }

    initscr();
    cbreak();
    // raw();
    curs_set(0);
    start_color();
    init_pair(1, COLOR_MAGENTA , COLOR_MAGENTA);
    init_pair(2, COLOR_BLACK , COLOR_BLACK);
    init_pair(3, 16          , 15);
    init_pair(4, COLOR_RED   , 15);
    init_pair(5, COLOR_BLACK , 16);
    init_pair(6, COLOR_WHITE , 16);

    return EXIT_SUCCESS;
}
























inline bool is_admin( void ) { return get_yn_from_user(k_question_admin) == 'Y'; }

static char *
get_db_name_from_user( void )
{
    char *database_name = (char *) malloc(sizeof(char) * 20);
    char  answer = get_yn_from_user(k_question_database);

    if (answer == 'N') exit(EXIT_SUCCESS);

    printf("What's the database name? ");
    scanf(" %s", database_name);

    return database_name;
}



char
get_yn_from_user( const char *question )
{
    char answer;

    do
    {
        printf("%s [Y/n] ", question);
        answer = get_char_from_user();
    }
    while (answer != 'Y' && answer != 'N');

    return answer;
}



char
get_char_from_user( void )
{
    char character;

    scanf(" %c%*c", &character);
    character = toupper(character);

    return character;
}


















/******************************************************************************
 ****                         SCREEN OUTPUT LAYOUT                         ****
 ******************************************************************************/

WINDOW *
create_basic_layout( const int min_y, const int min_x )
{
    int term_y;
    int term_x;

    getmaxyx(stdscr, term_y, term_x);

    const int padding_y = term_y < min_y ? 3 : term_y - min_y;
    const int padding_x = term_x < min_x ? 3 : term_x - min_x;
    const int height    = term_y - padding_y;
    const int width     = term_x - padding_x;
    const int start_y   = padding_y / 2;
    const int start_x   = padding_x / 2;

    WINDOW *content = newwin(height, width, start_y, start_x);
    WINDOW *shadow  = newwin(height, width, start_y + 1, start_x + 1);

    bkgd(COLOR_PAIR(1));
    wbkgd(shadow, COLOR_PAIR(2));
    wbkgd(content, COLOR_PAIR(3));

    box(content, 0, 0);
    wattron(content, COLOR_PAIR(4));
    mvwprintw(content, 0, (width - k_program_size) / 2, k_program);
    mvwprintw(content, height - 1, width - k_creator_size - 2, k_creator);
    wattroff(content, COLOR_PAIR(4));

    refresh();
    wrefresh(shadow);
    wrefresh(content);

    return content;
}

int
screen_sign_in( void )
{
    int  choice;
    int  highlight = 0;
    bool status    = RUNNING;

    while (status)
    {
        WINDOW *content = create_basic_layout(k_menu_sign_in_height, k_width_min);
        keypad(content, true);

        for (int i = 0; i < k_menu_sign_in_size; ++i)
        {
            if (i == highlight)
            {
                wattron(content, COLOR_PAIR(4));
            }
            mvwprintw(content, i+3, 4, k_menu_sign_in[ i ]);
            wattroff(content, COLOR_PAIR(4));
        }

        choice = wgetch(content);

        switch (choice)
        {
            case KEY_UP:
                if (highlight > 0) highlight--;
                break;
            case KEY_DOWN:
                if (highlight < k_menu_sign_in_size - 1) highlight++;
                break;
            case 'I': case 'i': highlight = 0; break;
            case 'E': case 'e': highlight = 1; break;
            case '\n':          status = STOP; break;
            default: break;
        }
    }

    return highlight;
}

int
screen_menu_admin( void )
{
    int  choice;
    int  highlight = 0;
    bool status    = RUNNING;

    while (status)
    {
        WINDOW *content = create_basic_layout(k_menu_admin_height, k_width_min);
        keypad(content, true);

        for (int i = 0; i < k_menu_admin_size; ++i)
        {
            if (i == highlight)
            {
                wattron(content, COLOR_PAIR(4));
            }
            mvwprintw(content, i+PADDING/2, 4, k_menu_admin[ i ]);
            wattroff(content, COLOR_PAIR(4));
        }

        choice = wgetch(content);

        switch (choice)
        {
            case KEY_UP:
                if (highlight > 0) highlight--;
                break;
            case KEY_DOWN:
                if (highlight < k_menu_admin_size - 1) highlight++;
                break;
            case 'N': case 'n': highlight = 0; break;
            case 'S': case 's': highlight = 1; break;
            case 'T': case 't': highlight = 2; break;
            case 'R': case 'r': highlight = 3; break;
            case 'M': case 'm': highlight = 4; break;
            case 'O': case 'o': highlight = 5; break;
            case '\n':          status = STOP; break;
            default: break;
        }
    }

    return highlight;
}

int
screen_menu_user( void )
{
    int  choice;
    int  highlight = 0;
    bool status    = RUNNING;

    while (status)
    {
        WINDOW *content = create_basic_layout(k_menu_user_height, k_width_min);
        keypad(content, true);

        for (int i = 0; i < k_menu_user_size; ++i)
        {
            if (i == highlight)
            {
                wattron(content, COLOR_PAIR(4));
            }
            mvwprintw(content, i+PADDING/2, 4, k_menu_user[ i ]);
            wattroff(content, COLOR_PAIR(4));
        }

        choice = wgetch(content);

        switch (choice)
        {
            case KEY_UP:
                if (highlight > 0) highlight--;
                break;
            case KEY_DOWN:
                if (highlight < k_menu_user_size - 1) highlight++;
                break;
            case 'N': case 'n': highlight = 0; break;
            case 'S': case 's': highlight = 1; break;
            case 'T': case 't': highlight = 2; break;
            case 'R': case 'r': highlight = 3; break;
            case 'O': case 'o': highlight = 5; break;
            case '\n':          status = STOP; break;
            default: break;
        }
    }

    return highlight;
}




























int
main( const int argc, const char **argv )
{
    MYSQL *connection = NULL;

    if (init_api_ncurses())
    {
        return EXIT_FAILURE;
    }
    
    if (argc == 1)
    {
        connection = init_api_mysql(get_db_name_from_user());
    }
    else
    {
        connection = init_api_mysql(argv[ 1 ]);
    }


    bool admin = 0;
    bool state = RUNNING;

    switch (screen_sign_in())
    {
        case 0: state = RUNNING; break;
        case 1: state = STOP;    break;
        default: break;
    }
    
    while (state)
    {
        admin = is_admin();

        do
        {
            if (admin)
            {
                switch (screen_menu_admin())
                {
                    case 0:  break;
                    case 1:  break;
                    case 2:  break;
                    case 3:  break;
                    case 4:  break;
                    case 5: state = STOP; break;
                    default: break;
                }
            }
            else
            {
                switch (screen_menu_user())
                {
                    case 0: 
                     break;
                    case 1:  break;
                    case 2:  break;
                    case 3:  break;
                    case 4: state = STOP; break;
                    default: break;
                }
            }
        }
        while(state);

        switch (screen_sign_in())
        {
            case 0: state = RUNNING; break;
            case 1: state = STOP;    break;
            default: break;
        }
    }

    close_api_mysql(connection);
    endwin();

    return EXIT_SUCCESS;
}
