QT -= gui
QT += core sql network

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
    objects.cpp \
    server.cpp

#LIBS += C:\Qt\5.10.0\mingw53_32\lib\libws2_32.a
#LIBS += -fopenmp
LIBS += C:\Qt\Qt5.8.0\Projects\schedule\qhttpserver\lib\libqhttpserver.a
LIBS += C:\Qt\Qt5.8.0\Tools\mingw32\plugins\sqldrivers\qsqlite.dll
LIBS += C:\Qt\Qt5.8.0\Tools\mingw32\plugins\sqldrivers\qsqlited.dll
#QMAKE_CXXFLAGS += -fopenmp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    objects.h \
    server.h
