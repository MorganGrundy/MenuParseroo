#Application version
VERSION_MAJOR = 0
VERSION_MINOR = 1
VERSION_BUILD = 0

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR" \
	"VERSION_MINOR=$$VERSION_MINOR" \
	"VERSION_BUILD=$$VERSION_BUILD"

#Target version
VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}

FORMS += \
$$PWD/mainwindow.ui

HEADERS += \
$$PWD/graphicsfontmetricitem.h \
$$PWD/ocrgraphicsview.h \
$$PWD/mainwindow.h

SOURCES += \
$$PWD/graphicsfontmetricitem.cpp \
$$PWD/ocrgraphicsview.cpp \
$$PWD/mainwindow.cpp

INCLUDEPATH += $$PWD
