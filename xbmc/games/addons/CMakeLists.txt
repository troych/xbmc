set(SOURCES GameClient.cpp
            GameClientInput.cpp
            GameClientKeyboard.cpp
            GameClientProperties.cpp
            GameClientTiming.cpp
            GameClientTranslator.cpp)

set(HEADERS GameClient.h
            GameClientCallbacks.h
            GameClientInput.h
            GameClientKeyboard.h
            GameClientProperties.h
            GameClientTiming.h
            GameClientTranslator.h)

core_add_library(gameaddons)
