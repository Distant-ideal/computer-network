#!/bin/bash
time=`date +"%Y-%m-%d_%H:%M:%S"`

total=`free -m | awk 'NR==2{print $2}'`
use=`free -m | awk 'NR==2{print $4}'`

(( used=total-use ))
(( Occupy=used*100/total ))

echo -n "$time $total $use $Occupy% "
echo "scale=1;0.3*$1+0.7$Occupy" | bc
