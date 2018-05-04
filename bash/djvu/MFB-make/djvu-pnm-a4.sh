#!/bin/bash
# Make (Mask-Foreground-Background)-djvu file.
# Threshold: bimodal thresholding.
# Build: djvulibre.
#
# Depends: djvulibre-bin, netpbm, imthreshold, bc.

ta4w2400="19843"
ta4h2400="28063"
tdpi="300"
tpmr="0"

function usage()
{
    echo "Imtreshold."
    echo
    echo "USAGE: bash $0 [options] image"
    echo "options:"
    echo "  -d N        dpi (default = 300);"
    echo "  -o Name     output file (default = imagename-a4.tif);"
    echo "  -p N        PMean filter (default = 0 [none]);"
    echo "  -h          help."
    echo
    exit 1
}
if [ $# = 0 ]
then
    usage
fi
while getopts ":d:o:p:h" opt
do
    case $opt in
        d) tdpi="$OPTARG"
            ;;
        o) dest="$OPTARG"
            ;;
        p) tpmr="$OPTARG"
            ;;
        h) usage
            ;;
        *) echo "Unknown option -$OPTARG"
            exit 1
            ;;
    esac
done
shift "$(($OPTIND - 1))"
src="$1"
if [ -z "$src" ]
then
    usage
fi
if [ -z "$dest" ]
then
    dest="$src-a4.tif"
fi
ttmp="/tmp/djvu-colors-$$"
mkdir -p "$ttmp"

ta4w="$(($ta4w2400*$tdpi/2400)).5"
ta4h="$(($ta4h2400*$tdpi/2400)).5"

anytopnm "$src" > "$ttmp/a.ppm"
tinfo=`imthreshold-finfo "$ttmp/a.ppm" "$ttmp/a.tif"`
ttp=`echo "$tinfo" | grep "Bits=" | awk '{ print $2 }'`
tw=`echo "$tinfo" | grep "Width=" | awk '{ print $2 }'`
th=`echo "$tinfo" | grep "Height=" | awk '{ print $2 }'`
rm -f "$ttmp/a.ppm"
echo "$src: ($ttp) $tw x $th"
if [ $tw -gt $th ]
then
    twl=$th
    thl=$tw
else
    twl=$tw
    thl=$th
fi
tkw=`echo "$ta4w/$twl" | bc -l`
tkh=`echo "$ta4h/$thl" | bc -l`
tfwh=`echo "$tkw>$tkh" | bc`
if [ $tfwh -eq 0 ]
then
    tki="$tkw"
else
    tki="$tkh"
fi
echo "scale: $tki"
tf1=`echo "$tki>1" | bc`
while [ "$tf1" -eq 1 ]
do
    imthreshold-shris "$ttmp/a.tif" "$ttmp/b.tif"
    mv -f "$ttmp/b.tif" "$ttmp/a.tif"
    tki=`echo "$tki/2" | bc -l`
    echo "scale: $tki"
    tf1=`echo "$tki>1" | bc`
done
imthreshold-size -r "$tki" "$ttmp/a.tif" "$ttmp/a4.tif"
rm -f "$ttmp/a.tif"
imthreshold-fpmean -r "$tpmr" "$ttmp/a4.tif" "$dest"
rm -f "$ttmp/a4.tif"
rm -fr "$ttmp"
echo ""
