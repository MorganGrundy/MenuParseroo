TEMPLATE = app

TARGET = MenuParseroo

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QMAKE_CXXFLAGS += /std:c++17

DEFINES += _USE_MATH_DEFINES

QMAKE_CXXFLAGS_RELEASE += -MT

# Tesseract and Leptonica libraries
VCPKG_DIR = C:/vcpkg/vcpkg/installed/x64-windows-static
INCLUDEPATH += $${VCPKG_DIR}/include
LIBS += -L$${VCPKG_DIR}/lib \
-larchive \
-lbz2 \
-lcharset \
-lgif \
-liconv \
-ljpeg \
-lleptonica-1.80.0 \
-llibcrypto \
-llibpng16 \
-llibssl \
-llibwebpmux \
-llibxml2 \
-llz4 \
-llzma \
-llzo2 \
-lopenjp2 \
-ltesseract41 \
-ltiff \
-ltiffxx \
-lturbojpeg \
-lwebp \
-lwebpdecoder \
-lwebpdemux \
-lxxhash \
-lzlib \
-lzstd_static

# OpenCV libraries
INCLUDEPATH += $$(OPENCV_DIR)/../../include
CONFIG( debug, debug|release ) {
	# debug
	LIBS += -L$$(OPENCV_DIR)/lib \
	-lopencv_core440d \
	-lopencv_imgcodecs440d \
	-lopencv_imgproc440d \
	-lopencv_features2d440d \
	-lopencv_objdetect440d
} else {
	# release
	LIBS += -L$$(OPENCV_DIR)/lib \
	-lopencv_core440 \
	-lopencv_imgcodecs440 \
	-lopencv_imgproc440 \
	-lopencv_features2d440 \
	-lopencv_objdetect440 \
	-lopencv_highgui440
}

include(logic/logic.pri)
include(gui/gui.pri)

SOURCES += main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
	data/tesseract.config \
	data/eng.user-words \
	data/eng.user-patterns

# Copy tesseract config, user-words, and user-patterns file to build dir
copydata.commands = $(COPY_FILE) \"$$shell_path($$PWD\\data)\" \"$$shell_path($$OUT_PWD\\release)\"
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata
