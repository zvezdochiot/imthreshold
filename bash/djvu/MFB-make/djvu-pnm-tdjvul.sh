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
    echo "USAGE: bash $0 [options] image"
    echo "options:"
    echo "  -a N.N      anisotropic (default = 0.0);"
    echo "  -c N        despeckle clean (default = 0);"
    echo "  -d N        dpi (default = 300);"
    echo "  -o Name     output file (default = imagename.djvu);"
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
while getopts ":a:c:d:o:h" opt
do
    case $opt in
        a) tanis="$OPTARG"
            ;;
        c) tcln="$OPTARG"
            ;;
        d) tdpi="$OPTARG"
            ;;
        o) dest="$OPTARG"
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
    dest="$src.djvu"
fi

ttmp="/tmp/djvu-colors-$$"
mkdir -p "$ttmp"

imthreshold-finfo "$src"
anytopnm "$src" > "$ttmp/a.ppm"
imthreshold-tdjvul -a "$tanis" "$ttmp/a.ppm" "$ttmp/m.tif" "$ttmp/fg.tif" "$ttmp/bg.tif" "$ttmp/fm.tif" "$ttmp/bm.tif"
rm -f "$ttmp/a.ppm"
anytopnm "$ttmp/m.tif" > "$ttmp/m.pbm"
if [ $tcln -gt 0 ]
then
    imthreshold-fdespeckle -a $tcln "$ttmp/m.pbm" "$ttmp/m.b.pbm"
    imthreshold-fdespeckle -a $tcln "$ttmp/m.b.pbm" "$ttmp/m.pbm"
    rm -f "$ttmp/m.b.pbm"
fi
rm -f "$ttmp/m.tif"
cjb2 -clean -dpi "$tdpi" "$ttmp/m.pbm" "$ttmp/m.djvu"
rm -f "$ttmp/m.pbm"
djvuextract "$ttmp/m.djvu" Sjbz="$ttmp/Sjbz.cnk"
rm -f "$ttmp/m.djvu"
anytopnm "$ttmp/fg.tif" > "$ttmp/fg.ppm"
anytopnm "$ttmp/fm.tif" > "$ttmp/fm.pbm"
rm -f "$ttmp/fg.tif" "$ttmp/fm.tif"
c44 -mask "$ttmp/fm.pbm" -slice 100 "$ttmp/fg.ppm" "$ttmp/fg.djvu"
rm -f "$ttmp/fg.ppm" "$ttmp/fm.pbm"
djvuextract "$ttmp/fg.djvu" BG44="$ttmp/FG44.cnk"
rm -f "$ttmp/fg.djvu"
anytopnm "$ttmp/bg.tif" > "$ttmp/bg.ppm"
anytopnm "$ttmp/bm.tif" > "$ttmp/bm.pbm"
rm -f "$ttmp/bg.tif" "$ttmp/bm.tif"
c44 -mask "$ttmp/bm.pbm" -slice 74+10+7+6 "$ttmp/bg.ppm" "$ttmp/bg.djvu"
rm -f "$ttmp/bg.ppm" "$ttmp/bm.pbm"
djvuextract "$ttmp/bg.djvu" BG44="$ttmp/BG44.cnk"
rm -f "$ttmp/bg.djvu"
djvumake "$dest" INFO=,,"$tdpi" Sjbz="$ttmp/Sjbz.cnk" FG44="$ttmp/FG44.cnk" BG44="$ttmp/BG44.cnk"
rm -f Sjbz="$ttmp/Sjbz.cnk" FG44="$ttmp/FG44.cnk" BG44="$ttmp/BG44.cnk"
ls -l "$dest"
rm -fr "$ttmp"
echo ""
