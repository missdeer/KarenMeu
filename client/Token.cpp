#include <QString>

#include "Token.h"

int Token::getType() const
{
    return type;
}

void Token::setType(int type)
{
    this->type = type;
}

int Token::getPosition() const
{
    return position;
}

void Token::setPosition(int position)
{
    this->position = position;
}

int Token::getLength() const
{
    return length;
}

void Token::setLength(int length)
{
    this->length = length;
}

int Token::getOpeningMarkupLength() const
{
    return openingMarkupLength;
}

void Token::setOpeningMarkupLength(int length)
{
    this->openingMarkupLength = length;
}

int Token::getClosingMarkupLength() const
{
    return closingMarkupLength;
}

void Token::setClosingMarkupLength(int length)
{
    this->closingMarkupLength = length;
}
