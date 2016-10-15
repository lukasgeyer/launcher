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

#include <type_traits>

#include <QHash>
#include <QPair>
#include <QString>
#include <QStringList>

#include "item.h"
#include "itemeditor.h"

/*!
 * \brief A factory for item editors.
 */
class ItemEditorFactory
{
public:
   /*!
    * Declares the item editor \a ItemEditorType with the item type \a itemType and the item type
    * name \a itemTypeName.
    */
   template <typename ItemEditorType> inline void declare(Item::ItemType itemType, const QString& itemTypeName)
   {
      static_assert(std::is_base_of<ItemEditor, ItemEditorType>::value, "ItemEditorType must be an ItemEditor");

      types_.insert(itemType, qMakePair(itemTypeName, &ItemEditorFactory::create_<ItemEditorType>));
   }

   /*!
    * Returns the list item types registered in this factory.
    */
   inline QList<Item::ItemType> itemTypes() const
   {
      return types_.keys();
   }

   /*!
    * Returns the item type name for the item type \a itemType or an empty string if there
    * is no such name.
    */
   inline QString itemTypeName(Item::ItemType itemType)
   {
      QString itemTypeName;

      auto type = types_.find(itemType);
      if (type != types_.end())
      {
         itemTypeName = type.value().first;
      }

      return itemTypeName;
   }

   /*!
    * Returns an instance of the item source represented by the item type \a itemType or \a
    * nullptr if no such type has been declared.
    */
   inline ItemEditor* create(Item::ItemType itemType) const
   {
      ItemEditor* itemEditor;

      auto type = types_.find(itemType);
      if (type != types_.end())
      {
         itemEditor = type.value().second();
      }

      return itemEditor;
   }

private:
   /*!
    * The item source type to creator mapping
    */
   QHash<Item::ItemType, QPair<QString, ItemEditor* (*)()>> types_;

   /*!
    * Creates an item source of type \a ItemEditorType.
    */
   template <typename ItemEditorType> static ItemEditor* create_()
   {
      return static_cast<ItemEditor*>(new ItemEditorType);
   }
};

#endif // ITEMEDITORFACTORY_H
