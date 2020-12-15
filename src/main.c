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

#define RUNNING true
#define STOP    false

#define PADDING 1
#define MARGIN  3



const char *k_yes_no[ ] = {
    "No",
    "Yes"
};
const int k_yes_no_size   = sizeof(k_yes_no) / sizeof(char *);
const int k_yes_no_height = k_yes_no_size + PADDING * 2;
const int k_yes_no_width  = 4 + MARGIN * 2;

const char *k_menu_sign_in[ ] = {
    "Sign [I]n",
    "[E]xit"
};
const int k_menu_sign_in_size   = sizeof(k_menu_sign_in) / sizeof(char *);
const int k_menu_sign_in_height = k_menu_sign_in_size + PADDING * 2;
const int k_menu_sign_in_width  = 20 + MARGIN * 2;

const char *k_menu_admin[ ] = {
    "Register [N]ew Car",
    "[S]ell",
    "[T]rade",
    "Schedule [R]evision",
    "[M]anagement",
    "Sign [O]ut"
};
const int k_menu_admin_size   = sizeof(k_menu_admin) / sizeof( char * );
const int k_menu_admin_height = k_menu_admin_size + PADDING * 2;
const int k_menu_admin_width  = 21 + MARGIN * 2;

const char *k_menu_admin_management[ ] = {
    "New [S]ector",
    "New [F]unctionary",
    "[B]ack"
};
const int k_menu_admin_management_size   = sizeof(k_menu_admin_management) / sizeof(char *);
const int k_menu_admin_management_height = k_menu_admin_management_size + PADDING * 2;
const int k_menu_admin_management_width  = 20 + MARGIN * 2;

const char *k_menu_user[ ] = {
    "Register [N]ew Car",
    "[S]ell",
    "[T]rade",
    "Schedule [R]evision",
    "Sign [O]ut"
};
const int k_menu_user_size   = sizeof(k_menu_user) / sizeof(char *);
const int k_menu_user_height = k_menu_user_size + PADDING * 2;
const int k_menu_user_width  = 21 + MARGIN * 2;

const char k_program[ ] = {
    " Car Dealer "
};
const int k_program_size = sizeof(k_program) / sizeof(char);
const char k_creator[ ] = {
    " Vinicius F. Aguiar "
};
const int k_creator_size = sizeof(k_creator) / sizeof(char);

const int k_width_min = 20 + MARGIN * 2;

const char k_database_name_error[ ] = {
    "No database was given."
};
const int k_database_name_error_size = sizeof(k_database_name_error) / sizeof(char);

const char k_question_admin[ ] = {
    "Are you an Admin?"
};
const int k_question_admin_size = sizeof(k_question_admin) / sizeof(char);

const char k_question_data[ ] = {
    "Are you sure the data is correct?"
};
const int k_question_data_size = sizeof(k_question_data) / sizeof(char);

const char k_question_new_car[ ] = {
    "Is the car new?"
};
const int k_question_new_car_size = sizeof(k_question_new_car) / sizeof(char);

const char k_fatal_error[ ] = {
    "\033[31;1mfatal error:\033[0;0m"
};



MYSQL *init_api_mysql( const char *database_name );
inline void error_api_mysql( MYSQL *connection );
void close_api_mysql( MYSQL *connection );
int init_api_ncurses( void );
void error_api_ncurses( void );
void close_api_ncurses( void );

inline bool is_admin( void );
char get_yn_from_user( const char *question );
char get_char_from_user( void );



/******************************************************************************
 ****                            APIs HANDLERS                             ****
 ******************************************************************************/



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



inline void
error_api_mysql( MYSQL *connection )
{
    fprintf(stderr, "%s\n", mysql_error(connection));
}



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
        error_api_ncurses();
        return EXIT_FAILURE;
    }

    initscr();
    cbreak();
    // raw();
    curs_set(0);
    start_color();
    init_pair(1, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(2, COLOR_BLACK, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);
    init_pair(4, COLOR_RED, COLOR_WHITE);
    init_pair(5, 8, COLOR_BLACK);
    init_pair(6, COLOR_WHITE, COLOR_BLACK);
    init_pair(7, COLOR_RED, COLOR_BLACK);

    return EXIT_SUCCESS;
}



inline void
error_api_ncurses( void )
{
    fprintf(stderr, "api: ncurses: %s your terminal does not support colors\n", k_fatal_error);
}



void
close_api_ncurses( void )
{
    endwin();
}



/******************************************************************************
 ****                                                                      ****
 ******************************************************************************/



inline bool is_admin( void ) { return get_yn_from_user(k_question_admin) == 'Y'; }



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

    WINDOW *shadow  = newwin(height + 2, width + 2, start_y + 1, start_x + 1);
    WINDOW *border  = newwin(height + 2, width + 2, start_y    , start_x);
    WINDOW *content = newwin(height    , width    , start_y + 1, start_x + 1);
    
    bkgd(COLOR_PAIR(1)); 
    wbkgd(shadow, COLOR_PAIR(2));
    wbkgd(border , COLOR_PAIR(3));
    box(border, 0, 0);
    wattron(border, COLOR_PAIR(4));
    mvwprintw(border, 0         , (width + 2 - k_program_size) / 2, k_program);
    mvwprintw(border, height + 1,  width     - k_creator_size     , k_creator);
    wattroff(border, COLOR_PAIR(4));
    wbkgd(content, COLOR_PAIR(3));
    
    refresh(); 
    wrefresh(shadow);
    wrefresh(border);
    wrefresh(content);

    return content;
}



bool
screen_yes_no( const char *question, const int question_size )
{
    const int width     = question_size + MARGIN * 2;
    const int height    = 5;
    const int menu_size = 2;

    int  highlight = 0;
    bool status    = RUNNING;

    while (status)
    {
        WINDOW *content = create_basic_layout(height, width);
        keypad(content, true);
        mvwprintw(content, 1, MARGIN, question);

        for (int i = 0; i < menu_size; ++i)
        {
            if (i == highlight)
            {
                wattron(content, COLOR_PAIR(4));
            }
            mvwprintw(content, 3, 5 * i + (width - 10) / 2, k_yes_no[ i ]);
            wattroff(content, COLOR_PAIR(4));
        }
        wrefresh(content);

        switch (wgetch(content))
        {
            case KEY_LEFT:
                if (highlight > 0) highlight--;
                break;
            case KEY_RIGHT:
                if (highlight < menu_size - 1) highlight++;
                break;
            case 'N': case 'n': highlight = 0; break;
            case 'Y': case 'y': highlight = 1; break;
            case '\n':          status = STOP; break;
            default: break;
        }
    }

    return highlight;
}



int
screen_sign_in( void )
{
    int  highlight = 0;
    bool status    = RUNNING;

    const int width     = k_menu_sign_in_width;
    const int height    = k_menu_sign_in_height;
    const int menu_size = k_menu_sign_in_size;

    while (status)
    {
        WINDOW *content = create_basic_layout(height, width);
        keypad(content, true);

        for (int i = 0; i < menu_size; ++i)
        {
            if (i == highlight)
            {
                wattron(content, COLOR_PAIR(4));
            }
            mvwprintw(content, PADDING + i, MARGIN, k_menu_sign_in[ i ]);
            wattroff(content, COLOR_PAIR(4));
        }
        
        switch (wgetch(content))
        {
            case KEY_UP:
                if (highlight > 0) highlight--;
                break;
            case KEY_DOWN:
                if (highlight < menu_size - 1) highlight++;
                break;
            case 'I': case 'i': highlight = 0; break;
            case 'E': case 'e': highlight = 1; break;
            case KEY_ENTER:     status = STOP; break;
            default: break;
        }
    }

    return highlight;
}



int
screen_menu_admin( void )
{
    int  highlight = 0;
    bool status    = RUNNING;

    while (status)
    {
        WINDOW *content = create_basic_layout(k_menu_admin_height, k_menu_admin_width);
        keypad(content, true);

        for (int i = 0; i < k_menu_admin_size; ++i)
        {
            if (i == highlight)
            {
                wattron(content, COLOR_PAIR(4));
            }
            mvwprintw(content, PADDING + i, MARGIN, k_menu_admin[ i ]);
            wattroff(content, COLOR_PAIR(4));
        }

        switch (wgetch(content))
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
    int  highlight = 0;
    bool status    = RUNNING;

    while (status)
    {
        WINDOW *content = create_basic_layout(k_menu_user_height, k_menu_user_width);
        keypad(content, true);

        for (int i = 0; i < k_menu_user_size; ++i)
        {
            if (i == highlight)
            {
                wattron(content, COLOR_PAIR(4));
            }
            mvwprintw(content, PADDING + i, MARGIN, k_menu_user[ i ]);
            wattroff(content, COLOR_PAIR(4));
        }

        switch (wgetch(content))
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
screen_new_car( void )
{
    char  brand[ 31 ];
    char  model[ 31 ];
    char  plate[ 8 ];
    bool  is_new;
    short year;

    do
    {
        WINDOW *content = create_basic_layout(12, 40);
        keypad(content, true);

        mvwprintw(content, 1, (41 - 20) / 2, "Registering New Car");

        mvwprintw(content,  3,  4, "Brand");
        mvwprintw(content,  6,  4, "Model");
        mvwprintw(content,  9,  4, "Year");
        mvwprintw(content,  9, 13, "Plate");
        wattron(content, COLOR_PAIR(5));
        mvwprintw(content,  4,  4, "                                ");
        mvwprintw(content,  7,  4, "                                ");
        mvwprintw(content, 10,  4, " 1234 ");
        mvwprintw(content, 10, 13, " ABC-1234 ");
        wattroff(content, COLOR_PAIR(5));
        wrefresh(content);

        curs_set(1);
        wattron(content, COLOR_PAIR(6));
        mvwscanw(content,  4,  5, " %s", brand);
        mvwscanw(content,  7,  5, " %s", model);
        mvwscanw(content, 10,  5, " %d", &year);
        mvwscanw(content, 10, 14, " %s", plate);
        wattroff(content, COLOR_PAIR(6));
        curs_set(0);
        wrefresh(content);

        is_new = screen_yes_no(k_question_new_car, k_question_new_car_size);
    }
    while (!screen_yes_no(k_question_data, k_question_data_size));

    //Fazer a query aqui

    return EXIT_SUCCESS;
}



/******************************************************************************
 ****                                 MAIN                                 ****
 ******************************************************************************/



int
main( const int argc, const char **argv )
{
    if (argc == 1)          return EXIT_FAILURE;
    if (init_api_ncurses()) return EXIT_FAILURE;

    MYSQL *connection = init_api_mysql(argv[ 1 ]);
    if (connection == NULL)
    {
        return EXIT_FAILURE;
    }

    bool   logged_in  = STOP;
    bool   state      = RUNNING;
    
    while (state)
    {
        bool admin = false;
        
        switch (screen_sign_in())
        {
            case 0:
                logged_in = true;
                admin = screen_yes_no( k_question_admin, k_question_admin_size );
                break;
            case 1:
                state = STOP;
                break;
            default: break;
        }

        while(logged_in)
        {
            int error = 0;

            if (admin)
            {
                switch (screen_menu_admin())
                {
                    case 0: error = screen_new_car(); break;
                    case 1:  break;
                    case 2:  break;
                    case 3:  break;
                    case 4:  break;
                    case 5: logged_in = false; break;
                    default: break;
                }

                if (error)
                {}
            }
            else
            {
                switch (screen_menu_user())
                {
                    case 0: error = screen_new_car(); break;
                    case 1:  break;
                    case 2:  break;
                    case 3:  break;
                    case 4: logged_in = false; break;
                    default: break;
                }

                if (error)
                {}
            }
        }
    }

    close_api_mysql(connection);
    close_api_ncurses();

    return EXIT_SUCCESS;
}
