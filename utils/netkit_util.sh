#!/usr/bin/env bash

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
            echo "" >> $m.startup
            echo "/etc/init.d/bind start" >> $m.startup
            mkdir -p $m/etc/bind
            touch $m/etc/bind/named.conf
        fi
    done
}
