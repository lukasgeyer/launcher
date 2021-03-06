/*!
 * \file itemsourceposition.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "itemsourceposition.h"


ItemSourcePosition::ItemSourcePosition(int lineNumber, int columnNumber, int size) : lineNumber_(lineNumber), columnNumber_(columnNumber), size_(size)
{
}

void ItemSourcePosition::setLineNumber(int lineNumber)
{
   lineNumber_ = lineNumber;
}

int ItemSourcePosition::lineNumber() const
{
   return lineNumber_;
}

void ItemSourcePosition::setColumnNumber(int columnNumber)
{
   columnNumber_ = columnNumber;
}

int ItemSourcePosition::columnNumber() const
{
   return columnNumber_;
}

void ItemSourcePosition::setSize(int size)
{
   size_ = size;
}

int ItemSourcePosition::size() const
{
   return size_;
}
