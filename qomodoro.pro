QT += core
QT += gui
QT += multimedia
QT += widgets

TARGET = qomodoro
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

SOURCES += \
    main.cpp \
    preferencesdialog.cpp \
    settings.cpp \
    application.cpp \
    statemachine.cpp

HEADERS += \
    preferencesdialog.h \
    settings.h \
    application.h \
    statemachine.h

FORMS += \
    preferencesdialog.ui


qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
