#ifndef MARKDOWN_STATES_H
#define MARKDOWN_STATES_H

#include "HighlighterLineStates.h"

/*
 * Line states for use in tracking the line state of each block (line) in
 * a Markdown document.  See the userState() method in QTextBlock for details.
 */
enum MarkdownState
{
    MarkdownStateUnknown = HIGHLIGHTER_LINE_STATE_UNKNOWN,
    MarkdownStateParagraphBreak,
    MarkdownStateListLineBreak,
    MarkdownStateParagraph,
    MarkdownStateAtxHeading1,
    MarkdownStateAtxHeading2,
    MarkdownStateAtxHeading3,
    MarkdownStateAtxHeading4,
    MarkdownStateAtxHeading5,
    MarkdownStateAtxHeading6,
    MarkdownStateBlockquote,
    MarkdownStateCodeBlock,
    MarkdownStateInGithubCodeFence,
    MarkdownStateInPandocCodeFence,
    MarkdownStateCodeFenceEnd,
    MarkdownStateComment,
    MarkdownStateHorizontalRule,
    MarkdownStateNumberedList,
    MarkdownStateBulletPointList,
    MarkdownStateSetextHeading1Line1,
    MarkdownStateSetextHeading1Line2,
    MarkdownStateSetextHeading2Line1,
    MarkdownStateSetextHeading2Line2,
    MarkdownStatePipeTableHeader,
    MarkdownStatePipeTableDivider,
    MarkdownStatePipeTableRow
};

#endif // MARKDOWN_STATES_H
