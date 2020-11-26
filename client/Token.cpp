#include <QString>

#include "Token.h"

Token::Token()
{
    type = -1;
    position = 0;
    length = 0;
    openingMarkupLength = 0;
    closingMarkupLength = 0;
}

Token::~Token()
{

}

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
