#!/bin/bash

	chmod -R 755 /app/wordpress/*
	chown -R mysql:mysql /var/run/mysqld/ /var/lib/mysql/
	service mysql start
	# make
	./webserv /app/configuration_files/test_docker.conf