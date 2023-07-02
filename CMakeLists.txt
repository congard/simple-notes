cmake_minimum_required(VERSION 3.26)
project(simple_notes)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

message(STATUS "Using Qt${QT_VERSION_MAJOR}")

add_subdirectory(${FIREBASE_SDK} firebase_cpp_sdk)
add_subdirectory(contrib/QtAwesome)

set(PROJECT_SOURCES
        src/ui/resources/resources.qrc
        src/main.cpp
        src/ui/MainWindow.cpp src/ui/MainWindow.h
        src/backend/provider/DatabaseProvider.cpp src/backend/provider/DatabaseProvider.h
        src/backend/FirebaseApp.cpp src/backend/FirebaseApp.h
        src/backend/provider/AuthProvider.cpp src/backend/provider/AuthProvider.h
        src/backend/Note.cpp src/backend/Note.h
        src/backend/IStatusHandler.h
        src/backend/StatusHandler.h
        src/backend/provider/NoteProvider.cpp src/backend/provider/NoteProvider.h
        src/backend/Error.h
        src/backend/ErrorCode.h
        src/backend/IdGenerator.cpp src/backend/IdGenerator.h
        src/ui/NoteWidget.cpp src/ui/NoteWidget.h
        src/ui/App.cpp src/ui/App.h
        src/ui/ErrorBox.cpp src/ui/ErrorBox.h
        src/ui/NoteEditorDialog.cpp src/ui/NoteEditorDialog.h
        src/ui/AuthDialog.cpp src/ui/AuthDialog.h
        src/ui/AboutDialog.cpp src/ui/AboutDialog.h
        src/ui/HelpDialog.cpp src/ui/HelpDialog.h src/Details.h)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(simple_notes ${PROJECT_SOURCES})
else()
    add_executable(simple_notes ${PROJECT_SOURCES})
endif()

target_link_libraries(simple_notes PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        QtAwesome
        firebase_database
        firebase_auth
        firebase_app)

target_include_directories(simple_notes PRIVATE
        src
        contrib/QtAwesome/QtAwesome)