#!/bin/bash
# Author: Airvikar <http://ubuntu-wine.ru>

if ! [ -f /usr/bin/djvused ]
then
    #nopack="Packades is not instaled - djvulibre-bin!"
    nopack="Not found djvulibre-bin!"
    echo -en $nopack; sleep 5
    exit
fi
if [ -z "$2" ]
then
    echo "Usage: $0 <djvufile> <textfie> [offset=0]"
    echo "Format <textfie>:"
    echo "[title 1][ ][number page]"
    echo "[title 2][ ][number page]"
    echo "..."
    exit
else
    DJ="$1"
    TX="$2"
    if [ -z "$3" ]
    then
        tofs="0"
    else
        tofs="$3"
    fi
fi
tsed="$DJ.nav.sed"

IM=${TX##*\/}
IM=${IM/\'}
RED=$(cat "$IM" | sed '/^$/d' | sed 's/^[ ]*//;s/[ ]*$//' | sed "s/\"/\'\'/g")
N=$(echo "$RED" | sed -n '$=')
> "$tsed"
echo "set-outline" >> "$tsed"
echo "(bookmarks" >> "$tsed"
for i in `seq $N`
do
  str=$(echo "$RED" | sed -n ""$i"p")
  str1=$(echo "$str" | sed 's/[^>]\+[ ]//g')
  str2=$(echo "$str" | sed 's/[^ ]*$//g')
  str1=$(($str1+$tofs))
  echo "(\"$str2\" \"#$str1\")" >> "$tsed"
done
echo ")" >> "$tsed"
echo "." >> "$tsed"
echo "save" >> "$tsed"
djvu=${DJ##*\/}
djvu=${djvu/\'}
uni2ascii -q -a K < "$tsed" > "$tsed.utf8"
mv -f "$tsed.utf8" "$tsed"
djvused $djvu -f "$tsed"
echo ""
#rm -f "$tsed"
exit
