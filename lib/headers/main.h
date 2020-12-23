#ifndef _MAIN_H_
# define _MAIN_H_


# define RUNNING true
# define STOP    false

# define PADDING 1
# define MARGIN  3



const char *k_yes_no[ ] = {
    "[N]o",
    "[Y]es"
};
const int k_yes_no_size = sizeof(k_yes_no) / sizeof(char *);

const char *k_menu_sign_in[ ] = {
    "Sign [I]n",
    "[E]xit"
};
const int k_menu_sign_in_size = sizeof(k_menu_sign_in) / sizeof(char *);

const char *k_menu_admin[ ] = {
    "Register [N]ew Car",
    "[S]ell",
    "[T]rade",
    "Schedule [R]evision",
    "[M]anagement",
    "Sign [O]ut"
};
const int k_menu_admin_size = sizeof(k_menu_admin) / sizeof(char *);

const char *k_menu_admin_management[ ] = {
    "New [S]ector",
    "New [F]unctionary",
    "[B]ack"
};
const int k_menu_admin_management_size = sizeof(k_menu_admin_management) / sizeof(char *);

const char *k_menu_user[ ] = {
    "Register [N]ew Car",
    "[S]ell",
    "[T]rade",
    "Schedule [R]evision",
    "Sign [O]ut"
};
const int k_menu_user_size = sizeof(k_menu_user) / sizeof(char *);

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
    "Is the car brand new?"
};
const int k_question_new_car_size = sizeof(k_question_new_car) / sizeof(char);

const char k_fatal_error[ ] = {
    "\033[31;1mfatal error:\033[0;0m"
};



MYSQL *init_api_mysql( const char * );
inline void error_api_mysql( MYSQL * );
void close_api_mysql( MYSQL * );
int init_api_ncurses( void );
void error_api_ncurses( void );
void close_api_ncurses( void );

WINDOW *create_basic_layout( const int, const int );
void template_print_car_info( WINDOW *, const char *, const int, const int );
void template_get_car_info( WINDOW *, const int, const int, char *, char *, short *, char * );
short screen_yes_no( const char *, const int );
int screen_menu( const char **, const int );
int screen_new_car( void );
int screen_sell( void );



#endif // _MAIN_H_
