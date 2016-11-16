/*!
 * \file itemeditorfactory.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMEDITORFACTORY_H
#define ITEMEDITORFACTORY_H

#include <QPair>
#include <QList>
#include <QString>

#include "factory.h"
#include "item.h"
#include "itemeditor.h"

/*!
 * \brief A factory for item editors.
 */
class ItemEditorFactory : public Factory<Item::Type, ItemEditor>
{
public:
   /*!
    * Returns the item types registered in this factory.
    */
   inline QList<Item::Type> types() const
   {
      return ids();
   }
};

#endif // ITEMEDITORFACTORY_H