/*!
 * \file groupitem.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license the program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef GROUPITEM_H
#define GROUPITEM_H

#include <functional>

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
    * Constructs a group item.
    */
   GroupItem();
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
    * Inserts the item \a item into the item group at row \a row. The group takes ownership. If
    * \a row is 0 the item is prepended, if \a row is size the item is appended.
    */
   void insertItem(Item* item, int position);
   /*!
    * Removes the item \a item from the item group and returns \a true if the
    * item could be removed; \a false otherwise.
    */
   bool removeItem(Item* item);
   /*!
    * Removes all items.
    */
   void removeItems();

   /*!
    * Sets the list of items to \a items. The group takes ownership.
    */
   void setItems(const QVector<Item*>& items);
   /*!
    * Returns the list of items in the item group.
    */
   const QVector<Item*>& items() const;
   /*!
    * Returns the number of items in the item group.
    */
   int itemCount() const;
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
    * Returns the index of the item \a item or a negative value if there is no such item.
    */
   int indexOf(Item* item) const;

   /*!
    * Recursively applies the function \a function to each item of type \a Type.
    */
   template <typename ItemType, typename GroupItemType = GroupItem> void apply(const std::function<void(ItemType* item)> function)
   {
      for (const auto& item : items_)
      {
         if (auto typedItem = Item::cast<ItemType>(item))
         {
            function(typedItem);
         }
         else if (auto typedGroupItem = Item::cast<GroupItemType>(item))
         {
            typedGroupItem->apply<ItemType>(function);
         }
      }
   }

   /*!
    * Returns \a true if this group item is a container for an item of the type \a type;
    * \a false otherwise. The default implementation returns \a false and should be overridden
    * in any derived class.
    */
   virtual bool isContainerOf(Item::Type /* type */) const
   {
      return false;
   }

   /*!
    * Returns \a true if the item is a or is a base of \a type.
    */
   static bool isBaseOf(Item::Type type)
   {
      return ((type == Item::Type::Group) ||
              (type == Item::Type::LinkGroup) ||
              (type == Item::Type::ImportGroup) ||
              (type == Item::Type::Source));
   }

protected:
   /*!
    * Constructs a gorup item of the type \a type.
    */
   explicit GroupItem(Item::Type type);

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

#endif // GROUPITEM_H
