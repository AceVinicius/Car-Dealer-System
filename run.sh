clang -g -ggdb -Wall -W -Wpedantic -O3 -lncurses ./src/main.c -o ./bin/cds $(mysql_config --libs)
./bin/cds car_dealer_system
