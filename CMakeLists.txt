cmake_minimum_required(VERSION 3.5)

project(HanoiTower VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

set(SOURCE_DIR source)

set(PROJECT_SOURCES
  ${SOURCE_DIR}/main.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(HanoiTower
        MANUAL_FINALIZATION

        resources/resources.qrc

        ${PROJECT_SOURCES}

        ${SOURCE_DIR}/HanoiStack/hanoislice.h
        ${SOURCE_DIR}/HanoiStack/hanoistack.h
        ${SOURCE_DIR}/HanoiStack/hanoistack.cpp

        ${SOURCE_DIR}/Config/config.h

        ${SOURCE_DIR}/Utils/utils.h

        ${SOURCE_DIR}/GameView/gameview.h
        ${SOURCE_DIR}/GameView/gameview.cpp

        ${SOURCE_DIR}/GameView/gameview.cpp
        ${SOURCE_DIR}/GameView/gameview.h
        ${SOURCE_DIR}/GameView/gameview_reset.cpp
        ${SOURCE_DIR}/GameView/gameview_game_mechanics.cpp
        ${SOURCE_DIR}/GameView/gameview_input_handling.cpp
        ${SOURCE_DIR}/GameView/gameview_rendering.cpp
        ${SOURCE_DIR}/GameView/gameview_scaling.cpp
        ${SOURCE_DIR}/GameView/gameview_sidebar_updater.cpp
        ${SOURCE_DIR}/GameView/gameview_autosolver.cpp
        ${SOURCE_DIR}/GameView/gameview_widget_events.cpp

        ${SOURCE_DIR}/MainWindow/mainwindow.h
        ${SOURCE_DIR}/MainWindow/mainwindow.cpp
        ${SOURCE_DIR}/MainWindow/mainwindow.ui

        ${SOURCE_DIR}/SettingsWindow/settingswindow.h
        ${SOURCE_DIR}/SettingsWindow/settingswindow.cpp
        ${SOURCE_DIR}/SettingsWindow/settingswindow.ui

        ${SOURCE_DIR}/GameWindow/gamewindow.h
        ${SOURCE_DIR}/GameWindow/gamewindow.cpp
        ${SOURCE_DIR}/GameWindow/gamewindow.ui
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET HanoiTower APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(HanoiTower SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(HanoiTower
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(HanoiTower PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.HanoiTower)
endif()
set_target_properties(HanoiTower PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS HanoiTower
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(HanoiTower)
endif()
