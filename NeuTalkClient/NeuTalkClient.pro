QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addfriend.cpp \
    chatwindow.cpp \
    clientsocket.cpp \
    logindlg.cpp \
    main.cpp \
    mainwindow.cpp \
    qnchatmessage.cpp \
    registerdlg.cpp \
    registerdlgreal.cpp \
    smtp.cpp

HEADERS += \
    addfriend.h \
    chatwindow.h \
    clientsocket.h \
    logindlg.h \
    mainwindow.h \
    qnchatmessage.h \
    registerdlg.h \
    registerdlgreal.h \
    smtp.h

FORMS += \
    addfriend.ui \
    chatwindow.ui \
    logindlg.ui \
    mainwindow.ui \
    registerdlg.ui \
    registerdlgreal.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc
