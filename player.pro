#-------------------------------------------------
#
# Project created by QtCreator 2014-08-11T10:01:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = player
TEMPLATE = app

DEFINES += PLANA

SOURCES += main.cpp\
        player.cpp \
    playcontrols.cpp \
    tools.cpp \
    videoplayer.cpp

HEADERS  += player.h \
    playcontrols.h \
    tools.h \
    videoplayer.h

FORMS    += player.ui \
    playcontrols.ui

RESOURCES += \
    resources.qrc

CONFIG += link_pkgconfig
PKGCONFIG += gstreamer-1.0

target.path = /opt/VideoPlayer
target.files = player Atmel-Video-Player.sh
configfile.path = /opt/ApplicationLauncher/applications/xml
configfile.files = configs/7-atmel-video-player.xml
imagefile.path = /opt/ApplicationLauncher/applications/resources
imagefile.files = configs/atmel-video-player.png
mediafiles.path = /opt/VideoPlayer/media
mediafiles.files = media/Microchip-masters.mp4
INSTALLS += target configfile imagefile mediafiles
