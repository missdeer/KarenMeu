INCLUDEPATH += $$PWD/lexilla/include \
    $$PWD/lexilla/src \
    $$PWD/lexilla/lexlib

SOURCES += \
    $$files($$PWD/lexilla/src/*.cxx, false)  \
    $$files($$PWD/lexilla/lexlib/*.cxx, false)  \
    $$files($$PWD/lexilla/lexers/*.cxx, false)

HEADERS += \
    $$files($$PWD/lexilla/include/*.h, false)

RC_FILE += $$PWD/lexilla/src/LexillaVersion.rc

DEFINES += SCINTILLA_QT=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1 _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES=1 _SCL_SECURE_NO_WARNINGS=1
CONFIG(release, debug|release) {
    DEFINES += NDEBUG=1
}

macx {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
}

OUT_FOLDER = $$absolute_path($$relative_path($$PWD, $$_PRO_FILE_PWD_), $$OUT_PWD)/lexilla/bin
lib_name = ScintillaEdit
macx {
    lib_framework_name = $${lib_name}.framework

    INCLUDEPATH += $${OUT_FOLDER}/$${lib_framework_name}/Headers
    LIBS += -F$${OUT_FOLDER} -framework $${lib_name}
} else {
    INCLUDEPATH *= $$PWD/include
    win32: {
        LIBS += -L$$OUT_FOLDER -l$${lib_name}5
    } else: {
        LIBS += -L$$OUT_FOLDER -l$${lib_name}
    }
}
