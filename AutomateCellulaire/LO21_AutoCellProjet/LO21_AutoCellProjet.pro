QT += widgets
QT += core
QT += gui
QT += network
QT += multimedia
QMAKE_CXXFLAGS = -std=c++11
QMAKE_LFLAGS = -std=c++11


SOURCES += \
        main.cpp \
        autocell.cpp \
    Automate.cpp \
    AutomateManager.cpp \
    Etat.cpp \
    Simulateur.cpp

HEADERS += \
        autocell.h \
    Automate.hpp \
    AutomateManager.hpp \
    Etat.hpp \
    Simulateur.hpp
