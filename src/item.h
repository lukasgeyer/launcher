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
    * \brief Returns \a true if the item is a or is a base of \a type.
    */
   enum class Type
   {
      Link,
      Import,
      Group,
       LinkGroup,
       ImportGroup,
       Source,

      Count
   };

   /*!
    * \brief The access of the item.
    */
   enum class Access
   {
      ReadOnly,
      ReadWrite
   };

   /*!
    * Destructs the item.
    */
   virtual ~Item()
   {
   }

   /*!
    * Returns the type of the item.
    */
   Type type() const
   {
      return type_;
   }

   /*!
    * Returns the access of the item.
    */
   Access access() const
   {
      return access_;
   }

   /*!
    * Sets the parent to \a parent.
    */
   void setParent(Item* parent)
   {
      parent_ = parent;
   }
   /*!
    * Returns the parent of this item.
    */
   Item* parent()
   {
      return parent_;
   }
   /*!
    * Returns the parent of this item.
    */
   Item* parent() const
   {
      return parent_;
   }
   /*!
    * Returns the recursive-next parent of the item which is of type \a Type.
    */
   template <typename Type> const Type* parent() const
   {
      const Item* parent = parent_;

      for (; ((parent != nullptr) && (!Item::is<Type>(parent))); parent = parent->parent_)
      {
      }

      return static_cast<const Type*>(parent);
   }

   /*!
    * Returns \a true if the item \a item is of type \a type; false otherwise.
    */
   template <typename TargetType, typename SourceType>
   static typename std::enable_if<std::is_base_of<Item, TargetType>::value, bool>::type is(SourceType* item)
   {
      return ((item != nullptr) && (TargetType::isBaseOf(item->type_)));
   }

   /*!
    * Casts the item \a item to the derived item type \a Type and returns a pointer to that type
    * or \a nullptr if the item is not of that type.
    */
   template <typename TargetType,
             typename SourceType,
             typename ReturnType = std::conditional<std::is_const<SourceType>::value, const TargetType*, TargetType*>::type>
   static typename std::enable_if<std::is_base_of<Item, TargetType>::value, ReturnType>::type cast(SourceType* item)
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
   Item(Type type, Access access = Access::ReadWrite) : type_(type), access_(access)
   {
   }

private:
   /*!
    * Returns \a true if the item is a or is a base of \a type.
    */
   Type type_;

   /*!
    * The access of the item.
    */
   Access access_;

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
