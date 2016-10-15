/*!
 * \file itemsourcefactory.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMSOURCEFACTORY_H
#define ITEMSOURCEFACTORY_H

#include <type_traits>

#include <QHash>
#include <QString>
#include <QStringList>

#include "itemsource.h"

/*!
 * \brief A factory for item sources.
 */
class ItemSourceFactory
{
public:
   /*!
    * Declares the item source \a ItemSourceType with the MIME type \a mimeType.
    */
   template <typename ItemSourceType> inline void declare(const QString& mimeType)
   {
      static_assert(std::is_base_of<ItemSource, ItemSourceType>::value, "ItemSourceType must be an ItemSource");

      types_.insert(mimeType, &ItemSourceFactory::create_<ItemSourceType>);
   }

   /*!
    * Returns the list of MIME types registered in this factory.
    */
   inline QList<QString> mimeTypes() const
   {
      return types_.keys();
   }

   /*!
    * Returns an instance of the item source represented by the MIME type \a mimeType or \a
    * nullptr if no such type has been declared.
    */
   inline ItemSource* create(const QString &mimeType) const
   {
      ItemSource* itemSource;

      auto type = types_.find(mimeType);
      if (type != types_.end())
      {
         itemSource = type.value()();
      }

      return itemSource;
   }

private:
   /*!
    * The item source type to creator mapping
    */
   QHash<QString, ItemSource* (*)()> types_;

   /*!
    * Creates an item source of type \a ItemSourceType.
    */
   template <typename ItemSourceType> static ItemSource* create_()
   {
      return static_cast<ItemSource*>(new ItemSourceType);
   }
};

#endif // ITEMSOURCEFACTORY_H
