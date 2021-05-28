QT       += core gui charts  qml quick quickwidgets serialport mqtt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

INCLUDEPATH += realtimeplot

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bar.cpp \
    gmap.cpp \
    ledc.cpp \
    main.cpp \
    garuda.cpp \
    qcustomplot.cpp

HEADERS += \
    bar.h \
    garuda.h \
    gmap.h \
    ledc.h \
    qcustomplot.h

FORMS += \
    garuda.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc

DISTFILES += \
    Images/TeamLogo.png \
    Images/bar.png \
    Images/bar1-06.png \
    Images/border-01.png \
    Images/border-02.png \
    Images/border-03.png \
    Images/border-04.png \
    Images/cansat.png \
    Images/greenled.png \
    Images/kalpana.png \
    Images/ledmap.jpg \
    Images/panel.png \
    Images/redled.png \
    Images/text-01.png \
    Images/text-02.png \
    Images/text-03.png \
    Images/text-04.png \
    Images/text-05.png \
    Images/text-06.png \
    Images/text-07.png \
    Images/text-08.png \
    Images/text-09.png \
    Images/text-10.png \
    Images/text-11.png \
    sampleDataFile.csv


