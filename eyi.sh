#!/bin/bash
time=`date +%Y-%m-%d_%H:%M:%S`

n=`ps -aux | wc -l`

name=(`ps -aux | awk 'NR>1{print $11","$2","$1","$3"%"","$4"%"}'`)

for (( i=0; i<n; i++ )); do
    cpu[$i]=`echo ${name[$i]} | cut -d "," -f4`
    used[$i]=`echo ${name[$i]} | cut -d "," -f5`
    PID1[$i]=`echo ${name[$i]} | cut -d "," -f2`
done

m=0

for (( i=0; i<n; i++ )); do
    if [[ ${cpu[$i]} > 50 || ${used[$i]} > 50 ]]; then
        sum1[$m]=${PID1[$i]}
        name1[$m]=${name[$i]}
        (( m++ ))
    fi
done


sleep 5s

time=`date +%Y-%m-%d_%H:%M:%S`

n=`ps -aux | wc -l`

name=(`ps -aux | awk 'NR>1{print $11","$2","$1","$3"%"","$4"%"}'`)

for (( i=0; i<n; i++ )); do
    cpu[$i]=`echo ${name[$i]} | cut -d "," -f4`
    used[$i]=`echo ${name[$i]} | cut -d "," -f5`
    PID2[$i]=`echo ${name[$i]} | cut -d "," -f2`
done

x=0

for (( i=0; i<n; i++ )); do
    if [[ ${cpu[$i]} > 50 || ${used[$i]} > 50 ]]; then
        sum2[$x]=${PID2[$i]}
        name2[$x]=${name[$i]}
        (( x++ ))
    fi
done

for (( i=0; i<m; i++ )); do
    for (( j=0; j<x; j++ )); do
        if [[ ${sum1[$i]} -eq ${sum2[$j]} ]]; then
            name3[$i]=`echo ${name2[$i]} | tr "," " "`
            echo "$time ${name3[$i]}"
            break
        fi
    done
done
