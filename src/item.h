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

#include <QHash>

/*!
 * \brief An item.
 */
class Item
{
public:
   /*!
    * \brief The type of the item.
    */
   enum class Type
   {
      Invalid,
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
    * Returns the type of the item.
    */
   inline Type type() const
   {
      return type_;
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

   /*!
    * Returns \a true if the item \a item is of type \a type; false otherwise.
    */
   template <typename TargetType, typename SourceType>
   static inline typename std::enable_if<std::is_base_of<Item, TargetType>::value, bool>::type is(SourceType* item)
   {
      return ((item != nullptr) && (TargetType::isType(item->type_)));
   }

   /*!
    * Casts the item \a item to the derived item type \a Type and returns a pointer to that type
    * or \a nullptr if the item is not of that type.
    */
   template <typename TargetType,
             typename SourceType,
             typename ReturnType = std::conditional<std::is_const<SourceType>::value, const TargetType*, TargetType*>::type>
   static inline typename std::enable_if<std::is_base_of<Item, TargetType>::value, ReturnType>::type cast(SourceType* item)
   {
      ReturnType returnItem = nullptr;

      if (Item::is<TargetType>(item))
      {
         returnItem = static_cast<decltype(returnItem)>(item);
      }

      return returnItem;
   }

protected:
   /*!
    * Constructs an item of the type \a type.
    */
   inline Item(Type type) : type_(type)
   {
   }

private:
   /*!
    * The type of the item.
    */
   Type type_;

   /*!
    * A pointer to the parent of this item.
    */
   Item* parent_ = nullptr;
};

/*!
 * Returns the hash for the item type \a type.
 */
inline uint qHash(Item::Type type, uint seed = 0)
{
   return qHash(static_cast<int>(type), seed);
}

#endif // ITEM_H
