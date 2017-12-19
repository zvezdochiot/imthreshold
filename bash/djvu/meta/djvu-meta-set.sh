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
if [ ! -f "/usr/bin/uni2ascii" ]
then
    if [ "+$tprogs" != "+" ]
    then
	tprogs="$tprogs, "
    fi
    tprogs="$tprogs""uni2ascii (uni2ascii_*.deb)"
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

if [ ! -f "$fmeta" ]
then
    echo "Usage: $0 <djvufile> <metafile>"
    exit 1
fi

fname="${src%.djvu}"
fmetau="$fmeta.uni"
fmetased="$fmeta.dsed"
fdest="$fname.meta.djvu"

uni2ascii -q -a K < "$fmeta" > "$fmetau"
echo "select" > "$fmetased"
echo "set-meta \"$fmetau\"" >> "$fmetased"
echo "save-bundled \"$fdest\"" >> "$fmetased"

djvused -v -f "$fmetased" "$src"

rm -f "$fmetased"
rm -f "$fmetau"
