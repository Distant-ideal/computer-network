#!/bin/bash

time=`date +%Y-%m-%d_%H:%M:%S`

fenqu=(`df -T -x devtmpfs -m -x tmpfs | tail -n +2 | awk '{printf("a["NR"]=%s,%d,%d,%s ",$7,$3,$5,$6)}'`)

total=0
used=0

(( n=${#fenqu[@]}-1 ))

for (( i=0;i<=n;i++ )); do
    (( total=$total+`echo ${fenqu[$i]} | cut -d "," -f 2` )) 
    (( used=$used+`echo ${fenqu[$i]} | cut -d "," -f 3` ))
done

(( use=($total-$used)*100/$total )) 

echo "$time 0 disk $total $used $use%" 

for (( j=0;j<=n;j++ )); do
    echo "$time 1 `echo ${fenqu[j]} | cut -d "=" -f 2 | tr "," " "`" 
done
