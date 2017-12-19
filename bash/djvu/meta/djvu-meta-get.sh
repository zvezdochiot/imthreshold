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
if [ ! -f "/usr/bin/ascii2uni" ]
then
    if [ "+$tprogs" != "+" ]
    then
	tprogs="$tprogs, "
    fi
    tprogs="$tprogs""ascii2uni (uni2ascii_*.deb)"
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
    echo "Usage: $0 <djvufile> [<metafile>]"
    exit 1
else
    src="$1"
fi

if [ -z "$2" ]
then
    fmeta="$src.meta"
else
    fmeta="$2"
fi

fname="${src%.djvu}"
fmetau="$fmeta.uni"
fmetased="$fmeta.dsed"

djvused "$src" -e 'print-meta' > "$fmetau"
ascii2uni -q -a K < "$fmetau" > "$fmeta"

rm -f "$fmetau"
