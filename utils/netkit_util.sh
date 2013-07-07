#!/usr/bin/env bash

takelab()
{
    mkdir $1
    cd $1
    cp $HOME/dotlab/assets/labstart.py .
}

hw()
{
    local hws="$@"

    for m in $hws; do
        mkdir $m
        touch $m.startup
    done
}


bindit()
{
    local hws="$@"

    for m in $hws; do
        if [[ -f $m.startup ]] && [[ -d $m ]]; then
            echo "/etc/init.d/bind start" >> $m.startup
            mkdir -p $m/etc/bind
            touch $m/etc/bind/named.conf
        fi
    done
}

routeit()
{
    local hws="$@"

    for m in $hws; do
        if [[ -f $m.startup ]] && [[ -d $m ]]; then
            echo "/etc/init.d/zebra start" >> $m.startup
            mkdir -p $m/etc/zebra
            echo "zebra=yes" > $m/etc/zebra/daemons
        fi
    done
}

ripit()
{
    local hws="$@"
    
    for m in $hws; do
        if [[ -f $m.startup ]] && [[ -d $m/etc/zebra ]] && [[ -f $m/etc/zebra/daemons ]]; then
            echo "ripd=yes" >> $m/etc/zebra/daemons
            touch $m/etc/zebra/ripd.conf
        fi
    done
}

bgpit()
{
    local hws="$@"
    
    for m in $hws; do
        if [[ -f $m.startup ]] && [[ -d $m/etc/zebra ]] && [[ -f $m/etc/zebra/daemons ]]; then
            echo "bgpd=yes" >> $m/etc/zebra/daemons
            touch $m/etc/zebra/bgpd.conf
        fi
    done
}
