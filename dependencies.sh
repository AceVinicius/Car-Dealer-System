sudo apt install mysql-server -y
sudo service mysql start
sudo mysql_secure_connection
sudo apt install libncurses5-dev libncursesw5-dev
sudo mysql -u root -p
ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'Appetite#1987'; flush privileges;
exit
