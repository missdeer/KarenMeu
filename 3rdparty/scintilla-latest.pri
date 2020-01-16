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
    $$PWD/scintilla/lexers/LexA68k.cxx          \ 
    $$PWD/scintilla/lexers/LexAbaqus.cxx        \ 
    $$PWD/scintilla/lexers/LexAda.cxx           \ 
    $$PWD/scintilla/lexers/LexAPDL.cxx          \ 
    $$PWD/scintilla/lexers/LexAsm.cxx           \ 
    $$PWD/scintilla/lexers/LexAsn1.cxx          \ 
    $$PWD/scintilla/lexers/LexASY.cxx           \ 
    $$PWD/scintilla/lexers/LexAU3.cxx           \ 
    $$PWD/scintilla/lexers/LexAVE.cxx           \ 
    $$PWD/scintilla/lexers/LexAVS.cxx           \ 
    $$PWD/scintilla/lexers/LexBaan.cxx          \ 
    $$PWD/scintilla/lexers/LexBash.cxx          \ 
    $$PWD/scintilla/lexers/LexBasic.cxx         \ 
    $$PWD/scintilla/lexers/LexBatch.cxx         \ 
    $$PWD/scintilla/lexers/LexBibTeX.cxx        \ 
    $$PWD/scintilla/lexers/LexBullant.cxx       \ 
    $$PWD/scintilla/lexers/LexCaml.cxx          \ 
    $$PWD/scintilla/lexers/LexCIL.cxx           \ 
    $$PWD/scintilla/lexers/LexCLW.cxx           \ 
    $$PWD/scintilla/lexers/LexCmake.cxx         \ 
    $$PWD/scintilla/lexers/LexCOBOL.cxx         \ 
    $$PWD/scintilla/lexers/LexCoffeeScript.cxx  \ 
    $$PWD/scintilla/lexers/LexConf.cxx          \ 
    $$PWD/scintilla/lexers/LexCPP.cxx           \ 
    $$PWD/scintilla/lexers/LexCrontab.cxx       \ 
    $$PWD/scintilla/lexers/LexCsound.cxx        \ 
    $$PWD/scintilla/lexers/LexCSS.cxx           \ 
    $$PWD/scintilla/lexers/LexD.cxx             \ 
    $$PWD/scintilla/lexers/LexDiff.cxx          \ 
    $$PWD/scintilla/lexers/LexDMAP.cxx          \ 
    $$PWD/scintilla/lexers/LexDMIS.cxx          \ 
    $$PWD/scintilla/lexers/LexECL.cxx           \ 
    $$PWD/scintilla/lexers/LexEDIFACT.cxx       \ 
    $$PWD/scintilla/lexers/LexEiffel.cxx        \ 
    $$PWD/scintilla/lexers/LexErlang.cxx        \ 
    $$PWD/scintilla/lexers/LexErrorList.cxx     \ 
    $$PWD/scintilla/lexers/LexEScript.cxx       \ 
    $$PWD/scintilla/lexers/LexFlagship.cxx      \ 
    $$PWD/scintilla/lexers/LexForth.cxx         \ 
    $$PWD/scintilla/lexers/LexFortran.cxx       \ 
    $$PWD/scintilla/lexers/LexGAP.cxx           \ 
    $$PWD/scintilla/lexers/LexGui4Cli.cxx       \ 
    $$PWD/scintilla/lexers/LexHaskell.cxx       \ 
    $$PWD/scintilla/lexers/LexHex.cxx           \ 
    $$PWD/scintilla/lexers/LexHTML.cxx          \ 
    $$PWD/scintilla/lexers/LexIndent.cxx        \ 
    $$PWD/scintilla/lexers/LexInno.cxx          \ 
    $$PWD/scintilla/lexers/LexJSON.cxx          \ 
    $$PWD/scintilla/lexers/LexKix.cxx           \ 
    $$PWD/scintilla/lexers/LexKVIrc.cxx         \ 
    $$PWD/scintilla/lexers/LexLaTeX.cxx         \ 
    $$PWD/scintilla/lexers/LexLisp.cxx          \ 
    $$PWD/scintilla/lexers/LexLout.cxx          \ 
    $$PWD/scintilla/lexers/LexLua.cxx           \ 
    $$PWD/scintilla/lexers/LexMagik.cxx         \ 
    $$PWD/scintilla/lexers/LexMake.cxx          \ 
    $$PWD/scintilla/lexers/LexMarkdown.cxx      \ 
    $$PWD/scintilla/lexers/LexMatlab.cxx        \ 
    $$PWD/scintilla/lexers/LexMaxima.cxx        \ 
    $$PWD/scintilla/lexers/LexMetapost.cxx      \ 
    $$PWD/scintilla/lexers/LexMMIXAL.cxx        \ 
    $$PWD/scintilla/lexers/LexModula.cxx        \ 
    $$PWD/scintilla/lexers/LexMPT.cxx           \ 
    $$PWD/scintilla/lexers/LexMSSQL.cxx         \ 
    $$PWD/scintilla/lexers/LexMySQL.cxx         \ 
    $$PWD/scintilla/lexers/LexNim.cxx           \ 
    $$PWD/scintilla/lexers/LexNimrod.cxx        \ 
    $$PWD/scintilla/lexers/LexNsis.cxx          \ 
    $$PWD/scintilla/lexers/LexNull.cxx          \ 
    $$PWD/scintilla/lexers/LexOpal.cxx          \ 
    $$PWD/scintilla/lexers/LexOScript.cxx       \ 
    $$PWD/scintilla/lexers/LexPascal.cxx        \ 
    $$PWD/scintilla/lexers/LexPB.cxx            \ 
    $$PWD/scintilla/lexers/LexPerl.cxx          \ 
    $$PWD/scintilla/lexers/LexPLM.cxx           \ 
    $$PWD/scintilla/lexers/LexPO.cxx            \ 
    $$PWD/scintilla/lexers/LexPOV.cxx           \ 
    $$PWD/scintilla/lexers/LexPowerPro.cxx      \ 
    $$PWD/scintilla/lexers/LexPowerShell.cxx    \ 
    $$PWD/scintilla/lexers/LexProgress.cxx      \ 
    $$PWD/scintilla/lexers/LexProps.cxx         \ 
    $$PWD/scintilla/lexers/LexPS.cxx            \ 
    $$PWD/scintilla/lexers/LexPython.cxx        \ 
    $$PWD/scintilla/lexers/LexR.cxx             \ 
    $$PWD/scintilla/lexers/LexRebol.cxx         \ 
    $$PWD/scintilla/lexers/LexRegistry.cxx      \ 
    $$PWD/scintilla/lexers/LexRuby.cxx          \ 
    $$PWD/scintilla/lexers/LexRust.cxx          \ 
    $$PWD/scintilla/lexers/LexSAS.cxx           \ 
    $$PWD/scintilla/lexers/LexScriptol.cxx      \ 
    $$PWD/scintilla/lexers/LexSmalltalk.cxx     \ 
    $$PWD/scintilla/lexers/LexSML.cxx           \ 
    $$PWD/scintilla/lexers/LexSorcus.cxx        \ 
    $$PWD/scintilla/lexers/LexSpecman.cxx       \ 
    $$PWD/scintilla/lexers/LexSpice.cxx         \ 
    $$PWD/scintilla/lexers/LexSQL.cxx           \ 
    $$PWD/scintilla/lexers/LexStata.cxx         \ 
    $$PWD/scintilla/lexers/LexSTTXT.cxx         \ 
    $$PWD/scintilla/lexers/LexTACL.cxx          \ 
    $$PWD/scintilla/lexers/LexTADS3.cxx         \ 
    $$PWD/scintilla/lexers/LexTAL.cxx           \ 
    $$PWD/scintilla/lexers/LexTCL.cxx           \ 
    $$PWD/scintilla/lexers/LexTCMD.cxx          \ 
    $$PWD/scintilla/lexers/LexTeX.cxx           \ 
    $$PWD/scintilla/lexers/LexTxt2tags.cxx      \ 
    $$PWD/scintilla/lexers/LexVB.cxx            \ 
    $$PWD/scintilla/lexers/LexVerilog.cxx       \ 
    $$PWD/scintilla/lexers/LexVHDL.cxx          \ 
    $$PWD/scintilla/lexers/LexVisualProlog.cxx  \ 
    $$PWD/scintilla/lexers/LexX12.cxx           \ 
    $$PWD/scintilla/lexers/LexYAML.cxx          

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
