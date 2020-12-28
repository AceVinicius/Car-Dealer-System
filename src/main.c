/**
 * @author Vinícius Ferreira Aguiar (acevinicius@outlook.com)
 * @brief
 * @version 0.1
 * @date 2020-12-06
 *
 * @copyright Copyright (c) 2020
 *
 */
#include <time.h>
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

    if (mysql_real_connect(new_connection, "localhost", "root", "Appetite#1987", database_name, 0, NULL, 0) == NULL)
    {
        error_api_mysql(new_connection);
        close_api_mysql(new_connection);
        return NULL;
    }

    return new_connection;
}



int
make_query( MYSQL *connection, const char *query )
{
    if (mysql_query(connection, query))
    {
        error_api_mysql(connection);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}



inline void
error_api_mysql( MYSQL *connection )
{
    FILE *file = fopen("error_output.txt", "a");
    fprintf(file, "%s\n", mysql_error(connection));
    fclose(file);
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
    init_pair(2, COLOR_BLACK  , COLOR_BLACK);
    init_pair(3, COLOR_BLACK  , COLOR_WHITE);
    init_pair(4, COLOR_RED    , COLOR_WHITE);
    init_pair(5, 8            , COLOR_BLACK);
    init_pair(6, COLOR_WHITE  , COLOR_BLACK);
    init_pair(7, COLOR_RED    , COLOR_BLACK);

    return EXIT_SUCCESS;
}



inline void
error_api_ncurses( void )
{
    FILE *file = fopen("error_output.txt", "a");
    fprintf(file, "api: ncurses: %s your terminal does not support colors\n", k_fatal_error);
    fclose(file);
}



inline void close_api_ncurses( void ) { endwin(); }



/******************************************************************************
 ****                              Templates                               ****
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
    const int size   = strlen(title);
    const int width  = k_car_width  * start_x;
    const int height = k_car_height * start_y;

    // Title
    mvwprintw(content, height +  1, width + (k_car_width - size) / 2, title);

    // Field Names
    mvwprintw(content, height +   3, width +  4, "Brand");
    mvwprintw(content, height +   6, width +  4, "Model");
    mvwprintw(content, height +   9, width +  4, "Cost");
    mvwprintw(content, height +   9, width + 22, "Value");
    mvwprintw(content, height +  12, width +  4, "Year");
    mvwprintw(content, height +  12, width + 13, "Chassis");

    // Placeholders
    wattron(content, COLOR_PAIR(5));
    mvwprintw(content, height +  4, width +  4, " Ford                             ");
    mvwprintw(content, height +  7, width +  4, " Mustang GT                       ");
    mvwprintw(content, height + 10, width +  4, " R$ 1234567.89 ");
    mvwprintw(content, height + 10, width + 22, " R$ 1234567.89 ");
    mvwprintw(content, height + 13, width +  4, " 2018 ");
    mvwprintw(content, height + 13, width + 13, " 0ABCD12E34F567890 ");
    wattroff(content, COLOR_PAIR(5));

    wrefresh(content);
}



void
template_get_car_info(       WINDOW *content ,
                             CAR    *car     ,
                       const int     start_x ,
                       const int     start_y )
{
    const int width  = k_car_width  * start_x;
    const int height = k_car_height * start_y;

    curs_set(1);
    wattron(content, COLOR_PAIR(6));

    mvwprintw(content, height +  4, width +  4, "                                  ");
    wrefresh(content);
    mvwscanw(content,  height +  4, width +  5, " %32s", car->brand);
    
    mvwprintw(content, height +  7, width +  4, "                                  ");
    wrefresh(content);
    mvwscanw(content,  height +  7, width +  5, " %32s", car->model);
    
    mvwprintw(content, height + 10, width +  4, " R$            ");
    wrefresh(content);
    mvwscanw(content,  height + 10, width +  8, " %10d", &car->cost_value);

    mvwprintw(content, height + 10, width +  22, " R$            ");
    wrefresh(content);
    mvwscanw(content,  height + 10, width +  26, " %10d", &car->sell_value);

    mvwprintw(content, height + 13, width +  4, "      ");
    wrefresh(content);
    mvwscanw(content,  height + 13, width +  5, " %4d", &car->year);
    
    mvwprintw(content, height + 13, width + 13, "                   ");
    wrefresh(content);
    mvwscanw(content,  height + 13, width + 14, " %17s", car->chassis);

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
    mvwprintw(content, height +  3, width + 23, "Employee CPF");
    mvwprintw(content, height +  3, width + 42, "Sector");
    mvwprintw(content, height +  3, width + 50, "Telephone");
    mvwprintw(content, height +  6, width +  4, "Name");
    mvwprintw(content, height +  6, width + 40, "Plate");
    mvwprintw(content, height +  9, width +  4, "Address");

    // Placeholders
    wattron(content, COLOR_PAIR(5));
    mvwprintw(content, height +  4, width +  4, " 123.456.789-01 ");
    mvwprintw(content, height +  4, width + 23, " 123.456.789-01 ");
    mvwprintw(content, height +  4, width + 42, " 012 ");
    mvwprintw(content, height +  4, width + 50, " (12) 34567-8901 ");
    mvwprintw(content, height +  7, width +  4, " Vinícius Ferreira Aguiar        ");
    mvwprintw(content, height +  7, width + 41, " ABC-1234 ");
    mvwprintw(content, height + 10, width +  4, " Rua Jorge Emilio Fontenelle, 110, apartamento 202                ");
    wattroff(content, COLOR_PAIR(5));

    wrefresh(content);
}



void
template_get_client_info(       WINDOW *content ,
                                CLIENT *client  ,
                          const int     start_x ,
                          const int     start_y )
{
    const int width  = k_client_width  * start_x;
    const int height = k_client_height * start_y;

    curs_set(1);
    wattron(content, COLOR_PAIR(6));

    mvwprintw(content, height + 4, width + 4, "                ");
    wrefresh(content);
    mvwscanw(content,  height + 4, width + 5, " %14s", client->client_cpf);

    mvwprintw(content, height + 4, width + 23, "                ");
    wrefresh(content);
    mvwscanw(content,  height + 4, width + 24, " %14s", client->employee_cpf);

    mvwprintw(content, height + 4, width + 42, "     ");
    wrefresh(content);
    mvwscanw(content,  height + 4, width + 43, " %3d", &client->employee_sector);

    mvwprintw(content, height + 4, width + 50, "                 ");
    wrefresh(content);
    mvwscanw(content,  height + 4, width + 51, " %15s", client->telephone);

    mvwprintw(content, height + 7, width + 4, "                                  ");
    wrefresh(content);
    mvwscanw(content,  height + 7, width + 5, " %32s", client->name);

    mvwprintw(content, height + 7, width + 40, "          ");
    wrefresh(content);
    mvwscanw(content,  height + 7, width + 41, " %64s", client->plate);

    mvwprintw(content, height + 10, width + 4, "                                                                  ");
    wrefresh(content);
    mvwscanw(content,  height + 10, width + 5, " %64s", client->address);

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
    mvwprintw(content, height + 3, width +  4, "CPF");
    mvwprintw(content, height + 3, width + 23, "Salary");
    mvwprintw(content, height + 3, width + 41, "Sector");
    mvwprintw(content, height + 6, width +  4, "Name");
    mvwprintw(content, height + 9, width +  4, "Address");

    // Placeholders
    wattron(content, COLOR_PAIR(5));
    mvwprintw(content, height +  4, width +  4, " 123.456.789-01 ");
    mvwprintw(content, height +  4, width + 23, " R$ 1234567.89 ");
    mvwprintw(content, height +  4, width + 41, " 012 ");
    mvwprintw(content, height +  7, width +  4, " Vinícius Ferreira Aguiar        ");
    mvwprintw(content, height + 10, width +  4, " Rua Jorge Emilio Fontenelle, 110, apartamento 202                ");
    wattroff(content, COLOR_PAIR(5));

    wrefresh(content);
}



void
template_get_employee_info(       WINDOW   *content  ,
                                  EMPLOYEE *employee ,
                            const int       start_x  ,
                            const int       start_y  )
{
    const int width  = k_employee_width  * start_x;
    const int height = k_employee_height * start_y;

    curs_set(1);
    wattron(content, COLOR_PAIR(6));

    mvwprintw(content, height + 4, width + 4, "                ");
    wrefresh(content);
    mvwscanw(content,  height + 4, width + 5, " %14s", employee->cpf);

    mvwprintw(content, height + 4, width + 23, " R$            ");
    wrefresh(content);
    mvwscanw(content,  height + 4, width + 27, " %10lf", &employee->salary);

    mvwprintw(content, height + 4, width + 41, "     ");
    wrefresh(content);
    mvwscanw(content,  height + 4, width + 42, " %3d", &employee->sector_code);

    mvwprintw(content, height + 7, width + 4, "                                  ");
    wrefresh(content);
    mvwscanw(content,  height + 7, width + 5, " %32s", employee->name);

    mvwprintw(content, height + 10, width + 4, "                                                                  ");
    wrefresh(content);
    mvwscanw(content,  height + 10, width + 5, " %64s", employee->address);

    wattroff(content, COLOR_PAIR(6));
    curs_set(0);
    wrefresh(content);
}



void
template_print_sector_info(       WINDOW *content ,
                            const char   *title   ,
                            const int     start_x ,
                            const int     start_y )
{
    const int size = strlen(title);
    const int width  = k_sector_width  * start_x;
    const int height = k_sector_height * start_y;

    // Title
    mvwprintw(content, height +  1, width + (k_sector_width - size) / 2, title);

    // Field Names
    mvwprintw(content, height + 3, width +  4, "Agency");
    mvwprintw(content, height + 3, width + 12, "Name");

    // Placeholders
    wattron(content, COLOR_PAIR(5));
    mvwprintw(content, height + 4, width +  4, " 012 ");
    mvwprintw(content, height + 4, width + 12, " Vendas           ");
    wattroff(content, COLOR_PAIR(5));

    wrefresh(content);
}



void
template_get_sector_info(         WINDOW *content ,
                                  SECTOR *sector  ,
                            const int     start_x ,
                            const int     start_y )
{
    const int width  = k_employee_width  * start_x;
    const int height = k_employee_height * start_y;

    curs_set(1);
    wattron(content, COLOR_PAIR(6));

    mvwprintw(content, height + 4, width + 4, "     ");
    wrefresh(content);
    mvwscanw(content,  height + 4, width + 5, " %3d", &sector->agency_code);

    mvwprintw(content, height + 4, width + 12, "                  ");
    wrefresh(content);
    mvwscanw(content,  height + 4, width + 13, " %16s", sector->name);

    wattroff(content, COLOR_PAIR(6));
    curs_set(0);
    wrefresh(content);
}



/******************************************************************************
 ****                               Screens                                ****
 ******************************************************************************/



int
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
            wrefresh(content);
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
screen_new_client( MYSQL *connection )
{
    CLIENT *client = (CLIENT *) calloc(1, sizeof(CLIENT));

    client->employee_sector = -1;

    do
    {
        WINDOW *content = create_basic_layout(k_client_height, k_client_width);
        if (content == NULL)
        {
            return EXIT_FAILURE;
        }

        keypad(content, true);

        template_print_client_info(content, "Register New Client", 0, 0);
        template_get_client_info(content, client, 0, 0);
    }
    while (!screen_yes_no(k_question_data, k_question_data_size));

    if (strcmp(client->client_cpf  , "")) return EXIT_SUCCESS;
    if (strcmp(client->name        , "")) return EXIT_SUCCESS;
    if (strcmp(client->address     , "")) return EXIT_SUCCESS;
    if (strcmp(client->telephone   , "")) return EXIT_SUCCESS;
    if (strcmp(client->plate       , "")) return EXIT_SUCCESS;
    if (strcmp(client->employee_cpf, "")) return EXIT_SUCCESS;
    if (client->employee_sector == -1)    return EXIT_SUCCESS;

    char *query = (char *) calloc(512, sizeof(char));
    
    sprintf(
        query,
        "INSERT INTO cliente (cpf, nome, endereco, telefone, placa_carro, cpf_funcionario, cod_setor_origem) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', %d);",
        client->client_cpf,
        client->name,
        client->address,
        client->telephone,
        client->plate,
        client->employee_cpf,
        client->employee_sector
    );

    return make_query(connection, query);
}



int
screen_new_employee( MYSQL *connection )
{
    EMPLOYEE *employee = (EMPLOYEE *) calloc(1, sizeof(EMPLOYEE));

    employee->salary      = -1.0;
    employee->sector_code = -1;

    do
    {
        WINDOW *content = create_basic_layout(k_client_height, k_client_width);
        if (content == NULL)
        {
            return EXIT_FAILURE;
        }

        keypad(content, true);

        template_print_employee_info(content, "Register New Employee", 0, 0);
        template_get_employee_info(content, employee, 0, 0);
    }
    while (!screen_yes_no(k_question_data, k_question_data_size));

    if (strcmp(employee->cpf    , "")) return EXIT_SUCCESS;
    if (strcmp(employee->name   , "")) return EXIT_SUCCESS;
    if (strcmp(employee->address, "")) return EXIT_SUCCESS;
    if (employee->salary      == -1.0) return EXIT_SUCCESS;
    if (employee->sector_code == -1)   return EXIT_SUCCESS;

    char *query = (char *) calloc(512, sizeof(char));
    
    sprintf(
        query,
        "INSERT INTO funcionario (cpf, nome, endereco, salario, cod_setor, data_Admissao) VALUES ('%s', '%s', '%s', %2lf, %d, NOW());",
        employee->cpf,
        employee->name,
        employee->address,
        employee->salary,
        employee->sector_code
    );

    return make_query(connection, query);
}



int
screen_new_car( MYSQL *connection )
{
    CAR *car = (CAR *) calloc(1, sizeof(CAR));

    car->sell_value  = -1.0;
    car->cost_value  = -1.0;
    car->sector_code = -1;
    car->year        = -1;

    do
    {
        WINDOW *content = create_basic_layout(k_car_height, k_car_width);
        if (content == NULL)
        {
            return EXIT_FAILURE;
        }

        keypad(content, true);

        template_print_car_info(content, "Registering New Car", 0, 0);
        template_get_car_info(content, car, 0, 0);
        
        car->is_new   = screen_yes_no(k_question_new_car  , k_question_new_car_size);
        car->is_sell  = screen_yes_no(k_question_sell_car , k_question_sell_car_size);
        car->is_trade = screen_yes_no(k_question_trade_car, k_question_trade_car_size);
    }
    while (!screen_yes_no(k_question_data, k_question_data_size));

    if (strcmp(car->model  , "")) return EXIT_SUCCESS;
    if (strcmp(car->chassis, "")) return EXIT_SUCCESS;
    if (strcmp(car->brand  , "")) return EXIT_SUCCESS;
    if (car->cost_value  == -1.0) return EXIT_SUCCESS;
    if (car->sell_value  == -1.0) return EXIT_SUCCESS;
    if (car->sector_code == -1)   return EXIT_SUCCESS;
    if (car->year        == -1)   return EXIT_SUCCESS;

    char *query = (char *) calloc(512, sizeof(char));

    sprintf(
        query,
        "INSERT INTO modelo (modelo) VALUES ('%s'); INSERT INTO carro (chassi, marca, ano, valor_custo, valor_venda, is_venda, is_novo, is_troca, cod_setor, cod_modelo) VALUES ('%s', '%s', %d, %2lf, %2lf, %d, %d, %d, %d, LAST_INSERT_ID());",
        car->model,
        car->chassis,
        car->brand,
        car->year,
        car->cost_value,
        car->sell_value,
        car->is_sell,
        car->is_new,
        car->is_trade,
        car->sector_code
    );

    return make_query(connection, query);
}



int
screen_new_sector( MYSQL *connection )
{
    SECTOR *sector = (SECTOR *) calloc(1, sizeof(SECTOR));
    
    sector->agency_code = -1;
    
    do
    {
        WINDOW *content = create_basic_layout(k_sector_height, k_sector_width);
        if (content == NULL)
        {
            return EXIT_FAILURE;
        }

        keypad(content, true);

        template_print_sector_info(content, "Register New Sector", 0, 0);
        template_get_sector_info(content, sector, 0, 0);
    }
    while (!screen_yes_no(k_question_data, k_question_data_size));

    if (strcmp(sector->name, ""))  return EXIT_SUCCESS;
    if (sector->agency_code == -1) return EXIT_SUCCESS;

    char *query = (char *) calloc(512, sizeof(char));
    
    sprintf(
        query,
        "INSERT INTO setor (cod_agencia, nome) VALUES (%d, '%s');",
        sector->agency_code,
        sector->name
    );

    return make_query(connection, query);
}



// int
// screen_sell( MYSQL *connection )
// {
//     WINDOW *content = create_basic_layout(k_car_height, k_car_width);
//     if (content == NULL)
//     {
//         return EXIT_FAILURE;
//     }
    
//     CAR *car = (CAR *) calloc(1, sizeof(CAR));

//     keypad(content, true);

//     template_print_car_info(content, "Sell", 0, 0);
//     template_get_car_info(content, car, 0, 0);

//     car->is_new   = screen_yes_no(k_question_new_car  , k_question_new_car_size);
//     car->is_sell  = screen_yes_no(k_question_sell_car , k_question_sell_car_size);
//     car->is_trade = screen_yes_no(k_question_trade_car, k_question_trade_car_size);

//     char *query = (char *) calloc(512, sizeof(char));
    
//     sprintf(
//         query,
//         "INSERT INTO modelo (modelo) VALUES ('%s'); INSERT INTO carro (chassi, marca, ano, valor_custo, valor_venda, is_venda, is_novo, is_troca, cod_setor, cod_modelo) VALUES ('%s', '%s', %d, %2lf, %2lf, %d, %d, %d, %d, LAST_INSERT_ID());",
//         car->model,
//         car->chassis,
//         car->brand,
//         car->year,
//         car->cost_value,
//         car->sell_value,
//         car->is_sell,
//         car->is_new,
//         car->is_trade,
//         car->sector_code
//     );

//     return make_query(connection, query);

//     return EXIT_SUCCESS;
// }



// int
// screen_trade( MYSQL *connection )
// {
//     WINDOW *content = create_basic_layout(k_car_height, 2 * k_car_width);
//     if (content == NULL)
//     {
//         return EXIT_FAILURE;
//     }

//     char  brand_client[ 31 ] = "*";
//     char  model_client[ 31 ] = "*";
//     char  plate_client[ 8 ]  = "*";
//     short year_client = -1;

//     char  brand_dealer[ 31 ] = "*";
//     char  model_dealer[ 31 ] = "*";
//     char  plate_dealer[ 8 ]  = "*";
//     short year_dealer = -1;

//     keypad(content, true);

//     template_print_car_info(content, "Client", 0, 0);
//     template_print_car_info(content, "Dealer", 1, 0);
//     template_get_car_info(content, 0, 0, brand_client, model_client, &year_client, plate_client);
//     template_get_car_info(content, 1, 0, brand_dealer, model_dealer, &year_dealer, plate_dealer);

//     bool is_new_dealer = screen_yes_no(k_question_new_car, k_question_new_car_size);

//     char *query = (char *) calloc(512, sizeof(char));
    
//     sprintf(query, "INSERT INTO funcionario(cpf, nome, endereco, salario, cod_setor, data_Admissao)\
//                     VALUES (%s, %s, %s, %2lf, %d, %s);", cpf, name, address, salary, sector_code, get_current_date());
    
//     return make_query(connection, query);
// }



// int
// screen_revision( MYSQL *connection )
// {
//     WINDOW *content = create_basic_layout(12, 40);
//     if (content == NULL)
//     {
//         return EXIT_FAILURE;
//     }

//     char  plate_client[ 8 ] = "*";

//     char *query = (char *) calloc(512, sizeof(char));
    
//     sprintf(query, "INSERT INTO funcionario(cpf, nome, endereco, salario, cod_setor, data_Admissao)\
//                     VALUES (%s, %s, %s, %2lf, %d, %s);", cpf, name, address, salary, sector_code, get_current_date());

//     return make_query(connection, query);
// }



int
screen_management( MYSQL *connection )
{
    bool state = RUNNING;
    bool error = false;

    while (state)
    {
        switch (screen_menu(k_menu_management, k_menu_management_size))
        {
            case 0: error = screen_new_sector(connection);   break;
            case 1: error = screen_new_employee(connection); break;
            case 2: /* error = screen_bonus(connection); */      break;
            case 3: state = STOP;                            break;
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
    FILE *file = fopen("error_output.txt", "w");
    fclose(file);

    if (init_api_ncurses()) return EXIT_FAILURE;
    
    if (argc == 1)
    {
        file = fopen("error_output.txt", "a");
        fprintf(file, "%s %s", k_fatal_error, k_database_name_error);
        fclose(file);
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
                    case 0: error = screen_new_client(connection); break;
                    case 1: error = screen_new_car(connection);    break;
                    case 2: /*error = screen_sell(connection);*/       break;
                    case 3: /*error = screen_trade(connection);*/      break;
                    case 4: /*error = screen_revision(connection);*/   break;
                    case 5: error = screen_management(connection); break;
                    case 6: logged_in = false;                     break;
                    default: break;
                }
            }
            else
            {
                switch (screen_menu(k_menu_user, k_menu_user_size))
                {
                    case 0: error = screen_new_client(connection); break;
                    case 1: error = screen_new_car(connection);    break;
                    case 2: /*error = screen_sell(connection);*/       break;
                    case 3: /*error = screen_trade(connection);*/      break;
                    case 4: /*error = screen_revision(connection);*/   break;
                    case 5: logged_in = false;                     break;
                    default: break;
                }
            }

            if (error)
            {
                logged_in = false;
                state = STOP;
            }
        }
    }

    close_api_mysql(connection);
    close_api_ncurses();
    fclose(file);

    return EXIT_SUCCESS;
}
