/*!
 * \file sourceerrorindication.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "sourceerrorindication.h"

SourceErrorIndication::SourceErrorIndication(const QString& text, const QString& source, const SourcePosition& sourcePosition) :
   Indication(text), source_(source), sourcePosition_(sourcePosition)
{
}

void SourceErrorIndication::setSource(const QString& source)
{
   source_ = source;
}

QString SourceErrorIndication::source() const
{
   return source_;
}

void SourceErrorIndication::setSourcePosition(const SourcePosition& sourcePosition)
{
   sourcePosition_ = sourcePosition;
}

SourcePosition SourceErrorIndication::sourcePosition() const
{
   return sourcePosition_;
}
