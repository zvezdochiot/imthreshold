#!/bin/bash

if [ -z "$1" ]
then
	echo "USAGE: bash $0 djvufile [dpi=300]"
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
ttmp="/tmp/djvu-$$"

testdjvu=`djvudump "$src" | grep "FORM:DJVU"`
if [ -z "$testdjvu" ]
then
    echo "File \"$src\" not djvu!"
else
    echo "$src"
    name="${src%.*}"
    mkdir -p "layers"
    mkdir -p "layers/bw"
    mkdir -p "layers/fg"
    mkdir -p "layers/bg"
    ddjvu -format=tiff -mode=mask "$src" "layers/bw/$name.tif"
    ls "layers/bw/$name.tif"
    djvuextract "$src" FG44="layers/fg/$name.djvu.FG44.cnk" BG44="layers/bg/$name.djvu.BG44.cnk"
fi
echo ""
