QT += core widgets serialport printsupport

TARGET = interface

DESTDIR = bin

OBJECTS_DIR = build/obj

MOC_DIR = build/moc

SOURCES += src/main.cpp \
           src/mainwindow.cpp \
           src/serialhandler.cpp \
           src/mh.c \
           src/cobs.c \
           src/utl_crc16.c \
           lib/QCustomPlot/qcustomplot.cpp

HEADERS += src/mainwindow.h \
           src/serialhandler.h \
           src/mh.h \
           src/cobs.h \
           src/utl_crc16.h \
           lib/QCustomPlot/qcustomplot.h

FORMS += ui/mainwindow.ui
