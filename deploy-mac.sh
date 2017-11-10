#!/bin/bash
# prepare .dmg file for Joker TV player for OSx
set -e

if [ -z "$1" ]
then
	echo "Please specify folder with joker-player.app. Example: ./deploy-mac.sh ./build/src/"
	false
fi

echo "Processing ..."

pushd $1 > /dev/null
install_name_tool -add_rpath /usr/lib/ ./joker-player.app/Contents/MacOS/joker-player
install_name_tool -add_rpath /usr/local/lib/ ./joker-player.app/Contents/MacOS/joker-player

cp -pR xml ./joker-player.app/Contents/MacOS/
cp -pR /Applications/VLC.app/Contents/MacOS/plugins ./joker-player.app/Contents/Frameworks/

/usr/local/opt/qt//bin/macdeployqt joker-player.app -libpath=/usr/local/ -dmg -always-overwrite -qmldir=../../src/qml/
echo "Done. Resulting file $1/joker-player.dmg"
