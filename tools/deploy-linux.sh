#!/bin/bash
# prepare AppImage file for Joker TV player for Linux
set -ex

JDIR=""
JICO=""
JSIGN=""
TMPDIR="dist_tmp"
QMAKE="/mnt/sdd/src/qt/5.9.1/gcc_64/bin/qmake"
DEPLOYQT="/mnt/sdd/src/qt/5.9.1/gcc_64/bin/linuxdeployqt"

VER="$(git describe --tags)"
IMGNAME=Joker_Player-linux-x86_64-$VER.AppImage

while getopts ":d:i:q:e:" opt; do
    case ${opt} in
        d ) # directory with joker-player
            JDIR=$OPTARG
            ;;
        i ) # iconfile
            JICO=$OPTARG
            ;;
        q ) # qmake
            QMAKE=$OPTARG
            ;;
        e ) # linuxdeployqt-continuous-x86_64.AppImage
            DEPLOYQT=$OPTARG
            ;;
        \? ) echo "Usage: cmd [-h] [-t]"
            ;;
    esac
done

if [ -z "$JDIR" ] || [ -z "$JICO" ]
then
	echo "Incorrect arguments."
	echo "Example: ./deploy-linux.sh -d ../build/src -i logo-joker-whiteborder-1024x1024.png"
	echo "Options:"
	echo "	-e /usr/local/bin/linuxdeployqt-continuous-x86_64.AppImage"
	echo "	-q /mnt/sdd/src/qt/5.9.1/gcc_64/bin/qmake"
	false
fi

echo "Processing $JDIR ..."

rm -rf $TMPDIR
mkdir -p $TMPDIR

# get absolute path
TMPDIR=$(readlink -f $TMPDIR)
JICO=$(readlink -f $JICO)
JICO_BASE=$(basename $JICO)
JICO_BASE_NO_EXT="${JICO_BASE%.*}"

cp -p $JDIR/joker-player $TMPDIR
cp -p $JICO $TMPDIR
cp -pR $JDIR/xml $TMPDIR
cp -pR /usr/lib/vlc/plugins $TMPDIR/vlc_plugins
find $TMPDIR/vlc_plugins/ -name "*.so" -exec strip {} \;

# generate desktop file
cat << EOF > $TMPDIR/joker-player.desktop
[Desktop Entry]
Type=Application
Name=Joker Player
Icon=$JICO_BASE_NO_EXT
Exec=joker-player %F
Terminal=true
EOF

# generate final AppImage
$DEPLOYQT $TMPDIR/joker-player.desktop -appimage -bundle-non-qt-libs -qmldir=$JDIR/../../src/qml/ -qmake=$QMAKE

#rename Joker_Player-x86_64.AppImage
mv Joker_Player-x86_64.AppImage $IMGNAME

echo "Done. Result: $IMGNAME"
