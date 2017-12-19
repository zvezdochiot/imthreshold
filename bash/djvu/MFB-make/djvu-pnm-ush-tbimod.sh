#!/bin/bash
# Make (Mask-Foreground-Background)-djvu file.
# Peaking: unsharp filter.
# Threshold: bimodal thresholding.
# Build: djvulibre.
#
# Depends: djvulibre-bin, netpbm, imthreshold.

if [ -z "$1" ]
then
	echo "USAGE: bash $0 images [dpi=300]"
	exit 1
else
	src="$1"
fi
if [ -z "$2" ]
then
	tdpi="300"
else
	tdpi="$2"
fi
ttmp="/tmp/djvu-colors-$$"

imthreshold-finfo "$src"
anytopnm "$src" > "$ttmp.a.ppm"
imthreshold-fautoinv "$ttmp.a.ppm" "$ttmp.b.tif"
imthreshold-funsharp -r 6.0 -a 2.0 "$ttmp.b.tif" "$ttmp.c.tif"
rm -f "$ttmp.b.tif"
imthreshold-tbimod "$ttmp.c.tif" "$ttmp.d.tif"
rm -f "$ttmp.c.tif"
anytopnm "$ttmp.d.tif" | pbmclean -minneighbors=4 -black | pbmclean -minneighbors=4 -white > "$ttmp.m.pbm"
rm -f "$ttmp.d.tif"
cjb2 -clean -dpi "$tdpi" "$ttmp.m.pbm" "$ttmp.m.djvu"
ls -l "$ttmp.m.djvu"
djvumake "$src.djvu" INFO=,,"$tdpi" Sjbz="$ttmp.m.djvu" PPM="$ttmp.a.ppm"
ls -l "$src.djvu"
rm -f "$ttmp.a.ppm"
rm -f "$ttmp.m.pbm"
rm -f "$ttmp.m.djvu"
echo ""
