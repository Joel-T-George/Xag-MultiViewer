cmake_minimum_required(VERSION 3.16)

project(xagcamera VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(PKG_CONFIG_EXECUTABLE "C:/pkg-config/bin/pkg-config.exe")


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Multimedia)
find_package(PkgConfig REQUIRED)

qt_policy(SET QTP0002 NEW)




if(ANDROID)

    set(GSTREAMER_ROOT
        "D:/gstreamer-android")

    set(GSTREAMER_ABI arm64)



    include_directories(
        ${GSTREAMER_ROOT}/${GSTREAMER_ABI}/include/gstreamer-1.0
        ${GSTREAMER_ROOT}/${GSTREAMER_ABI}/include/glib-2.0
        ${GSTREAMER_ROOT}/${GSTREAMER_ABI}/lib/glib-2.0/include
    )

    link_directories(
        ${GSTREAMER_ROOT}/${GSTREAMER_ABI}/lib
    )

else()

    set(GSTREAMER_ROOT
        "C:/gstreamer/1.0/msvc_x86_64")

    include_directories(
        ${GSTREAMER_ROOT}/include/gstreamer-1.0
        ${GSTREAMER_ROOT}/include/glib-2.0
        ${GSTREAMER_ROOT}/lib/glib-2.0/include
    )

    link_directories(
        ${GSTREAMER_ROOT}/lib
    )

endif()

# set(GSTREAMER_ROOT "C:/gstreamer/1.0/msvc_x86_64")
# include_directories(
#     ${GSTREAMER_ROOT}/include/gstreamer-1.0
#     ${GSTREAMER_ROOT}/include/glib-2.0
#     ${GSTREAMER_ROOT}/lib/glib-2.0/include
# )

# link_directories(
#     ${GSTREAMER_ROOT}/lib
# )

# pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-video-1.0)

set(TS_FILES xagcamera_en_IN.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        VideoWidget.cpp
        HomeTab.cpp
        ConnectTab.cpp
        CameraCardWidget.cpp
        CreateCameraDialog.cpp
        SkydroidDriver.cpp
        EditCameraDialog.cpp
        VideoEngine.cpp
        CameraManager.cpp
        OverlayWidget.cpp
        
        CameraDraft.h
        OverlayWidget.h
        CameraManager.h
        SkydroidDriver.h
        VideoWidget.h
        VideoEngine.h
        HomeTab.h
        ConnectTab.h
        mainwindow.h
        HomeTab.h
        CameraCardWidget.h
        CameraInstance.h
        CreateCameraDialog.h
        EditCameraDialog.h
        mainwindow.ui
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(xagcamera
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        resources.qrc
    )
    
    # set_property(TARGET xagcamera APPEND PROPERTY
    # QT_ANDROID_PACKAGE_SOURCE_DIR
    # ${CMAKE_CURRENT_SOURCE_DIR}/android



# Define target properties for Android with Qt 6 as:
#    set_property(TARGET xagcamera APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(xagcamera SHARED
            ${PROJECT_SOURCES}
        )


# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(xagcamera
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

if (ANDROID)

target_link_libraries(xagcamera
    PRIVATE Qt6::Widgets
       PRIVATE Qt6::Multimedia

       gstreamer-1.0
       gstvideo-1.0
       gobject-2.0
       glib-2.0
       gmodule-2.0
       ffi
       intl


       iconv
       pcre2-8

       log
       android
       EGL
       GLESv2
)

else()

target_link_libraries(xagcamera
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia
    gstreamer-1.0
    gstvideo-1.0
    gobject-2.0
    glib-2.0
)
endif()

set_target_properties(xagcamera PROPERTIES
    OUTPUT_NAME "xagcamera"
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.xagcamera)
endif()
set_target_properties(xagcamera PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS xagcamera
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(xagcamera)
endif()
