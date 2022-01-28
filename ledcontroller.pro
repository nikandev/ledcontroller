QT += core gui network widgets

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    8266.ino \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    log.txt \
    settings.ini

RESOURCES += \
    resources.qrc
