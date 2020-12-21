/**
 * @author Vinícius Ferreira Aguiar (acevinicius@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-12-06
 *
 * @copyright Copyright (c) 2020
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include <mysql/mysql.h>

#include "../lib/headers/main.h"



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



inline void error_api_ncurses( void ) { fprintf(stderr, "api: ncurses: %s your terminal does not support colors\n", k_fatal_error); }



inline void close_api_ncurses( void ) { endwin(); }



/******************************************************************************
 ****                                 MAIN                                 ****
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
    if (shadow == NULL)
    {
        return NULL;
    }

    WINDOW *border  = newwin(height + 2, width + 2, start_y    , start_x);
    if (shadow == NULL)
    {
        free(shadow);
        return NULL;
    }

    WINDOW *content = newwin(height    , width    , start_y + 1, start_x + 1);
    if (shadow == NULL)
    {
        free(shadow);
        free(border);
        return NULL;
    }

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



short
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
        if (content ==  NULL)
        {
            return -1; 
        }

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
        if (content == NULL)
        {
            return EXIT_FAILURE;
        }

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
            case '\n':          status = STOP; break;
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
        if (content == NULL)
        {
            return EXIT_FAILURE;
        }
        
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
        if (content == NULL)
        {
            return EXIT_FAILURE;
        }
        
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
    char  plate[ 9 ];
    bool  is_new;
    short year;

    do
    {
        WINDOW *content = create_basic_layout(12, 40);
        if (content == NULL)
        {
            return EXIT_FAILURE;
        }

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
        mvwscanw(content,   4,  5, " %30s", brand);
        mvwscanw(content,   7,  5, " %30s", model);
        mvwprintw(content, 10,  4, "      ");
        wrefresh(content);
        mvwscanw(content,  10,  5,  " %4d", &year);
        mvwprintw(content, 10, 13, "          ");
        wrefresh(content);
        mvwscanw(content, 10, 14, " %8s", plate);
        wattroff(content, COLOR_PAIR(6));
        curs_set(0);
        wrefresh(content);

        // if all the fields get an overflow, the model will not print anything
        
        is_new = screen_yes_no(k_question_new_car, k_question_new_car_size);
    }
    while (!screen_yes_no(k_question_data, k_question_data_size));

    // Fazer a query aqui

    return EXIT_SUCCESS;
}



int
screen_sell( void )
{
    char  brand[ 31 ] = "*";
    char  model[ 31 ] = "*";
    char  plate[ 8 ] = "*";
    short year = -1;

    WINDOW *content = create_basic_layout(12, 40);
    if (content == NULL)
    {
        return EXIT_FAILURE;
    }
    
    keypad(content, true);

    mvwprintw(content, 1, (41 - 4) / 2, "Sell");

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
    mvwscanw(content,  4,  5, " %30s%*s", brand);
    mvwscanw(content,  7,  5, " %30s%*s", model);
    mvwprintw(content, 10,  4, "      ");
    wrefresh(content);
    mvwscanw(content, 10,  5, " %4d%*d", &year);
    mvwprintw(content, 10, 13, "          ");
    wrefresh(content);
    mvwscanw(content, 10, 14, " %8s%*s", plate);
    wattroff(content, COLOR_PAIR(6));
    wrefresh(content);
    curs_set(0);

    bool is_new = screen_yes_no(k_question_new_car, k_question_new_car_size);

    // if all the fields get an overflow, the model will not print anything

    // Fazer a query aqui

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

    bool logged_in = false;
    bool state     = RUNNING;

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
                    case 0: error = screen_new_car();    break;
                    case 1: error = screen_sell();       break;
                    case 2: /* error = screen_trade(); */     break;
                    case 3: /* error = screen_revision(); */  break;
                    case 4: /* error = screen_management(); */ break;
                    case 5: logged_in = false;           break;
                    default: break;
                }

                if (error)
                {
                    
                }
            }
            else
            {
                switch (screen_menu_user())
                {
                    case 0: error = screen_new_car();  break;
                    case 1: error = screen_sell();     break;
                    case 2: /* error = screen_trade(); */   break;
                    case 3: /* error = screen_revision(); */ break;
                    case 4: /* logged_in = false; */        break;
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
