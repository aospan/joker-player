#!/bin/bash
# prepare .dmg file for Joker TV player for OSx
set -ex

JDIR=""
JICO=""
JSIGN=""

VER="$(git describe --tags)"
DMGNAME=joker_player-$VER.dmg

while getopts ":d:i:s:" opt; do
    case ${opt} in
        d ) # directory with joker-player.app
            JDIR=$OPTARG
            ;;
        i ) # iconfile
            JICO=$OPTARG
            ;;
        s ) # sign id
            JSIGN=$OPTARG
            ;;
        \? ) echo "Usage: cmd [-h] [-t]"
            ;;
    esac
done

if [ -z "$JDIR" ] || [ -z "$JICO" ] || [ -z "$JSIGN" ]
then
	echo "Incorrect arguments."
    echo "Example: ./deploy-mac.sh -d ../build/src/joker-player.app -i joker_player.icns -s \"Developer ID Application: Abylay Ospan (6HF3M4G8WM)\""
	false
fi

echo "Processing $JDIR ..."

install_name_tool -add_rpath /usr/lib/ $JDIR/Contents/MacOS/joker-player || true
install_name_tool -add_rpath /usr/local/lib/ $JDIR/Contents/MacOS/joker-player || true

if [ ! -d "$JDIR/Contents/MacOS/xml" ]
then
    cp -pR $JDIR/../xml $JDIR/Contents/MacOS/
fi

if [ ! -d "$JDIR/Contents/Resources" ]
then
    mkdir $JDIR/Contents/Resources
    cp -pR /Applications/VLC.app/Contents/MacOS/plugins $JDIR/Contents/Resources/
    cp $JICO $JDIR/Contents/Resources/
fi

# is macdeployqt already processed ?
if [ ! -d "$JDIR/Contents/Frameworks" ]
then
    /usr/local/opt/qt//bin/macdeployqt $JDIR -libpath=/usr/local/ -always-overwrite -qmldir=$JDIR/../../../src/qml/
fi

# sign files
codesign -f --sign "$JSIGN" $JDIR/Contents/Frameworks/*
codesign -f --sign "$JSIGN" $JDIR/Contents/MacOS/xml/*
find $JDIR/Contents/PlugIns/ -exec codesign -f --sign "$JSIGN" {} \;
find $JDIR/Contents/Resources/plugins/ -exec codesign -f --sign "$JSIGN" {} \;
codesign -f --sign "$JSIGN" $JDIR/Contents/MacOS/joker-player

#create dmg image
dmgbuild -s deploy-mac-settings.py -D app=${JDIR%/} "JokerPlayer" $DMGNAME

#sign dmg image
codesign -f --sign "$JSIGN" $DMGNAME

echo "Done."
