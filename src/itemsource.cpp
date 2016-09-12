/*!
 * \file itemsource.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "itemsource.h"

ItemSource::ItemSource(const Identifier& identifier) : identifier_(identifier)
{
}

ItemSource::~ItemSource()
{
}

void ItemSource::setIdentifier(const Identifier& identifier)
{
   identifier_ = identifier;
}

QString ItemSource::identifier() const
{
   return identifier_;
}
