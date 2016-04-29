/*!
 * \file sourceposition.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "sourceposition.h"


SourcePosition::SourcePosition(int lineNumber, int columnNumber, int size) : lineNumber_(lineNumber), columnNumber_(columnNumber), size_(size)
{
}

void SourcePosition::setLineNumber(int lineNumber)
{
   lineNumber_ = lineNumber;
}

int SourcePosition::lineNumber() const
{
   return lineNumber_;
}

void SourcePosition::setColumnNumber(int columnNumber)
{
   columnNumber_ = columnNumber;
}

int SourcePosition::columnNumber() const
{
   return columnNumber_;
}

void SourcePosition::setSize(int size)
{
   size_ = size;
}

int SourcePosition::size() const
{
   return size_;
}
