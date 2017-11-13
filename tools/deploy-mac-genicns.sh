#!/bin/bash
# prepare .icns file for Joker TV player for OSx
set -e

JICO=""

while getopts ":i:" opt; do
    case ${opt} in
        i ) # iconfile
            JICO=$OPTARG
            ;;
    esac
done

if [ -z "$JICO" ]
then
	echo "Incorrect arguments."
    echo "Example: ./deploy-mac-genicns.sh -i logo-joker-whiteborder-1024x1024.png"
	false
fi

echo "Processing $JICO ..."

INFILE=$JICO
OUTDIR=joker_player.iconset

mkdir $OUTDIR
sips -z 16 16     $INFILE --out $OUTDIR/icon_16x16.png
sips -z 32 32     $INFILE --out $OUTDIR/icon_16x16@2x.png
sips -z 32 32     $INFILE --out $OUTDIR/icon_32x32.png
sips -z 64 64     $INFILE --out $OUTDIR/icon_32x32@2x.png
sips -z 128 128   $INFILE --out $OUTDIR/icon_128x128.png
sips -z 256 256   $INFILE --out $OUTDIR/icon_128x128@2x.png
sips -z 256 256   $INFILE --out $OUTDIR/icon_256x256.png
sips -z 512 512   $INFILE --out $OUTDIR/icon_256x256@2x.png
sips -z 512 512   $INFILE --out $OUTDIR/icon_512x512.png
sips -z 1024 1024 $INFILE --out $OUTDIR/icon_512x512@2x.png
iconutil -c icns $OUTDIR

echo "Done. Result: joker_player.icns"
