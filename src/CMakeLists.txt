project(joker-player)

option(USE_STUB_JOKER "Build with fake joker provider" OFF)
option(USE_DEVELOPMENT_BUILD "Build with development features" OFF)

find_package(Qt5 5.9.0 REQUIRED COMPONENTS Core Quick Sql)
find_package(VLCQt REQUIRED COMPONENTS Qml)

if(NOT USE_STUB_JOKER)
    find_package(jokertv 1.3.0 REQUIRED)

    if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        find_library(LIBUDEV udev REQUIRED)
    endif()

endif(NOT USE_STUB_JOKER)

if(USE_DEVELOPMENT_BUILD)
    add_definitions(-DUSE_DEVELOPMENT_BUILD)
endif(USE_DEVELOPMENT_BUILD)

add_definitions(
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_ASCII
    -DQT_STRICT_ITERATORS
)

# It is used for Windows only with custom
# libvlc SDK location
include_directories(${LIBVLC_SDK_DIR}/include)
link_directories(${LIBVLC_SDK_DIR}/lib)

set(Project_Sources
    jokerplayerapp.cpp
    jokerplayer.cpp
    jokerprogrammodel.cpp
    jokerprogram.cpp
    jokeraccessprovider.cpp
    jokersettingsstorage.cpp
    jokertelevision.cpp
    jokerchannel.cpp
    jokerstandardsmodel.cpp
    jokersatellitesmodel.cpp
    jokerchannelsmodel.cpp
)

if(USE_STUB_JOKER)
    SET(Project_Sources
        jokeraccessprovider_stub.cpp
        ${Project_Sources}
    )
    add_definitions(
        -DUSE_STUB_JOKER
    )
else(USE_STUB_JOKER)
    SET(Project_Sources
        jokeraccessprovider_libusb.cpp
        ${Project_Sources}
    )
endif(USE_STUB_JOKER)

set(Project_Headers
    jokerplayer.h
    jokerprogrammodel.h
    jokerprogram.h
    jokeraccessprovider.h
    jokeraccessprovider_p.h
    jokersettingsstorage.h
    jokertelevision.h
    jokerchannel.h
    jokerstandardsmodel.h
    jokersatellitesmodel.h
    jokerchannelsmodel.h
)

set(Project_Resources
    jokerplayerapp.qrc
)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE WIN32
    ${Project_Headers}
    ${Project_Sources}
    ${Project_Resources}
    jokerplayerapp.rc
)

if(USE_STUB_JOKER)
    target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick Qt5::Sql VLCQt::Qml)
else(USE_STUB_JOKER)
    target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick Qt5::Sql VLCQt::Qml jokertv vlc ${LIBUDEV})
endif(USE_STUB_JOKER)

# Deploy XML files to destination directory
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/xml ${CMAKE_CURRENT_BINARY_DIR}/xml
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/xml
    COMMENT "Copy XML resources"
)
