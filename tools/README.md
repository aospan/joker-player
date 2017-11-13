# Build MAC (OSx) dmg application image

Build dmg image by following command
```
cd tools 
./deploy-mac.sh -d ../build/src/joker-player.app -i joker_player.icns -s "Developer ID Application: Abylay Ospan (6HF3M4G8WM)"
```
Available Apple developer sign identities can be found by following command
```
security find-identity -v -p codesigning
```

result will be placed into file
```
joker_player-v1.0.3.dmg
```
This dmg image is fully signed and can be distributed on the web site.
