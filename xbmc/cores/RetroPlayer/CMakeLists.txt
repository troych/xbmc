set(SOURCES PixelConverter.cpp
            RetroPlayer.cpp
            RetroPlayerAudio.cpp
            RetroPlayerGL.cpp
            RetroPlayerVideo.cpp)

set(HEADERS PixelConverter.h
            RetroPlayer.h
            RetroPlayerAudio.h
            RetroPlayerDefines.h
            RetroPlayerGL.h
            RetroPlayerTypes.h
            RetroPlayerVideo.h)

core_add_library(retroplayer)
