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
include($$PWD/../3rdparty/vtextedit/src/editor/editor_export.pri)
include($$PWD/../3rdparty/vtextedit/src/libs/syntax-highlighting/syntax-highlighting_export.pri)

SOURCES += \
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
        $$PWD/youdao.cpp \
        $$PWD/translatehelperpage.cpp \
        $$PWD/translateoutputwidget.cpp \
        $$PWD/clientutils.cpp \
        $$PWD/markdowneditor4/markdowneditor4.cpp \
        $$PWD/templatemanagerdialog.cpp \
        $$PWD/template.cpp \
        $$PWD/templatemanager.cpp

HEADERS += \
        $$PWD/custompreviewthemeeditwidget.h \
        $$PWD/previewpage.h \
        $$PWD/previewthemeeditor.h \
        $$PWD/rendereddocument.h \
        $$PWD/mainwindow.h \
        $$PWD/markdownview.h \
        $$PWD/preferencedialog.h \
        $$PWD/scintillaconfig.h \
        $$PWD/settings.h \
        $$PWD/youdao.h \
        $$PWD/translatehelperpage.h \
        $$PWD/translateoutputwidget.h \
        $$PWD/clientutils.h \
        $$PWD/markdowneditor4/markdowneditor4.h \
        $$PWD/templatemanagerdialog.h \
        $$PWD/template.h \
        $$PWD/templatemanager.h

FORMS += \
        $$PWD/mainwindow.ui \
        $$PWD/preferencedialog.ui \
        $$PWD/templatemanagerdialog.ui

INCLUDEPATH += $$PWD $$PWD/markdowneditor4  $$PWD/../utils
LIBS += -L$$PWD/../utils

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

macx: {
    ICON = KarenMeu.icns
    icon.path = $$PWD
    icon.files += KarenMeu.png
    INSTALLS += icon
    LIBS += -framework CoreFoundation -framework Security -framework AppKit

    HEADERS += \
        $$PWD/macapplication.h

    SOURCES += $$PWD/macapplication.cpp

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
    DESTDIR = $$OUT_PWD
    translate.commands = '$(COPY_DIR) $$shell_path($$PWD/translations) $$shell_path($$DESTDIR)'

    qti18n.depends = translate
    qti18n.commands = '$(COPY_FILE) $$shell_path($$[QT_INSTALL_BINS]/../translations/qt_zh_CN.qm) $$shell_path($${DESTDIR}/translations/qt_zh_CN.qm)'
}    

LIBS += -lutils
