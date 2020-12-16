#!/bin/bash
otool -L client/KarenMeu.app/Contents/MacOS/KarenMeu
$QTDIR/bin/macdeployqt client/KarenMeu.app
if [ ! -d client/KarenMeu.app/Contents/Frameworks ]; then
mkdir -p client/KarenMeu.app/Contents/Frameworks
fi
if [ ! -d client/KarenMeu.app/Contents/Frameworks/VSyntaxHighlighting.framework ]; then
cp -R 3rdparty/vtextedit/src/libs/syntax-highlighting/VSyntaxHighlighting.framework client/KarenMeu.app/Contents/Frameworks/
fi
if [ ! -d client/KarenMeu.app/Contents/Frameworks/VTextEdit.framework ]; then
cp -R 3rdparty/vtextedit/src/editor/VTextEdit.framework client/KarenMeu.app/Contents/Frameworks/
fi
install_name_tool -change VSyntaxHighlighting.framework/Versions/1/VSyntaxHighlighting @rpath/VSyntaxHighlighting.framework/Versions/1/VSyntaxHighlighting client/KarenMeu.app/Contents/MacOS/KarenMeu 
install_name_tool -change VTextEdit.framework/Versions/1/VTextEdit @rpath/VTextEdit.framework/Versions/1/VTextEdit client/KarenMeu.app/Contents/MacOS/KarenMeu
otool -L client/KarenMeu.app/Contents/MacOS/KarenMeu
