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
    * Returns the index of the item \a item or a negative value if there is no such item.
    */
   int indexOf(Item* item) const;

   /*!
    * Returns \a true if this group item is a container for an item of the type \a type;
    * \a false otherwise. The default implementation returns \a false and should be overridden
    * in any derived class.
    */
   virtual inline bool isContainerOf(Item::Type /* type */) const
   {
      return false;
   }

   /*!
    * The type of the item.
    */
   static inline bool isType(Item::Type type)
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
