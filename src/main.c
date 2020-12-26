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
#include <string.h>
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
    init_pair(2, COLOR_BLACK  , COLOR_BLACK);
    init_pair(3, COLOR_BLACK  , COLOR_WHITE);
    init_pair(4, COLOR_RED    , COLOR_WHITE);
    init_pair(5, 8            , COLOR_BLACK);
    init_pair(6, COLOR_WHITE  , COLOR_BLACK);
    init_pair(7, COLOR_RED    , COLOR_BLACK);

    return EXIT_SUCCESS;
}



inline void error_api_ncurses( void ) { fprintf(stderr, "api: ncurses: %s your terminal does not support colors\n", k_fatal_error); }



inline void close_api_ncurses( void ) { endwin(); }



/******************************************************************************
 ****                                 GUI                                  ****
 ******************************************************************************/



WINDOW *
create_basic_layout( const int min_y ,
                     const int min_x )
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

    WINDOW *border  = newwin(height + 2, width + 2, start_y, start_x);
    if (shadow == NULL)
    {
        free(shadow);
        return NULL;
    }

    WINDOW *content = newwin(height, width, start_y + 1, start_x + 1);
    if (shadow == NULL)
    {
        free(shadow);
        free(border);
        return NULL;
    }

    bkgd(COLOR_PAIR(1));
    
    wbkgd(shadow, COLOR_PAIR(2));
    
    wbkgd(border, COLOR_PAIR(3));
    box(border, 0, 0);
    wattron(border, COLOR_PAIR(4));
    mvwprintw(border, 0, (width + 2 - k_program_size) / 2, k_program);
    mvwprintw(border, height + 1, width - k_creator_size, k_creator);
    wattroff(border, COLOR_PAIR(4));
    
    wbkgd(content, COLOR_PAIR(3));

    // Do not change this order
    refresh();
    wrefresh(shadow);
    wrefresh(border);
    wrefresh(content);

    return content;
}



void
template_print_car_info(       WINDOW *content ,
                         const char   *title   ,
                         const int     start_x ,
                         const int     start_y )
{
    const int size = strlen(title);
    const int width  = k_car_width  * start_x;
    const int height = k_car_height * start_y;

    // Title
    mvwprintw(content, height +  1, width + (k_car_width - size) / 2, title);

    // Field Names
    mvwprintw(content, height +  3, width +  4, "Brand");
    mvwprintw(content, height +  6, width +  4, "Model");
    mvwprintw(content, height +  9, width +  4, "Year");
    mvwprintw(content, height +  9, width + 13, "Plate");

    // Placeholders
    wattron(content, COLOR_PAIR(5));
    mvwprintw(content, height +  4, width +  4, " Ford                           ");
    mvwprintw(content, height +  7, width +  4, " Mustang GT                     ");
    mvwprintw(content, height + 10, width +  4, " 2018 ");
    mvwprintw(content, height + 10, width + 13, " ABC-1234 ");
    wattroff(content, COLOR_PAIR(5));

    wrefresh(content);
}



void
template_get_car_info(       WINDOW *content ,
                       const int     start_x ,
                       const int     start_y ,
                             char   *brand   ,
                             char   *model   ,
                             short  *year    ,
                             char   *plate   )
{
    const int width  = k_car_width  * start_x;
    const int height = k_car_height * start_y;

    curs_set(1);
    wattron(content, COLOR_PAIR(6));

    // If all the fields get an overflow, the model will not print anything
    mvwprintw(content, height +  4, width +  4, "                                ");
    wrefresh(content);
    mvwscanw(content,  height +  4, width +  5, " %30s%*s", brand);
    
    mvwprintw(content, height +  7, width +  4, "                                ");
    wrefresh(content);
    mvwscanw(content,  height +  7, width +  5, " %30s%*s", model);
    
    mvwprintw(content, height + 10, width +  4, "      ");
    wrefresh(content);
    mvwscanw(content,  height + 10, width +  5, " %4d%*d", &year);
    
    mvwprintw(content, height + 10, width + 13, "          ");
    wrefresh(content);
    mvwscanw(content,  height + 10, width + 14, " %8s%*s", plate);

    wattroff(content, COLOR_PAIR(6));
    curs_set(0);
    wrefresh(content);
}



void
template_print_client_info(       WINDOW *content ,
                            const char   *title   ,
                            const int     start_x ,
                            const int     start_y )
{
    const int size   = strlen(title);
    const int width  = k_client_width  * start_x;
    const int height = k_client_height * start_y;

    // Title
    mvwprintw(content, height +  1, width + (k_client_width - size) / 2, title);

    // Field Names
    mvwprintw(content, height +  3, width +  4, "CPF");
    mvwprintw(content, height +  3, width + 22, "Employee CPF");
    mvwprintw(content, height +  3, width + 40, "Telephone");
    mvwprintw(content, height +  6, width +  4, "Name");
    mvwprintw(content, height +  9, width +  4, "Adress");

    // Placeholders
    wattron(content, COLOR_PAIR(5));
    mvwprintw(content, height +  4, width +  4, " 123.456.789-01 ");
    mvwprintw(content, height +  4, width + 22, " 123.456.789-01 ");
    mvwprintw(content, height +  4, width + 40, " (12) 34567-8901 ");
    mvwprintw(content, height +  7, width +  4, " Vinícius Ferreira Aguiar                            ");
    mvwprintw(content, height + 10, width +  4, " Rua Jorge Emilio Fontenelle, 110, apartamento 202   ");
    wattroff(content, COLOR_PAIR(5));

    wrefresh(content);
}



void
template_get_client_info(       WINDOW *content      ,
                          const int     start_x      ,
                          const int     start_y      ,
                                char   *client_cpf   ,
                                char   *employee_cpf ,
                                char   *telephone    ,
                                char   *name         ,
                                char   *adress       )
{
    const int width  = k_client_width  * start_x;
    const int height = k_client_height * start_y;

    curs_set(1);
    wattron(content, COLOR_PAIR(6));

    mvwprintw(content, height +  4, width +  4, "                ");
    wrefresh(content);
    mvwscanw(content,  height +  4, width +  5, " %14s%*s", client_cpf);

    mvwprintw(content, height +  4, width + 22, "                ");
    wrefresh(content);
    mvwscanw(content,  height +  4, width + 23, " %14s%*s", employee_cpf);

    mvwprintw(content, height +  4, width + 40, "                 ");
    wrefresh(content);
    mvwscanw(content,  height +  4, width + 41, " %15d%*d", telephone);

    mvwprintw(content, height +  7, width +  4, "                                                     ");
    wrefresh(content);
    mvwscanw(content,  height +  7, width +  5, " %51s%*s", name);

    mvwprintw(content, height + 10, width +  4, "                                                     ");
    wrefresh(content);
    mvwscanw(content,  height + 10, width +  5, " %51s%*s", adress);

    wattroff(content, COLOR_PAIR(6));
    curs_set(0);
    wrefresh(content);
}



void
template_print_employee_info(       WINDOW *content ,
                              const char   *title   ,
                              const int     start_x ,
                              const int     start_y )
{
    const int size = strlen(title);
    const int width  = k_employee_width  * start_x;
    const int height = k_employee_height * start_y;

    // Title
    mvwprintw(content, height +  1, width + (k_employee_width - size) / 2, title);

    // Field Names
    mvwprintw(content, height +  3, width +  4, "CPF");
    mvwprintw(content, height +  3, width + 22, "Salary");
    mvwprintw(content, height +  3, width + 40, "Sector Code");
    mvwprintw(content, height +  6, width +  4, "Name");
    mvwprintw(content, height +  9, width +  4, "Adress");

    // Placeholders
    wattron(content, COLOR_PAIR(5));
    mvwprintw(content, height +  4, width +  4, " 123.456.789-01 ");
    mvwprintw(content, height +  4, width + 22, " R$ 123456,78   ");
    mvwprintw(content, height +  4, width + 40, " 012345          ");
    mvwprintw(content, height +  7, width +  4, " Vinícius Ferreira Aguiar                            ");
    mvwprintw(content, height + 10, width +  4, " Rua Jorge Emilio Fontenelle, 110, apartamento 202   ");
    wattroff(content, COLOR_PAIR(5));

    wrefresh(content);
}



void
template_get_employee_info(       WINDOW *content     ,
                            const int     start_x     ,
                            const int     start_y     ,
                                  char   *cpf         ,
                                  double *salary      ,
                                  int    *sector_code ,
                                  char   *name        ,
                                  char   *adress      )
{
    const int width  = k_employee_width  * start_x;
    const int height = k_employee_height * start_y;

    curs_set(1);
    wattron(content, COLOR_PAIR(6));

    mvwprintw(content, height +  4, width +  4, "                ");
    wrefresh(content);
    mvwscanw(content,  height +  4, width +  5, " %14s%*s", cpf);

    mvwprintw(content, height +  4, width + 22, "                ");
    wrefresh(content);
    mvwscanw(content,  height +  4, width + 23, " %14lf%*s", salary);

    mvwprintw(content, height +  4, width + 40, "                 ");
    wrefresh(content);
    mvwscanw(content,  height +  4, width + 41, " %15d%*d", sector_code);

    mvwprintw(content, height +  7, width +  4, "                                                     ");
    wrefresh(content);
    mvwscanw(content,  height +  7, width +  5, " %51s%*s", name);

    mvwprintw(content, height + 10, width +  4, "                                                     ");
    wrefresh(content);
    mvwscanw(content,  height + 10, width +  5, " %51s%*s", adress);

    wattroff(content, COLOR_PAIR(6));
    curs_set(0);
    wrefresh(content);
}



short
screen_yes_no( const char *question      ,
               const int   question_size )
{
    const int width  = question_size + MARGIN * 2;
    const int height = 5;
    const int size   = 2;

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

        for (int i = 0; i < size; ++i)
        {
            if (i == highlight)
            {
                wattron(content, COLOR_PAIR(4));
            }
            mvwprintw(content, 3, 6 * i + (width - 11) / 2, k_yes_no[ i ]);
            wattroff(content, COLOR_PAIR(4));
        }
        wrefresh(content);

        switch (wgetch(content))
        {
            case KEY_LEFT:
                if (highlight > 0) highlight--;
                break;
            case KEY_RIGHT:
                if (highlight < size - 1) highlight++;
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
screen_menu( const char **menu ,
             const int    size )
{
    const int height = size + PADDING * 2;
    const int width  = 22 + MARGIN * 2;

    int  highlight = 0;
    bool status    = RUNNING;

    while (status)
    {
        WINDOW *content = create_basic_layout(height, width);
        if (content == NULL)
        {
            return -1;
        }
        
        keypad(content, true);

        for (int i = 0; i < size; ++i)
        {
            if (i == highlight)
            {
                wattron(content, COLOR_PAIR(4));
            }
            mvwprintw(content, PADDING + i, MARGIN, menu[ i ]);
            wattroff(content, COLOR_PAIR(4));
        }

        switch (wgetch(content))
        {
            case KEY_UP:   if (highlight > 0) highlight--;        break;
            case KEY_DOWN: if (highlight < size - 1) highlight++; break;
            case '\n':     status = STOP;                         break;
            default: break;
        }
    }

    return highlight;
}



int
screen_new_client( void )
{
    char employee_cpf[ 15 ];
    char client_cpf[ 15 ];
    char telephone[ 16 ];
    char adress[ 52 ];
    char name[ 52 ];

    do
    {
        WINDOW *content = create_basic_layout(k_client_height, k_client_width);
        if (content == NULL)
        {
            return EXIT_FAILURE;
        }

        keypad(content, true);

        template_print_client_info(content, "Register New Client", 0, 0);
        template_get_client_info(content, 0, 0, client_cpf, employee_cpf, telephone, name, adress);
    }
    while (!screen_yes_no(k_question_data, k_question_data_size));

    // Fazer a query aqui

    return EXIT_SUCCESS;
}



int
screen_new_employee( void )
{
    char   cpf[ 15 ];
    double salary;
    int    sector_code;
    char   adress[ 52 ];
    char   name[ 52 ];

    do
    {
        WINDOW *content = create_basic_layout(k_client_height, k_client_width);
        if (content == NULL)
        {
            return EXIT_FAILURE;
        }

        keypad(content, true);

        template_print_employee_info(content, "Register New Client", 0, 0);
        template_get_employee_info(content, 0, 0, cpf, &salary, &sector_code, name, adress);
    }
    while (!screen_yes_no(k_question_data, k_question_data_size));

    // Fazer a query aqui

    return EXIT_SUCCESS;
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
        WINDOW *content = create_basic_layout(k_car_height, k_car_width);
        if (content == NULL)
        {
            return EXIT_FAILURE;
        }

        keypad(content, true);

        template_print_car_info(content, "Registering New Car", 0, 0);
        template_get_car_info(content, 0, 0, brand, model, &year, plate);
        
        is_new = screen_yes_no(k_question_new_car, k_question_new_car_size);
    }
    while (!screen_yes_no(k_question_data, k_question_data_size));

    // Fazer a query aqui

    return EXIT_SUCCESS;
}



int
screen_sell( void )
{
    WINDOW *content = create_basic_layout(k_car_height, k_car_width);
    if (content == NULL)
    {
        return EXIT_FAILURE;
    }
    
    char  brand[ 31 ] = "*";
    char  model[ 31 ] = "*";
    char  plate[ 8 ]  = "*";
    short year = -1;

    keypad(content, true);

    template_print_car_info(content, "Sell", 0, 0);
    template_get_car_info(content, 0, 0, brand, model, &year, plate);

    bool is_new = screen_yes_no(k_question_new_car, k_question_new_car_size);

    // Fazer a query aqui

    return EXIT_SUCCESS;
}



int
screen_trade( void )
{
    WINDOW *content = create_basic_layout(k_car_height, 2 * k_car_width);
    if (content == NULL)
    {
        return EXIT_FAILURE;
    }

    char  brand_client[ 31 ] = "*";
    char  model_client[ 31 ] = "*";
    char  plate_client[ 8 ]  = "*";
    short year_client = -1;

    char  brand_dealer[ 31 ] = "*";
    char  model_dealer[ 31 ] = "*";
    char  plate_dealer[ 8 ]  = "*";
    short year_dealer = -1;

    keypad(content, true);

    template_print_car_info(content, "Client", 0, 0);
    template_print_car_info(content, "Dealer", 1, 0);
    template_get_car_info(content, 0, 0, brand_client, model_client, &year_client, plate_client);
    template_get_car_info(content, 1, 0, brand_dealer, model_dealer, &year_dealer, plate_dealer);

    bool is_new_dealer = screen_yes_no(k_question_new_car, k_question_new_car_size);

    // Fazer a query aqui

    return EXIT_SUCCESS;
}



int
screen_revision( void )
{
    WINDOW *content = create_basic_layout(12, 40);
    if (content == NULL)
    {
        return EXIT_FAILURE;
    }

    char  plate_client[ 8 ] = "*";

    



    // Fazer a query aqui

    return EXIT_SUCCESS;
}



int
screen_management( void )
{
    bool state = RUNNING;
    bool error = false;

    while (state)
    {
        switch (screen_menu(k_menu_management, k_menu_management_size))
        {
            case 0: error = screen_new_employee(); break;
            case 1: /* error = screen_new_sector(); */  break;
            case 2: /* error = screen_bonus(); */      break;
            case 3: state = STOP;                  break;
            default: break;
        }

        if (error) break;
    }

    return error;
}



/******************************************************************************
 ****                                 MAIN                                 ****
 ******************************************************************************/



int
main( const int    argc ,
      const char **argv )
{
    if (init_api_ncurses()) return EXIT_FAILURE;
    
    if (argc == 1)
    {
        fprintf(stderr, "%s %s", k_fatal_error, k_database_name_error);
        return EXIT_FAILURE;
    }

    MYSQL *connection = init_api_mysql(argv[ 1 ]);
    if (connection == NULL)
    {
        return EXIT_FAILURE;
    }

    bool logged_in = false;
    bool state     = RUNNING;

    while (state)
    {
        short admin = false;

        switch (screen_menu(k_menu_sign_in, k_menu_sign_in_size))
        {
            case 0:
                logged_in = true;
                admin = screen_yes_no(k_question_admin, k_question_admin_size);
                break;
            case 1:
                state = STOP;
                break;
            default: break;
        }

        while(logged_in)
        {
            bool error = false;

            if (admin)
            {
                switch (screen_menu(k_menu_admin, k_menu_admin_size))
                {
                    case 0: error = screen_new_client(); break;
                    case 1: error = screen_new_car();    break;
                    case 2: error = screen_sell();       break;
                    case 3: error = screen_trade();      break;
                    case 4: error = screen_revision();   break;
                    case 5: error = screen_management(); break;
                    case 6: logged_in = false;           break;
                    default: break;
                }
            }
            else
            {
                switch (screen_menu(k_menu_user, k_menu_user_size))
                {
                    case 0: error = screen_new_client(); break;
                    case 1: error = screen_new_car();    break;
                    case 2: error = screen_sell();       break;
                    case 3: error = screen_trade();      break;
                    case 4: error = screen_revision();   break;
                    case 5: logged_in = false;           break;
                    default: break;
                }
            }

            if (error)
            {
                logged_in = false;
                state = STOP;
                fprintf(stderr, "%s Something went wrong with ncurses, please run the program again", k_fatal_error);
            }
        }
    }

    close_api_mysql(connection);
    close_api_ncurses();

    return EXIT_SUCCESS;
}
