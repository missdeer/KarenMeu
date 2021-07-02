#!/bin/bash
if [ ! -d client/KarenMeu.app/Contents/Frameworks ]; then mkdir client/KarenMeu.app/Contents/Frameworks; fi
cp -R 3rdparty/scintilla/bin/ScintillaEdit.framework client/KarenMeu.app/Contents/Frameworks/
cp -R 3rdparty/vtextedit/src/editor/VTextEdit.framework client/KarenMeu.app/Contents/Frameworks/
cp -R 3rdparty/vtextedit/src/libs/syntax-highlighting/VSyntaxHighlighting.framework client/KarenMeu.app/Contents/Frameworks/
