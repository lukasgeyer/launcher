/*!
 * \file item.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license the program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEM_H
#define ITEM_H

#include <type_traits>

/*!
 * \brief An item.
 */
class Item
{
public:
   /*!
    * \brief The type of the item.
    */
   enum ItemType
   {
      Link,
      Import,
      Group,
       LinkGroup,
       ImportGroup,
       Source
   };

   /*!
    * Destructs the item.
    */
   inline virtual ~Item()
   {
   }

   /*!
    * Casts the item to the derived item type \a TargetType and returns a pointer to
    * that type or \a nullptr if the item is not of that type.
    */
   template <typename TargetType> inline typename std::enable_if<std::is_base_of<Item, TargetType>::value, TargetType*>::type cast()
   {
      return (TargetType::isItemType(type_) ? (static_cast<TargetType*>(this)) : (nullptr));
   }

   /*!
    * Sets the parent to \a parent.
    */
   inline void setParent(Item* parent)
   {
      parent_ = parent;
   }
   /*!
    * Returns the parent of this item.
    */
   inline Item* parent()
   {
      return parent_;
   }

protected:
   /*!
    * Constructs an item of the type \a type.
    */
   inline Item(ItemType type) : type_(type)
   {
   }

private:
   /*!
    * The type of the item.
    */
   ItemType type_;

   /*!
    * A pointer to the parent of this item.
    */
   Item* parent_ = nullptr;
};

#endif // ITEM_H
