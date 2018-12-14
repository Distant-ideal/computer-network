#!/bin/bash
time=`date +%Y-%m-%d_%H:%M:%S`

sum=`who | cut -d' ' -f1 | sort | uniq | id | awk '{print $1}' | wc -l`

threeuser=(`last | cut -d' ' -f1 |grep "[a-zA-Z0-9]"| grep -v "wtmp" | grep -v "reboot" | sort | uniq -c | awk 'RN<=3{print $2}'`)

rootuser=`last | cut -d' ' -f1 |grep "[a-zA-Z0-9]"| grep -v "wtmp" | grep -v "reboot" | sort | uniq | id | awk '{print $1}' | cut -d '(' -f2 | cut -d ')' -f1`

userinformation=`who | awk '{print $1"_"$5"_"$2}' | grep "[a-zA-Z0-9]"`

echo "$time $sum [${threeuser[0]},${threeuser[1]},${threeuser[2]}] [$rootuser] [$userinformation]"
