#!/bin/bash
cd vtextedit
git apply ../vtextedit.diff
cd src/libs/syntax-highlighting
git apply ../../../../syntax-highlighting.diff
