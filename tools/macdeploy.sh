#!/bin/bash
otool -L client/KarenMeu.app/Contents/MacOS/KarenMeu
$QTDIR/bin/macdeployqt client/KarenMeu.app
cp -R 3rdparty/scintilla/bin/ScintillaEdit.framework client/KarenMeu.app/Contents/Frameworks/
cp -R 3rdparty/vtextedit/src/libs/syntax-highlighting/VSyntaxHighlighting.framework client/KarenMeu.app/Contents/Frameworks/
cp -R 3rdparty/vtextedit/src/editor/VTextEdit.framework client/KarenMeu.app/Contents/Frameworks/
install_name_tool -change @executable_path/../Frameworks/ScintillaEdit.framework/Versions/4/ScintillaEdit @rpath/ScintillaEdit.framework/Versions/4/ScintillaEdit client/KarenMeu.app/Contents/MacOS/KarenMeu
install_name_tool -change VSyntaxHighlighting.framework/Versions/1/VSyntaxHighlighting @rpath/VSyntaxHighlighting.framework/Versions/1/VSyntaxHighlighting client/KarenMeu.app/Contents/MacOS/KarenMeu 
install_name_tool -change VTextEdit.framework/Versions/1/VTextEdit @rpath/VTextEdit.framework/Versions/1/VTextEdit client/KarenMeu.app/Contents/MacOS/KarenMeu
otool -L client/KarenMeu.app/Contents/MacOS/KarenMeu
