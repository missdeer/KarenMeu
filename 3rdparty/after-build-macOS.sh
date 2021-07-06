#!/bin/bash
if [ ! -d client/KarenMeu.app/Contents/Frameworks ]; then mkdir client/KarenMeu.app/Contents/Frameworks; fi
if [ -d client/KarenMeu.app/Contents/Frameworks/ScintillaEdit.framework ]; then rm -rf client/KarenMeu.app/Contents/Frameworks/ScintillaEdit.framework; fi
cp -R 3rdparty/scintilla/bin/ScintillaEdit.framework client/KarenMeu.app/Contents/Frameworks/
if [ -d client/KarenMeu.app/Contents/Frameworks/VTextEdit.framework ]; then rm -rf client/KarenMeu.app/Contents/Frameworks/VTextEdit.framework; fi
cp -R 3rdparty/vtextedit/src/editor/VTextEdit.framework client/KarenMeu.app/Contents/Frameworks/
if [ -d client/KarenMeu.app/Contents/Frameworks/VSyntaxHighlighting.framework ]; then rm -rf client/KarenMeu.app/Contents/Frameworks/VSyntaxHighlighting.framework; fi
cp -R 3rdparty/vtextedit/src/libs/syntax-highlighting/VSyntaxHighlighting.framework client/KarenMeu.app/Contents/Frameworks/
