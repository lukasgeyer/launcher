/*!
 * \file itemgroup.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include "itemgroup.h"


void ItemGroup::appendItem(const Item& item)
{
   items_.append(item);
}

const Items& ItemGroup::items() const
{
   return items_;
}

void ItemGroup::appendTag(const Tag& tag)
{
   tags_.append(tag);
}

const Tags& ItemGroup::tags() const
{
   return tags_;
}

void ItemGroup::setBrush(const QBrush& brush)
{
   brush_ = brush;
}

const QBrush&ItemGroup::brush() const
{
   return brush_;
}
