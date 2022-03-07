# Webserv

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white) 

This school project is about writing our own HTTP server in C++ 98.

![Capture d’écran du 2022-03-07 14-07-06](https://user-images.githubusercontent.com/45667806/157040209-6c4e0157-b2b7-49b6-9be1-898bad8cda2f.png)

## Installation

Make sure to have the right php version for the webserver cgi (7.2 or 7.4 to be modified in .conf file depending on your version)

```bash
#to verify
php -v
```
## Usage

```bash
make && ./webserv configuration_files/test.conf # Works with any .conf that matches our writing conventions
```
Now you can connect to the port specified in the .conf file (8080 and 9000 for our test.conf file) 

## Authors

* **Elie Teillet** _alias_ [@Eteillet](https://github.com/eteillet)
* **Julien Nguyen** _alias_ [@Jnguyen](https://github.com/Ljulienng)

[![forthebadge](http://forthebadge.com/images/badges/built-with-love.svg)](http://forthebadge.com)
