#!/bin/bash

tprogs=""
if [ ! -f "/usr/bin/djvused" ]
then
    if [ "+$tprogs" != "+" ]
    then
	tprogs="$tprogs, "
    fi
    tprogs="$tprogs""djvused (djvulibre-bin_*.deb)"
fi
if [ ! -f "/usr/bin/awk" ]
then
    if [ "+$tprogs" != "+" ]
    then
	tprogs="$tprogs, "
    fi
    tprogs="$tprogs""awk (gawk_*.deb)"
fi
if [ "+$tprogs" != "+" ]
then
    echo "!!!!"
    echo "  Not found $tprogs!"
    echo "!!!!"
    exit 1
fi

if [ -z "$1" -o "+$1" = "+--help" -o "+$1" = "+-h" ]
then
    echo "Usage: $0 <djvufile>"
    exit 1
else
    src="$1"
fi

fmeta="$src.meta"

tname="${src%.djvu}"

fdest="$tname.meta.djvu"

tnamet=`echo "$tname" | sed -e 's/_/ /g'`

tautor=`echo "$tnamet" | awk -F "-" '{ print $1 }'`
echo "$tautor"
tyear=`echo "$tnamet" | awk -F "-" '{ print $2 }'`
echo "$tyear"
ttitle=`echo "$tnamet" | awk -F "-" '{ print $3 }'`
echo "$ttitle"
tdate=`date -R`
echo "$tdate"

echo "(metadata" > "$fmeta"
echo "  Author \"$tautor\"" >> "$fmeta"
echo "  Title  \"$ttitle\"" >> "$fmeta"
echo "  Year   \"$tyear\"" >> "$fmeta"
echo "  Subject  \"$src\"" >> "$fmeta"
echo "  Publisher  \"$tautor\"" >> "$fmeta"
echo "  Keywords  \"$ttitle\"" >> "$fmeta"
echo "  Producer   \"DjVu Editor 4.1.0 Build 333\"" >> "$fmeta"
echo "  Creator   \"djvused\"" >> "$fmeta"
echo "  CreationDate  \"$tdate\"" >> "$fmeta"
echo "  OCR    \"Tesseract (rus)\"" >> "$fmeta"
echo ")" >> "$fmeta"
echo "." >> "$fmeta"
