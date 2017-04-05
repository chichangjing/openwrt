#!/bin/ash

getpid() 
{ 
    user=$1 
    name=$2 

    pid=`ps |grep $name|grep -v grep|grep -v vi|grep -v dbx|grep -v tail|grep -v start|grep -v stop |sed -n 1p |awk '{print $1}'` 

    echo $pid 
}


key_value_get()
{
    line=`cat /tmp/netif_stat | grep $1`

    i=1
    while true
    do { 
            split=`echo $line | cut -d ":" -f$i`
            if [ "$split" != "" ]
            then
                if [ "$i" -eq 2 ]; then
                    echo `echo "$split" | sed 's/^[ \t]*//g'`
                fi
                let i+=1
            else
                break
            fi
        }
    done
}


check_netif_status()
{
    ethtool -S eth0 | sed -n '/IEEE_rx_[crc align macerr]/p' > /tmp/netif_stat

    crc_var=`key_value_get crc`
    align_var=`key_value_get align`
    macerr_var=`key_value_get macerr`

    if [ "$crc_var" -gt 512 -a "$macerr_var" -gt 512 ]; then
        logfile="/tmp/netif_log"
        date >> $logfile
        cat /tmp/netif_stat >> $logfile
        ifconfig eth0 down
        sleep 1
        ifconfig eth0 up
    fi
}


while true
do 
    {
        jwuppid=`getpid root jwupgrade.py`
        if [ "-$jwuppid" == "-" ]
        then
            {
                echo "restart jwupgrade.py"
                /usr/sbin/jwupgrade.py > /dev/null&
            }
        fi

        uid_line=`jwboardinfo | grep uid`
        uid=${uid_line:4}
        jwrstpid=`getpid root jwgpio.py`
        if [ "-$jwrstpid" == "-" ] && [ "$uid" != "54-10-01-06-01-01-00-00" ] && [ "$uid" != "54-20-01-06-01-02-00-00" ]
        then
            {
                echo "restart jwgpio.py"
                /usr/sbin/jwgpio.py > /dev/null&
            }
        fi

        check_netif_status eth0

        sleep 10
    }
    
done   
