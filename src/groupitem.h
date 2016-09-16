/*!
 * \file itemgroup.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license the program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMGROUP_H
#define ITEMGROUP_H

#include <QString>
#include <QVector>

#include "item.h"

/*!
 * \brief A group of items.
 */
class GroupItem : public Item
{
public:
   /*!
    * Destructs the item group, deleting all owned item groups and items.
    */
   virtual ~GroupItem();

   /*!
    * Sets the name to \a name.
    */
   void setName(const QString& name);
   /*!
    * Returns the name.
    */
   QString name() const;

   /*!
    * Appends the item \a item to the item group. The group takes ownership.
    */
   void appendItem(Item* item);
   /*!
    * Sets the list of items to \a items. The group takes ownership.
    */
   void setItems(const QVector<Item*>& items);
   /*!
    * Returns the list of items in the item group.
    */
   const QVector<Item*>& items() const;
   /*!
    * Returns a pointer to the item with the index \a index or \a nullptr if there is no
    * such item.
    */
   Item* item(int index);
   /*!
    * Returns a pointer to the item with the index \a index or \a nullptr if there is no
    * such item.
    */
   const Item* item(int index) const;

   /*!
    * The type of the item.
    */
   static inline bool isItemType(Item::ItemType itemType)
   {
      return ((itemType == Item::ItemType::Group) ||
              (itemType == Item::ItemType::LinkGroup) ||
              (itemType == Item::ItemType::ImportGroup) ||
              (itemType == Item::ItemType::Source));
   }

protected:
   /*!
    * Constructs an item of the type \a type.
    */
   explicit GroupItem(ItemType type);

private:
   /*!
    * The name of the item group.
    */
   QString name_;

   /*!
    * The list of items in the item group.
    */
   QVector<Item*> items_;
};

#endif // ITEMGROUP_H
