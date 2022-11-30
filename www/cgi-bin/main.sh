#!/bin/bash

echo -e \
"Content-Type: text/html

<!DOCTYPE html>
<html>
<h1>
$(env)"

while read line
do
	echo $line
done
echo $line

echo \
"</h1>
</html>"
