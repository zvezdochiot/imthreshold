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
if [ ! -f "/usr/bin/hocr2djvused" ]
then
    if [ "+$tprogs" != "+" ]
    then
	tprogs="$tprogs, "
    fi
    tprogs="$tprogs""hocr2djvused (ocrodjvu_*.deb)"
fi
if [ ! -f "/usr/bin/cuneiform" ]
then
    if [ "+$tprogs" != "+" ]
    then
	tprogs="$tprogs, "
    fi
    tprogs="$tprogs""cuneiform (cuneiform_*.deb)"
fi
if [ ! -f "/bin/sed" ]
then
    if [ "+$tprogs" != "+" ]
    then
	tprogs="$tprogs, "
    fi
    tprogs="$tprogs""sed (sed_*.deb)"
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
    echo "Usage: $0 <djvufile> [lang]"
    exit 1
else
    src="$1"
fi

if [ -z "$2" ]
then
    tlang="ruseng"
else
    tlang="$2"
fi

tpages=`djvused -e 'n;' "$src"`
tlist=`djvm -l "$src" | grep "djvu$" | awk '{ print $4 }'`

tname="${src%.djvu}"
tnmc="$tname-cuneiform"
focr="$tnmc.xml"
flog="$tnmc.log"
fdest="$tnmc.djvu"

echo "$src" > $flog
echo "" >> $flog

echo "$src"
echo "pages: $tpages"
echo ""

i=0

cp -f "$src" "$fdest"

for tpage in $tlist
do
    ttmp="/tmp/djvu-ocr-$tpage.$$"
    tdir="/tmp/djvu-ocr-${tpage}_files"
    let i=i+1
    ddjvu -format=pbm -mode=mask -page=$i "$src" "$ttmp.pbm"
    echo "Page $i: $tpage"
    echo "$tpage" >> "$flog"
    cuneiform -f hocr -l "$tlang" -o "$ttmp.hocr" "$ttmp.pbm" 2>> "$flog"
    rm -fr "$tdir"
    if [ ! -f  "$ttmp.hocr" ]
    then
	cuneiform --dotmatrix -f hocr -l "$tlang" -o "$ttmp.hocr" "$ttmp.pbm" 2>> "$flog"
	rm -fr "$tdir"
    fi
    if [ ! -f  "$ttmp.hocr" ]
    then
	cuneiform --singlecolumn -f hocr -l "$tlang" -o "$ttmp.hocr" "$ttmp.pbm" 2>> "$flog"
	rm -fr "$tdir"
    fi
    rm -f "$ttmp.pbm"
    if [ -f  "$ttmp.hocr" ]
    then
	hocr2djvused < "$ttmp.hocr" > "$ttmp.dsed"
	rm -f "$ttmp.hocr"
	sed -i -e "s/^select 1/select \"$tpage\"/" "$ttmp.dsed"
	echo "save;" >>  "$ttmp.dsed"
	djvused "$fdest" -f "$ttmp.dsed"
	rm -f "$ttmp.dsed"
    else
	echo "Error ocr $tpage !"
    fi
done
