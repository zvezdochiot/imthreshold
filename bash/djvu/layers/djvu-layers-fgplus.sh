#!/bin/bash

if [ -z "$1" ]
then
	echo "USAGE: bash $0 djvufile"
	exit 1
else
	src="$1"
fi
ttmp="/tmp/djvu-$$"

testdjvu=`djvudump "$src" | grep "FORM:DJVU"`
if [ -z "$testdjvu" ]
then
    echo "File \"$src\" not djvu!"
else
    echo "$src"
    tinc=`djvused "$src" -e 'ls' | grep "iff$" | awk '{ print $3 }'`
    tdpi=`djvused "$src" -e 'dump' | grep "INFO" | awk '{print $6}'`
    djvuextract "$src" Sjbz="$src.Sjbz.cnk"
    if [ ! -z "$tinc" ]
    then
	djvumake "$src" INFO=,,$tdpi INCL="$tinc" Sjbz="$src.Sjbz.cnk" FG44="../fg/$src.FG44.cnk" BG44="../bg/$src.BG44.cnk"
    else
	djvumake "$src" INFO=,,$tdpi Sjbz="$src.Sjbz.cnk" FG44="../fg/$src.FG44.cnk" BG44="../bg/$src.BG44.cnk"
    fi
fi
echo ""
