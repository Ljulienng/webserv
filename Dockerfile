FROM ubuntu:18.04

# quieting the configuration prompts
ARG DEBIAN_FRONTEND=noninteractive

# installation
RUN apt-get -y update
RUN apt-get install -y git wget vim clang make valgrind siege curl
RUN apt-get install -y php php-cgi php-mysql
RUN apt-get install -y mysql-server-5.7 mysql-client-5.7
RUN apt-get install -y unzip

RUN mkdir /app

# wordpress configuration
RUN cd /app && \
	wget https://wordpress.org/latest.zip && \
	unzip latest.zip

WORKDIR /app/wordpress
RUN mv wp-config-sample.php wp-config.php && \
	sed -i "s/database_name_here/wordpress/" wp-config.php && \
	sed -i "s/username_here/root/" wp-config.php && \
	sed -i "s/password_here/password/" wp-config.php
WORKDIR /
RUN rm -rf /app/latest.zip

# mysql configuration
RUN mkdir /var/run/mysqld
RUN chown -R mysql:mysql /var/lib/mysql /var/run/mysqld && \
	service mysql start; \
	mysql -u root -e "CREATE DATABASE wordpress;"; \
	mysql -u root -e "UPDATE mysql.user SET plugin='mysql_native_password';"; \
	mysql -u root -e "UPDATE mysql.user SET host = '%' WHERE USER = 'root';"; \
	mysql -u root -e "UPDATE mysql.user SET authentication_string=PASSWORD(\"password\") WHERE user='root';"; \
	mysql -u root -e "SELECT host, user, plugin, authentication_string FROM mysql.user WHERE user='root';"; \
	mysql -u root -e "FLUSH PRIVILEGES;";
	
RUN sed -i 's/;cgi.fix_pathinfo=1/cgi.fix_pathinfo=0/g' /etc/php/7.2/cgi/php.ini

# siege configuration
WORKDIR /etc/siege
RUN sed -i "s/gmethod = HEAD/gmethod = GET/" siegerc && \
	sed -i "s/limit = 255/limit = 100/" siegerc && \
	sed -i "s/connection = close/connection = keep-alive/" siegerc
# RUN cp /etc/siege/siegerc /root/.siege/siege.conf
WORKDIR /

RUN	apt-get update && apt-get install -y nginx php-fpm php-json php-mbstring php-mysqli php-phar curl

WORKDIR /app
EXPOSE 8080
COPY . /app

# run a terminal in the container : docker exec -it <container> bash