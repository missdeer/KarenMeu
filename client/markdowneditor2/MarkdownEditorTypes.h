#ifndef MARKDOWNEDITORTYPES_H
#define MARKDOWNEDITORTYPES_H

enum FocusMode
{
    FocusModeFirst,
    FocusModeDisabled = FocusModeFirst,
    FocusModeSentence,
    FocusModeCurrentLine,
    FocusModeThreeLines,
    FocusModeParagraph,
    FocusModeTypewriter,
    FocusModeLast = FocusModeTypewriter
};

enum EditorWidth
{
    EditorWidthFirst,
    EditorWidthNarrow = EditorWidthFirst,
    EditorWidthMedium,
    EditorWidthWide,
    EditorWidthFull,
    EditorWidthLast = EditorWidthFull
};

enum InterfaceStyle
{
    InterfaceStyleFirst,
    InterfaceStyleRounded = InterfaceStyleFirst,
    InterfaceStyleSquare,
    InterfaceStyleLast = InterfaceStyleSquare,
};

#endif // MARKDOWNEDITORTYPES_H
