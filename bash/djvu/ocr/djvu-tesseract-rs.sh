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
if [ ! -f "/usr/bin/tesseract" ]
then
    if [ "+$tprogs" != "+" ]
    then
	tprogs="$tprogs, "
    fi
    tprogs="$tprogs""tesseract (tesseract-ocr_*.deb)"
fi
if [ ! -f "/usr/bin/hocr2djvused" ]
then
    if [ "+$tprogs" != "+" ]
    then
	tprogs="$tprogs, "
    fi
    tprogs="$tprogs""hocr2djvused (ocrodjvu_*.deb)"
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
    tlang="rus"
else
    tlang="$2"
fi

tpages=`djvused -e 'n;' "$src"`
tlist=`djvm -l "$src" | grep "djvu$" | awk '{ print $4 }'`

tname="${src%.djvu}"
focr="$tname-tesseract.djvused"
flog="$tname-tesseract.log"
fdest="$tname-tesseract.djvu"

echo "" > $focr
echo "$src" > $flog
echo "" >> $flog

echo "$src"
echo "pages: $tpages"
echo ""

i=0

for tpage in $tlist
do
    ttemp="/tmp/$tpage.$$"
    let i=i+1
    ddjvu -format=ppm -page=$i "$src" "$ttemp.ppm"
    mogrify -despeckle -despeckle "$ttemp.ppm"
    tdim=`identify "$ttemp.ppm" | awk '{ print $3 }'`
    convert -filter cubic -resize 8.33333% -resize "$tdim"! "$ttemp.ppm" "$ttemp.b.ppm"
    convert -filter Gaussian -resize 50% -blur 0x10 -resize 200% "$ttemp.ppm" "$ttemp.b.ppm"
    composite -compose Minus "$ttemp.b.ppm" "$ttemp.ppm" "$ttemp.c.ppm"
    rm -f "$ttemp.b.ppm"
    rm -f "$ttemp.ppm"
    convert -normalize -threshold 10% -negate "$ttemp.c.ppm" "$tpage.pbm"
    rm -f "$ttemp.c.ppm"
    echo "Page $i: $tpage"
    echo "$tpage" >> "$flog"
    tesseract "$tpage.pbm" "$tpage" -l "$tlang" hocr 2>> "$flog"
    hocr2djvused < "$tpage.hocr" > "$tpage.dsed"
    sed -i -e "s/^select 1/select \"$tpage\"/" "$tpage.dsed"
    cat "$tpage.dsed" >> "$focr"
    rm -f "$tpage.pbm"
    rm -f "$tpage.txt"
    rm -f "$tpage.hocr"
    rm -f "$tpage.dsed"
done

echo "save-bundled \"$fdest\"" >> "$focr"
sed -i -e '/^\[.*\]$/d' "$focr"
echo "" >> "$focr"
djvused -v -f "$focr" "$src"
