# joker-player project

Qt based player application for Joker TV card (https://tv.jokersys.com).
Supported platforms: Linux, Mac OSx, Win32/64

(c) Abylay Ospan <aospan@jokersys.com>, 2017

(c) Ernst Maurer <ernst.maurer@gmail.com>, 2017

LICENSE: GPLv2

# Minimal build requirements

 * Qt 5.9.0
 * libjokertv 1.3.0, project source: https://github.com/aospan/libjokertv.git
 * libvlc 3.0.0, project source: https://github.com/videolan/vlc.git
 * vlc-qt, WARNING ! Please use following source with some fixes until it not upstreamed:
	https://github.com/amigo421/vlc-qt

# Compilation
```
mkdir build
cd build
cmake ../
make
```

# Run

```
./src/joker-player
```

# Build hints

Specify custom Qt installation folder before running cmake:
```
export CMAKE_PREFIX_PATH="/opt/qt/5.9.1/gcc_64/lib/cmake"
```
