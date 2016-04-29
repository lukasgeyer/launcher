/*!
 * \file indication.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "indication.h"


Indication::Indication(const QString& text) : text_(text)
{
}

Indication::~Indication()
{
}

void Indication::setText(const QString& text)
{
   text_ = text;
}

QString Indication::text() const
{
   return text_;
}
