#! /bin/bash
endless_loop()
{
    while true
    do
        i=i+100;
        i=100
        n=1024*1024
    done
}

if [ $# != 1 ] then
    echo "USAGE: $0 <CPUs>"
    exit 1;
fi

for i in `seq $1`
do   
    endless_loop
done
