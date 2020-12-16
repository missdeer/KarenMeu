INCLUDEPATH += $$PWD/scintilla/qt/ScintillaEdit \
    $$PWD/scintilla/qt/ScintillaEditBase \
    $$PWD/scintilla/include \
    $$PWD/scintilla/src \
    $$PWD/scintilla/lexlib \
    $$PWD/scintilla/include

DEFINES += SCINTILLA_QT=1 MAKING_LIBRARY=1 SCI_LEXER=1 _CRT_SECURE_NO_DEPRECATE=1
CONFIG(release, debug|release) {
    DEFINES += NDEBUG=1
}

macx {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/
}

OUT_FOLDER = $$absolute_path($$relative_path($$PWD, $$_PRO_FILE_PWD_), $$OUT_PWD)/scintilla/bin
lib_name = ScintillaEdit
macx {
    lib_framework_name = $${lib_name}.framework

    INCLUDEPATH += $${OUT_FOLDER}/$${lib_framework_name}/Headers
    LIBS += -F$${OUT_FOLDER} -framework $${lib_name}
} else {
    INCLUDEPATH *= $$PWD/include
    win32:CONFIG(release, debug|release) {
        LIBS += -L$$OUT_FOLDER/release -l$${lib_name}
    } else:win32:CONFIG(debug, debug|release) {
        LIBS += -L$$OUT_FOLDER/debug -l$${lib_name}
    } else:unix {
        LIBS += -L$$OUT_FOLDER -l$${lib_name}
    }
}
