include(E:\CFU\Qt-Secret\src\Qt-Secret.pri)

QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    invitesystem.cpp \
    main.cpp \
    mainwindow.cpp \
    messagesystem.cpp \
    msg.cpp \
    registrationform.cpp

HEADERS += \
    invitesystem.h \
    mainwindow.h \
    messagesystem.h \
    msg.h \
    registrationform.h

FORMS += \
    invitesystem.ui \
    mainwindow.ui \
    messagesystem.ui \
    registrationform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
