# start image
FROM ubuntu:18.04

## installation process
#
RUN apt-get -y update
RUN apt-get install -y git wget vim clang make valgrind siege curl
#zsh

# RUN sh -c "$(wget -O- https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get install -y php php-cgi php-mysql

RUN apt-get install -y unzip
RUN cd /root && \
	wget https://wordpress.org/latest.zip && \
	unzip latest.zip

RUN apt-get install -y mysql-server-5.7 mysql-client-5.7
RUN service mysql start; \
	mysql -u root -e "CREATE DATABASE wordpress;"; \
	mysql -u root -e "UPDATE mysql.user SET plugin='mysql_native_password';"; \
	mysql -u root -e "UPDATE mysql.user SET host = '%' WHERE USER = 'root';"; \
	mysql -u root -e "UPDATE mysql.user SET authentication_string=PASSWORD(\"password\") WHERE user='root';"; \
	mysql -u root -e "SELECT host, user, plugin, authentication_string FROM mysql.user WHERE user='root';"; \
	mysql -u root -e "FLUSH PRIVILEGES;";
	
RUN sed -i 's/;cgi.fix_pathinfo=1/cgi.fix_pathinfo=0/g' /etc/php/7.2/cgi/php.ini

RUN mkdir /app

WORKDIR /app
EXPOSE 8080

COPY . /app

CMD echo Starting 'run' && \
    valgrind --track-fds=yes /app/webserv /app/configuration_files/test_docker.conf

# run a terminal in the container : docker exec -it <container> bash