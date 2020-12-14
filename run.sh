sudo service mysql start

clang -g -Wall -Wpedantic -O3 -lncurses ./src/main.c -o ./bin/CDS $(mysql_config --libs)

./bin/CDS CDS_database