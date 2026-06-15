QT -= gui
QT += core network

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = qt_tcp_server
TEMPLATE = app

SOURCES += \
    server_main.cpp \
    server.cpp

HEADERS += \
    server.h