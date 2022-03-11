printf "Content-Type: text/plain\r\n\r\n"
i=0
while [ $i != 1000 ]
do
	echo "Hello World!"
	i=$((i+1))
done
