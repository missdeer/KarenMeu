TEMPLATE = subdirs

SUBDIRS = \
    vtextedit \
    scintilla \
    client \
    registerProtocolHandler

vtextedit.subdir = 3rdparty/vtextedit/src
scintilla.subdir  = 3rdparty/scintilla/qt/ScintillaEdit
app.subdir  = client

app.depends = vtextedit scintilla
