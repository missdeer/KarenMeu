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

include($$PWD/3rdparty/scintilla-latest.pri)

SOURCES += \
        previewpage.cpp \
        rendereddocument.cpp \
        main.cpp \
        mainwindow.cpp \
        markdowneditor.cpp \
        markdownview.cpp \
        preferencedialog.cpp \
        scintillaconfig.cpp \
        settings.cpp

HEADERS += \
        previewpage.h \
        rendereddocument.h \
        mainwindow.h \
        markdowneditor.h \
        markdownview.h \
        preferencedialog.h \
        scintillaconfig.h \
        settings.h

FORMS += \
        mainwindow.ui \
        preferencedialog.ui

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

lupdate.commands = $$QMAKE_LUPDATE -no-obsolete $$PWD/KarenMeu.pro
lupdates.depends = $$SOURCES $$HEADERS $$FORMS $$TRANSLATIONS
lrelease.commands = $$QMAKE_LRELEASE $$PWD/KarenMeu.pro
lrelease.depends = lupdate
translate.depends = lrelease
QMAKE_EXTRA_TARGETS += lupdate lrelease translate qti18n
POST_TARGETDEPS += translate qti18n

win32-*msvc* {
    QMAKE_LFLAGS += "/LTCG"
    QMAKE_CXXFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /DEBUG

    contains(QMAKE_HOST.arch, x86_64): {
        LIBS += -L$$PWD/renderer/x64
        INCLUDEPATH += $$PWD/renderer/x64
    } else: {
        LIBS += -L$$PWD/renderer/x86
        INCLUDEPATH += $$PWD/renderer/x86
    }

    CONFIG(release, debug|release) : {
        QMAKE_CXXFLAGS += /Zi
        QMAKE_LFLAGS += /INCREMENTAL:NO /Debug
        WINDEPLOYQT = $$[QT_INSTALL_BINS]/windeployqt.exe
    }
    translate.commands = '$(COPY_DIR) $$shell_path($$PWD/translations) $$shell_path($$DESTDIR/translations)'

    qti18n.depends = translate
    qti18n.commands = '$(COPY_FILE) $$shell_path($$[QT_INSTALL_BINS]/../translations/qt_zh_CN.qm) $$shell_path($${DESTDIR}/translations/qt_zh_CN.qm)'

} else: {
    INCLUDEPATH += $$PWD/renderer
    LIBS += -L$$PWD/renderer
}

macx: {
    ICON = KarenMeu.icns
    icon.path = $$PWD
    #icon.files += cjlv.png
    INSTALLS += icon
    LIBS += -framework CoreFoundation -framework Security


    CONFIG(release, debug|release) : {
        QMAKE_INFO_PLIST = osxInfo.plist
        MACDEPLOYQT = $$[QT_INSTALL_BINS]/macdeployqt

        translate.depends = lrelease
        translate.files = $$system("find $${PWD}/translations -name '*.qm' ")
        translate.path = Contents/Resources/translations/
        translate.commands = '$(COPY_DIR) $$shell_path($${PWD}/translations) $$shell_path($${DESTDIR}/$${TARGET}.app/Contents/Resources/)'

        qti18n.depends = translate
        qti18n.commands = '$(COPY_FILE) $$shell_path($$[QT_INSTALL_BINS]/../translations/qt_zh_CN.qm) $$shell_path($${DESTDIR}/$${TARGET}.app/Contents/Resources/translations/qt_zh_CN.qm)'

        bundlei18n.depends = qti18n
        bundlei18n.commands = '$(COPY_DIR) $$shell_path($${PWD}/../../mac/*.lproj) $$shell_path($${DESTDIR}/$${TARGET}.app/Contents/Resources/)'
        QMAKE_BUNDLE_DATA += translate qti18n bundlei18n

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

        QMAKE_EXTRA_TARGETS += deploy deploy_webengine deploy_appstore fixdeploy codesign makedmg bundlei18n
        POST_TARGETDEPS += bundlei18n
    }
}

unix: !macx {
    translate.commands = '$(COPY_DIR) $$shell_path($$PWD/translations) $$shell_path($$DESTDIR)'
}    

LIBS += -lrenderer

DISTFILES += \
    rc/images/codeblock.png \
    rc/images/paragraph.png \
    rc/images/table.png \
    rc/images/todolist.png
