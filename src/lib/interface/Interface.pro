TEMPLATE = app TARGET = name_of_the_app
QT = core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets charts

SOURCES += \
	Contexte.cpp \
    AffichageReseau.cpp \
    ChoixReseau.cpp \
    ConfigReseau.cpp \
    DonneeReseau.cpp \
    FenetreReseau.cpp \
    MenuEntete.cpp \
    MyWindow.cpp \
    main.cpp \

HEADERS += \
	Contexte.h \
    AffichageReseau.h \
    ChoixReseau.h \
    ConfigReseau.h \
    DonneeReseau.h \
    FenetreReseau.h \
    MenuEntete.h \
    MyWindow.h

DISTFILES +=