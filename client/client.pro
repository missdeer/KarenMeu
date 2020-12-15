#-------------------------------------------------
#
# Project created by QtCreator 2019-09-02T14:18:24
#
#-------------------------------------------------

QT       += core gui widgets network xml webengine webenginewidgets webchannel

TARGET = KarenMeu
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17

include($$PWD/../3rdparty/scintilla-latest.pri)

SOURCES += \
        $$PWD/markdowneditor2/ColorHelper.cpp \
        $$PWD/markdowneditor2/HighlightTokenizer.cpp \
        $$PWD/markdowneditor2/MarkdownHighlighter.cpp \
        $$PWD/markdowneditor2/MarkdownTokenizer.cpp \
        $$PWD/markdowneditor2/TextDocument.cpp \
        $$PWD/markdowneditor2/Theme.cpp \
        $$PWD/markdowneditor2/ThemeFactory.cpp \
        $$PWD/markdowneditor2/Token.cpp \
        $$PWD/markdowneditor2/markdowneditor.cpp \
        $$PWD/markdowneditor2/markdowneditor2.cpp \
        $$PWD/custompreviewthemeeditwidget.cpp \
        $$PWD/previewpage.cpp \
        $$PWD/previewthemeeditor.cpp \
        $$PWD/rendereddocument.cpp \
        $$PWD/main.cpp \
        $$PWD/mainwindow.cpp \
        $$PWD/markdownview.cpp \
        $$PWD/preferencedialog.cpp \
        $$PWD/scintillaconfig.cpp \
        $$PWD/settings.cpp \
        $$PWD/markdowneditor2/spelling/dictionary_manager.cpp \
        $$PWD/markdowneditor2/spelling/spell_checker.cpp \
        $$PWD/youdao.cpp

HEADERS += \
        $$PWD/markdowneditor2/ColorHelper.h \
        $$PWD/markdowneditor2/HighlightTokenizer.h \
        $$PWD/markdowneditor2/HighlighterLineStates.h \
        $$PWD/markdowneditor2/MarkdownEditorTypes.h \
        $$PWD/markdowneditor2/MarkdownHighlighter.h \
        $$PWD/markdowneditor2/MarkdownStates.h \
        $$PWD/markdowneditor2/MarkdownStyles.h \
        $$PWD/markdowneditor2/MarkdownTokenTypes.h \
        $$PWD/markdowneditor2/MarkdownTokenizer.h \
        $$PWD/markdowneditor2/TextBlockData.h \
        $$PWD/markdowneditor2/TextDocument.h \
        $$PWD/markdowneditor2/Theme.h \
        $$PWD/markdowneditor2/ThemeFactory.h \
        $$PWD/markdowneditor2/Token.h \
        $$PWD/markdowneditor2/markdowneditor.h \
        $$PWD/markdowneditor2/markdowneditor2.h \
        $$PWD/custompreviewthemeeditwidget.h \
        $$PWD/previewpage.h \
        $$PWD/previewthemeeditor.h \
        $$PWD/rendereddocument.h \
        $$PWD/mainwindow.h \
        $$PWD/markdownview.h \
        $$PWD/preferencedialog.h \
        $$PWD/scintillaconfig.h \
        $$PWD/settings.h \
        $$PWD/markdowneditor2/spelling/abstract_dictionary.h \
        $$PWD/markdowneditor2/spelling/abstract_dictionary_provider.h \
        $$PWD/markdowneditor2/spelling/dictionary_manager.h \
        $$PWD/markdowneditor2/spelling/dictionary_ref.h \
        $$PWD/markdowneditor2/spelling/spell_checker.h \
        $$PWD/youdao.h

FORMS += \
        $$PWD/mainwindow.ui \
        $$PWD/preferencedialog.ui

INCLUDEPATH += $$PWD $$PWD/markdowneditor2 $$PWD/markdowneditor2/spelling

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    karenmeu.qrc

RC_FILE = KarenMeu.rc

CODECFORTR      = UTF-8
CODECFORSRC     = UTF-8
TRANSLATIONS    = $$PWD/translations/KarenMeu_zh_CN.ts

isEmpty(QMAKE_LUPDATE) {
    win32:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]\lupdate.exe
    else:QMAKE_LUPDATE = $$[QT_INSTALL_BINS]/lupdate
}

isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

lupdate.commands = $$QMAKE_LUPDATE -no-obsolete $$PWD/client.pro
lupdates.depends = $$SOURCES $$HEADERS $$FORMS $$TRANSLATIONS
lrelease.commands = $$QMAKE_LRELEASE $$PWD/client.pro
lrelease.depends = lupdate
translate.depends = lrelease
QMAKE_EXTRA_TARGETS += lupdate lrelease translate qti18n
POST_TARGETDEPS += translate qti18n

win32-*msvc* {
    #QMAKE_LFLAGS += "/LTCG"
    QMAKE_CXXFLAGS_RELEASE += /Zi
    #QMAKE_LFLAGS_RELEASE += /DEBUG

    CONFIG(release, debug|release) : {
        WINDEPLOYQT = $$[QT_INSTALL_BINS]/windeployqt.exe
        DESTDIR = $$OUT_PWD/release
    } else : {
        DESTDIR = $$OUT_PWD/debug
    }
    translate.commands = '$(COPY_DIR) $$shell_path($$PWD/translations) $$shell_path($$DESTDIR/translations)'

    qti18n.depends = translate
    qti18n.commands = '$(COPY_FILE) $$shell_path($$[QT_INSTALL_BINS]/../translations/qt_zh_CN.qm) $$shell_path($${DESTDIR}/translations/qt_zh_CN.qm)'
}
INCLUDEPATH += $$PWD/../utils
LIBS += -L$$PWD/../utils

win32 {
    INCLUDEPATH += $$PWD/markdowneditor2/spelling/hunspell

    HEADERS += $$PWD/markdowneditor2/spelling/dictionary_provider_hunspell.h \
        $$PWD/markdowneditor2/spelling/dictionary_provider_voikko.h

    SOURCES += $$PWD/markdowneditor2/spelling/dictionary_provider_hunspell.cpp \
        $$PWD/markdowneditor2/spelling/dictionary_provider_voikko.cpp \
        $$PWD/markdowneditor2/spelling/hunspell/affentry.cxx \
        $$PWD/markdowneditor2/spelling/hunspell/affixmgr.cxx \
        $$PWD/markdowneditor2/spelling/hunspell/csutil.cxx \
        $$PWD/markdowneditor2/spelling/hunspell/filemgr.cxx \
        $$PWD/markdowneditor2/spelling/hunspell/hashmgr.cxx \
        $$PWD/markdowneditor2/spelling/hunspell/hunspell.cxx \
        $$PWD/markdowneditor2/spelling/hunspell/hunzip.cxx \
        $$PWD/markdowneditor2/spelling/hunspell/phonet.cxx \
        $$PWD/markdowneditor2/spelling/hunspell/replist.cxx \
        $$PWD/markdowneditor2/spelling/hunspell/suggestmgr.cxx

    DEFINES += HUNSPELL_STATIC
} 

macx: {
    ICON = KarenMeu.icns
    icon.path = $$PWD
    icon.files += KarenMeu.png
    INSTALLS += icon
    LIBS += -framework CoreFoundation -framework Security -framework AppKit

    HEADERS += $$PWD/markdowneditor2/spelling/dictionary_provider_nsspellchecker.h \
        $$PWD/macapplication.h

    SOURCES += $$PWD/macapplication.cpp

    OBJECTIVE_SOURCES += $$PWD/markdowneditor2/spelling/dictionary_provider_nsspellchecker.mm
    DESTDIR = $$OUT_PWD
    QMAKE_INFO_PLIST = osxInfo.plist
    CONFIG(release, debug|release) : {
        MACDEPLOYQT = $$[QT_INSTALL_BINS]/macdeployqt

        translate.depends = lrelease
        translate.files = $$system("find $${PWD}/translations -name '*.qm' ")
        translate.path = Contents/Resources/translations/
        translate.commands = '$(COPY_DIR) $$shell_path($${PWD}/translations) $$shell_path($${DESTDIR}/$${TARGET}.app/Contents/Resources/)'

        qti18n.depends = translate
        qti18n.commands = '$(COPY_FILE) $$shell_path($$[QT_INSTALL_BINS]/../translations/qt_zh_CN.qm) $$shell_path($${DESTDIR}/$${TARGET}.app/Contents/Resources/translations/qt_zh_CN.qm)'

        QMAKE_BUNDLE_DATA += translate qti18n 

        deploy.commands += $$MACDEPLOYQT \"$${DESTDIR}/$${TARGET}.app\"

        deploy_appstore.depends += deploy
        deploy_appstore.commands += $$MACDEPLOYQT \"$${DESTDIR}/$${TARGET}.app\" -appstore-compliant

        deploy_webengine.depends += deploy_appstore
        deploy_webengine.commands += $$MACDEPLOYQT \"$${DESTDIR}/$${TARGET}.app/Contents/Frameworks/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app\"

        fixdeploy.depends += deploy_webengine
        fixdeploy.commands += $$PWD/../macdeploy/macdeploy \"$${DESTDIR}/$${TARGET}.app\"

        APPCERT = Developer ID Application: Fan Yang (Y73SBCN2CG)
        INSTALLERCERT = 3rd Party Mac Developer Installer: Fan Yang (Y73SBCN2CG)
        BUNDLEID = info.minidump.karenmeu

        codesign.depends += fixdeploy
        codesign.commands = codesign -s \"$${APPCERT}\" -v -f --timestamp=none --deep \"$${DESTDIR}/$${TARGET}.app\"

        makedmg.depends += codesign
        makedmg.commands = hdiutil create -srcfolder \"$${DESTDIR}/$${TARGET}.app\" -volname \"$${TARGET}\" -format UDBZ \"$${DESTDIR}/$${TARGET}.dmg\" -ov -scrub -stretch 2g

        QMAKE_EXTRA_TARGETS += deploy deploy_webengine deploy_appstore fixdeploy codesign makedmg 
    }
} 

unix: !macx {
    translate.commands = '$(COPY_DIR) $$shell_path($$PWD/translations) $$shell_path($$DESTDIR)'
}    

LIBS += -lutils
