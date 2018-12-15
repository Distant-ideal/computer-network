#!/bin/bash
time=`date +%Y-%m-%d_%H:%M:%S`

load=`cat /proc/loadavg | awk '{print $1" "$2" "$3}'`

temp=`cat /sys/class/thermal/thermal_zone0/temp | awk '{printf("%d",$1/1000)}'`

canuse=`top -bn 1 -i -c | awk 'NR==3{print $8}'`
sum=100
used=`echo "scale=1;$sum-$canuse" | bc `

if [[ $temp>=0 && $temp<=10  ]]; then
    c="normal"
elif [[ $temp>=10 && $temp<=80   ]]; then
    c="note" 
elif [[ $temp>=80 && $temp<=100  ]]; then
    c="warning"
fi

echo "$time $load $used% $temp℃　$c"
