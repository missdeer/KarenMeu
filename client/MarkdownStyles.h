#ifndef MARKDOWNSTYLES_H
#define MARKDOWNSTYLES_H

typedef enum
{
    BlockquoteStyleFirst,
    BlockquoteStylePlain = BlockquoteStyleFirst,
    BlockquoteStyleItalic,
    BlockquoteStyleFancy,
    BlockquoteStyleLast = BlockquoteStyleFancy
} BlockquoteStyle;

#endif // MARKDOWNSTYLES_H
