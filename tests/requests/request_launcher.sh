#!/bin/bash

TYPE=$1
HEADER_PATH=""
BODY_PATH=""
URL=""

# need the method as argument
if [ "$#" != 1 ]; then
    echo "use : ./request_launcher.sh [get/post/delete]"
fi


if [ "$TYPE" = "get" ]; then
    echo -n "Enter headers path if you need : "
    read HEADER_PATH
    echo -n "Enter url : "
    read URL
    if [ -z "$HEADER_PATH" ]; then  # arg is empty
        curl -v $URL
    else
        curl -v -H @$HEADER_PATH $URL
    fi
elif [ "$TYPE" = "post" ]; then
    echo -n "Enter headers path if you need : "
    read HEADER_PATH
    echo -n "Enter body path : "
    read BODY_PATH
    if [ -z "$HEADER_PATH" ]; then # arg is empty
        curl -X POST -v -d @$BODY_PATH localhost:9000/test/post.php
    else
        curl -X POST -v -H @$HEADER_PATH -d @$BODY_PATH localhost:9000/test/post.php
    fi
elif [ "$TYPE" = "delete" ]; then
    echo  "delete method not implemented yet"
else
    echo  "Only this methods are accepted : get, post ans delete"
fi


