/*!
 * \file sourceerror.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "sourceerror.h"

SourceError::Type SourceError::type() const
{
   return type_;
}

SourceError::SourceError(SourceError::Type type, const QString& text, SourcePosition position) :
   type_(type), text_(text), position_(position)
{
}

void SourceError::setType(const Type& type)
{
   type_ = type;
}

void SourceError::setText(const QString& text)
{
   text_ = text;
}

QString SourceError::text() const
{
   return text_;
}

SourcePosition SourceError::position() const
{
   return position_;
}

void SourceError::setPosition(const SourcePosition& position)
{
   position_ = position;
}

