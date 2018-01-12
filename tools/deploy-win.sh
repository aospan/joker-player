#!/bin/bash
# prepare exe+dll for Joker TV player for Windows
set -ex

JDIR=""
JICO=""
JSIGN=""
TMPDIR="dist_tmp"
DEPLOYQT="/mingw64/bin/windeployqt.exe"

VER="$(git describe --tags)"
IMGNAME=Joker_Player-Windows-x86_64-$VER

while getopts ":d:i:q:e:" opt; do
    case ${opt} in
        d ) # directory with joker-player
            JDIR=$OPTARG
            ;;
        i ) # iconfile
            JICO=$OPTARG
            ;;
        \? ) echo "Usage: cmd"
            ;;
    esac
done

if [ -z "$JDIR" ]
then
	echo "Incorrect arguments."
	echo "Example: ./deploy-win.sh -d ../build/src"
	false
fi

echo "Processing $JDIR ..."

rm -rf $IMGNAME
rm -rf $TMPDIR
mkdir -p $TMPDIR

# get absolute path
TMPDIR=$(readlink -f $TMPDIR)
cp -p $JDIR/joker-player.exe $TMPDIR
cp -pR $JDIR/xml $TMPDIR
cp -pR /mingw64/lib/vlc/plugins $TMPDIR/vlc_plugins

$DEPLOYQT --release --compiler-runtime --qmldir $JDIR/../../src/qml/ $TMPDIR/joker-player.exe 

#copy dependency dll's
pushd $TMPDIR
for x in `cygcheck.exe ./joker-player.exe  | grep mingw64 | awk.exe  '{print $1}'`; do echo $x; cp $x ./; done
cp /usr/local/bin/libjokertv.dll ./
cp /usr/local/bin/libVLCQt* ./
popd

#rename 
mv $TMPDIR $IMGNAME

echo "Done. Result: $IMGNAME"
