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
    $$PWD/scintilla/src/AutoComplete.cxx     \
    $$PWD/scintilla/src/CallTip.cxx          \
    $$PWD/scintilla/src/CaseConvert.cxx      \
    $$PWD/scintilla/src/CaseFolder.cxx       \
    $$PWD/scintilla/src/Catalogue.cxx        \
    $$PWD/scintilla/src/CellBuffer.cxx       \
    $$PWD/scintilla/src/CharClassify.cxx     \
    $$PWD/scintilla/src/ContractionState.cxx \
    $$PWD/scintilla/src/DBCS.cxx             \
    $$PWD/scintilla/src/Decoration.cxx       \
    $$PWD/scintilla/src/Document.cxx         \
    $$PWD/scintilla/src/EditModel.cxx        \
    $$PWD/scintilla/src/Editor.cxx           \
    $$PWD/scintilla/src/EditView.cxx         \
    $$PWD/scintilla/src/ExternalLexer.cxx    \
    $$PWD/scintilla/src/Indicator.cxx        \
    $$PWD/scintilla/src/KeyMap.cxx           \
    $$PWD/scintilla/src/LineMarker.cxx       \
    $$PWD/scintilla/src/MarginView.cxx       \
    $$PWD/scintilla/src/PerLine.cxx          \
    $$PWD/scintilla/src/PositionCache.cxx    \
    $$PWD/scintilla/src/RESearch.cxx         \
    $$PWD/scintilla/src/RunStyles.cxx        \
    $$PWD/scintilla/src/ScintillaBase.cxx    \
    $$PWD/scintilla/src/Selection.cxx        \
    $$PWD/scintilla/src/Style.cxx            \
    $$PWD/scintilla/src/UniConversion.cxx    \
    $$PWD/scintilla/src/UniqueString.cxx     \
    $$PWD/scintilla/src/ViewStyle.cxx        \
    $$PWD/scintilla/src/XPM.cxx              \
    $$PWD/scintilla/lexlib/Accessor.cxx          \
    $$PWD/scintilla/lexlib/CharacterCategory.cxx \
    $$PWD/scintilla/lexlib/CharacterSet.cxx      \
    $$PWD/scintilla/lexlib/DefaultLexer.cxx      \
    $$PWD/scintilla/lexlib/LexerBase.cxx         \
    $$PWD/scintilla/lexlib/LexerModule.cxx       \
    $$PWD/scintilla/lexlib/LexerNoExceptions.cxx \
    $$PWD/scintilla/lexlib/LexerSimple.cxx       \
    $$PWD/scintilla/lexlib/PropSetSimple.cxx     \
    $$PWD/scintilla/lexlib/StyleContext.cxx      \
    $$PWD/scintilla/lexlib/WordList.cxx          \
    $$PWD/scintilla/lexers/LexCSS.cxx      \ 
    $$PWD/scintilla/lexers/LexHTML.cxx     \ 
    $$PWD/scintilla/lexers/LexJSON.cxx     \ 
    $$PWD/scintilla/lexers/LexMarkdown.cxx \ 
    $$PWD/scintilla/lexers/LexYAML.cxx      

HEADERS  += \
    $$PWD/scintilla/qt/ScintillaEdit/ScintillaEdit.h \
    $$PWD/scintilla/qt/ScintillaEdit/ScintillaDocument.h \
    $$PWD/scintilla/qt/ScintillaEditBase/ScintillaEditBase.h \
    $$PWD/scintilla/qt/ScintillaEditBase/ScintillaQt.h \
    $$PWD/scintilla/src/AutoComplete.h     \
    $$PWD/scintilla/src/CallTip.h          \
    $$PWD/scintilla/src/CaseConvert.h      \
    $$PWD/scintilla/src/CaseFolder.h       \
    $$PWD/scintilla/src/Catalogue.h        \
    $$PWD/scintilla/src/CellBuffer.h       \
    $$PWD/scintilla/src/CharClassify.h     \
    $$PWD/scintilla/src/ContractionState.h \
    $$PWD/scintilla/src/DBCS.h             \
    $$PWD/scintilla/src/Decoration.h       \
    $$PWD/scintilla/src/Document.h         \
    $$PWD/scintilla/src/EditModel.h        \
    $$PWD/scintilla/src/Editor.h           \
    $$PWD/scintilla/src/EditView.h         \
    $$PWD/scintilla/src/ElapsedPeriod.h    \
    $$PWD/scintilla/src/ExternalLexer.h    \
    $$PWD/scintilla/src/FontQuality.h      \
    $$PWD/scintilla/src/Indicator.h        \
    $$PWD/scintilla/src/IntegerRectangle.h \
    $$PWD/scintilla/src/KeyMap.h           \
    $$PWD/scintilla/src/LineMarker.h       \
    $$PWD/scintilla/src/MarginView.h       \
    $$PWD/scintilla/src/Partitioning.h     \
    $$PWD/scintilla/src/PerLine.h          \
    $$PWD/scintilla/src/Position.h         \
    $$PWD/scintilla/src/PositionCache.h    \
    $$PWD/scintilla/src/RESearch.h         \
    $$PWD/scintilla/src/RunStyles.h        \
    $$PWD/scintilla/src/ScintillaBase.h    \
    $$PWD/scintilla/src/Selection.h        \
    $$PWD/scintilla/src/SparseVector.h     \
    $$PWD/scintilla/src/SplitVector.h      \
    $$PWD/scintilla/src/Style.h            \
    $$PWD/scintilla/src/UniConversion.h    \
    $$PWD/scintilla/src/UniqueString.h     \
    $$PWD/scintilla/src/ViewStyle.h        \
    $$PWD/scintilla/src/XPM.h              \
    $$PWD/scintilla/lexlib/Accessor.h          \
    $$PWD/scintilla/lexlib/CharacterCategory.h \
    $$PWD/scintilla/lexlib/CharacterSet.h      \
    $$PWD/scintilla/lexlib/DefaultLexer.h      \
    $$PWD/scintilla/lexlib/LexAccessor.h       \
    $$PWD/scintilla/lexlib/LexerBase.h         \
    $$PWD/scintilla/lexlib/LexerModule.h       \
    $$PWD/scintilla/lexlib/LexerNoExceptions.h \
    $$PWD/scintilla/lexlib/LexerSimple.h       \
    $$PWD/scintilla/lexlib/OptionSet.h         \
    $$PWD/scintilla/lexlib/PropSetSimple.h     \
    $$PWD/scintilla/lexlib/SparseState.h       \
    $$PWD/scintilla/lexlib/StringCopy.h        \
    $$PWD/scintilla/lexlib/StyleContext.h      \
    $$PWD/scintilla/lexlib/SubStyles.h         \
    $$PWD/scintilla/lexlib/WordList.h          \
    $$PWD/scintilla/include/ILexer.h          \
    $$PWD/scintilla/include/ILoader.h         \
    $$PWD/scintilla/include/Platform.h        \
    $$PWD/scintilla/include/SciLexer.h        \
    $$PWD/scintilla/include/Scintilla.h       \
    $$PWD/scintilla/include/ScintillaWidget.h \
    $$PWD/scintilla/include/Sci_Position.h    


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
