#-------------------------------------------------
#
# Project created by QtCreator 2011-05-05T12:41:23
#
#-------------------------------------------------

QT       += core gui widgets

CONFIG += c++17

VERSION = 4.1.7

SOURCES += \
    $$PWD/scintilla/qt/ScintillaEdit/ScintillaEdit.cpp \
    $$PWD/scintilla/qt/ScintillaEdit/ScintillaDocument.cpp \
    $$PWD/scintilla/qt/ScintillaEditBase/PlatQt.cpp \
    $$PWD/scintilla/qt/ScintillaEditBase/ScintillaQt.cpp \
    $$PWD/scintilla/qt/ScintillaEditBase/ScintillaEditBase.cpp \
    $$PWD/scintilla/src/*.cxx \
    $$PWD/scintilla/lexlib/*.cxx \
    $$PWD/scintilla/lexers/*.cxx

HEADERS  += \
    $$PWD/scintilla/qt/ScintillaEdit/ScintillaEdit.h \
    $$PWD/scintilla/qt/ScintillaEdit/ScintillaDocument.h \
    $$PWD/scintilla/qt/ScintillaEditBase/ScintillaEditBase.h \
    $$PWD/scintilla/qt/ScintillaEditBase/ScintillaQt.h \
    $$PWD/scintilla/src/*.h \
    $$PWD/scintilla/lexlib/*.h \
    $$PWD/scintilla/include/*.h


OTHER_FILES +=

INCLUDEPATH += $$PWD/scintilla/qt/ScintillaEdit \
    $$PWD/scintilla/qt/ScintillaEditBase \
    $$PWD/scintilla/include \
    $$PWD/scintilla/src \
    $$PWD/scintilla/lexlib \
    $$PWD/scintilla/include

DEFINES += SCINTILLA_QT=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1 SCI_STATIC_LINK=1 EXPORT_IMPORT_API=
CONFIG(release, debug|release) {
    DEFINES += NDEBUG=1
}

macx {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/
}
