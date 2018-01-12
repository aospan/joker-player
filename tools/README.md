# Build MAC (OSx) dmg application image

Build dmg image by following command
```
security unlock-keychain
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

Open dmg file and drag "joker-player" to the Applications folder on your computer.

![](mac-install.png "Joker Player install on MAC")

# Build Linux application image (AppImage format)

Build AppImage by following command
```
cd tools 
./deploy-linux.sh -d ../build/src -i ./logo-joker-whiteborder-1024x1024.png
```

result will be placed into file
```
Joker_Player-linux-x86_64-v1.0.4-2-gca89066.AppImage
```

From https://appimage.org/ about AppImage format:
"The key idea of the AppImage format is one app = one file. Every AppImage contains an app and all the files the app needs to run. In other words, each AppImage has no dependencies other than what is included in the targeted base operating system(s)."
