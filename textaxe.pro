QT        += core gui widgets network
CONFIG    += c++11

TARGET     = textaxe
TEMPLATE   = app

SOURCES   += main.cpp mainwindow.cpp networkdialog.cpp urldialog.cpp codingmanager.cpp textedit.cpp
HEADERS   += mainwindow.h networkdialog.h urldialog.h codingmanager.h textedit.h
FORMS     += mainwindow.ui networkdialog.ui urldialog.ui
RESOURCES += resources.qrc
DISTFILES += resources.rc
RC_FILE    = resources.rc

include(libcharguess/libcharguess.pri)
