#include <QtAlgorithms>

#include "HighlightTokenizer.h"
#include "HighlighterLineStates.h"

HighlightTokenizer::HighlightTokenizer() {}

HighlightTokenizer::~HighlightTokenizer() {}

QList<Token> HighlightTokenizer::getTokens() const
{
    return tokens.values();
}

int HighlightTokenizer::getState() const
{
    return state;
}

bool HighlightTokenizer::backtrackRequested() const
{
    return backtrack;
}

void HighlightTokenizer::clear()
{
    tokens.clear();
    backtrack = false;
    state     = HIGHLIGHTER_LINE_STATE_UNKNOWN;
}

void HighlightTokenizer::addToken(const Token &token)
{
    // Insert tokens sorted by position to aid with nested formatting.
    tokens.insertMulti(token.getPosition(), token);
}

void HighlightTokenizer::setState(int state)
{
    this->state = state;
}

void HighlightTokenizer::requestBacktrack()
{
    backtrack = true;
}

bool HighlightTokenizer::tokenLessThan(const Token &t1, const Token &t2)
{
    return t1.getPosition() < t2.getPosition();
}
