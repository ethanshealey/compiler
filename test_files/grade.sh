#!/bin/bash
# directory listing

dir=(`ls *.lille`)
size=${#dir[@]}
index=0
while [ $index -lt $size ]
do   # print 5 filesnames to a line
    echo "****************************"
    echo "${dir[$index]}      "
    echo "****************************"
    ../compiler -l ${dir[$index]}
    index=`expr $index + 1`
done
echo "COMPLETED"


