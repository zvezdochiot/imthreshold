#!/bin/bash
# Make (Mask-Foreground-Background)-djvu file.
# Peaking: unsharp filter.
# Threshold: bimodal thresholding.
# Build: djvulibre.
#
# Depends: djvulibre-bin, netpbm, imthreshold.

function usage()
{
    echo "Imtreshold."
    echo
    echo "USAGE: bash $0 [options] filemask"
    echo "options:"
    echo "  -a N.N      anisotropic (default = 0.0);"
    echo "  -c N        despeckle clean (default = 0);"
    echo "  -d N        dpi (default = 300);"
    echo "  -o Name     output file (default = Date-Time-book.djvu);"
    echo "  -h          help."
    echo
    exit 1
}

if [ $# = 0 ]
then
    usage
fi
tdpi="300"
tanis="0"
tcln="0"
dest=`date +"%G%m%d-%H%M%S"`
dest="$dest-book.djvu"
while getopts ":a:c:d:lo:h" opt
do
    case $opt in
        a) tanis=$OPTARG;
            ;;
        c) tcln=$OPTARG;
            ;;
        d) tdpi=$OPTARG;
            ;;
        o) dest=$OPTARG;
            ;;
        h) usage
            ;;
        *) echo "Unknown option -$OPTARG";
            exit 1
            ;;
    esac
done
shift "$(($OPTIND - 1))"

srcmask="$@"

ttmp="/tmp/djvu-colors-$$"
mkdir -pv "$ttmp"
mkdir -pv "$ttmp/mask"
mkdir -pv "$ttmp/fg"
mkdir -pv "$ttmp/bg"

tflist="$@"
echo "$tflist"
echo ""
prex="page-"
x=10001
for timg in $tflist
do
    tpage="$prex${x:1}"
    imthreshold-finfo "$timg"
    anytopnm "$timg" > "$ttmp/a.ppm"
    imthreshold-tdjvul -a "$tanis" "$ttmp/a.ppm" "$ttmp/mask/$tpage.tif" "$ttmp/fg/$tpage.tif" "$ttmp/bg/$tpage.tif" "$ttmp/fg/$tpage.m.tif" "$ttmp/bg/$tpage.m.tif"
    rm -f "$ttmp/a.ppm"
    if [ $tcln -gt 0 ]
    then
	imthreshold-fdespeckle -a $tcln "$ttmp/mask/$tpage.tif" "$ttmp/mask/$tpage.b.pbm"
	imthreshold-fdespeckle -a $tcln "$ttmp/mask/$tpage.b.pbm" "$ttmp/mask/$tpage.pbm"
	rm -f "$ttmp/mask/$tpage.b.pbm"
	pnmtotiff -g4 "$ttmp/mask/$tpage.pbm" > "$ttmp/mask/$tpage.tif"
	rm -f "$ttmp/mask/$tpage.pbm"
    fi
    anytopnm "$ttmp/fg/$tpage.tif" > "$ttmp/fg/$tpage.ppm"
    anytopnm "$ttmp/fg/$tpage.m.tif" > "$ttmp/fg/$tpage.pbm"
    rm -f "$ttmp/fg/$tpage.tif"
    rm -f "$ttmp/fg/$tpage.m.tif"
    c44 -mask "$ttmp/fg/$tpage.pbm" -slice 100 "$ttmp/fg/$tpage.ppm" "$ttmp/fg/$tpage.djvu"
    rm -f "$ttmp/fg/$tpage.ppm"
    rm -f "$ttmp/fg/$tpage.pbm"
    djvuextract "$ttmp/fg/$tpage.djvu" BG44="$ttmp/fg/$tpage.FG44.cnk"
    rm -f "$ttmp/fg/$tpage.djvu"
    anytopnm "$ttmp/bg/$tpage.tif" > "$ttmp/bg/$tpage.ppm"
    anytopnm "$ttmp/bg/$tpage.m.tif" > "$ttmp/bg/$tpage.pbm"
    rm -f "$ttmp/bg/$tpage.tif"
    rm -f "$ttmp/bg/$tpage.m.tif"
    c44 -mask "$ttmp/bg/$tpage.pbm" -slice 74+10+7+6 "$ttmp/bg/$tpage.ppm" "$ttmp/bg/$tpage.djvu"
    rm -f "$ttmp/bg/$tpage.ppm"
    rm -f "$ttmp/bg/$tpage.pbm"
    djvuextract "$ttmp/bg/$tpage.djvu" BG44="$ttmp/bg/$tpage.BG44.cnk"
    rm -f "$ttmp/bg/$tpage.djvu"
    echo ""
    (( x++ ))
done
minidjvu -c -v -d "$tdpi" $ttmp/mask/*.tif "$ttmp/mask.djvu"
mkdir -pv "$ttmp/book"
djvmcvt -i "$ttmp/mask.djvu" "$ttmp/book" index.djvu
mkdir -pv "$ttmp/jb2"
for tdjvu in $ttmp/book/page-*.djvu
do
    tinc=`djvused "$tdjvu" -e 'ls' | grep "iff$" | awk '{ print $3 }'`
    tname=`basename "${tdjvu%.djvu}"`
    djvuextract "$tdjvu" Sjbz="$ttmp/jb2/${tname}.Sjbz.cnk"
    if [ ! -z "$tinc" ]
    then
        djvumake "$tdjvu" INFO=,,$tdpi INCL="$ttmp/book/$tinc" Sjbz="$ttmp/jb2/$tname.Sjbz.cnk" FG44="$ttmp/fg/$tname.FG44.cnk" BG44="$ttmp/bg/$tname.BG44.cnk"
    else
        djvumake "$tdjvu" INFO=,,$tdpi Sjbz="$ttmp/jb2/$tname.Sjbz.cnk" FG44="$ttmp/fg/$tname.FG44.cnk" BG44="$ttmp/bg/$tname.BG44.cnk"
    fi
    echo ""
done
djvmcvt -b "$ttmp/book/index.djvu" "$dest"
rm -frv "$ttmp"
