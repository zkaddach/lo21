QT += widgets
QMAKE_CXXFLAGS = -std=c++11
QMAKE_LFLAGS = -std=c++11

SOURCES += \
    main.cpp \
    automate1d/automate1d.cpp \
    automate1d/autocell1d.cpp \
    automate2d/automate2d.cpp \
    automate2d/autocell2d.cpp \
    autocell.cpp

HEADERS += \
    automate2d/automate2d.h \
    automate2d/autocell2d.h \
    automate1d/automate1d.h \
    automate1d/autocell1d.h \
    autocell.h
