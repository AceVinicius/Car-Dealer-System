time clang -Weverything -Wall -Wextra -O2 -lncurses -D_FORTIFY_SOURCE=2 ./src/main.c -o ./bin/cds $(mysql_config --libs)

./bin/cds car_dealer_system
