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

#include <memory>
#include <utility>

#include <QHash>
#include <QString>

#include "itemsource.h"

/*!
 * \brief A factory for various items sources.
 */
class ItemSourceFactory
{
public:
   /*!
    * \brief A declaration for the item source type \a ItemSourceType.
    */
   template <typename ItemSourceType> class Declaration
   {
   public:
      /*!
       * \brief The type of the item source type.
       */
      typedef ItemSourceType Type;

      /*!
       * Constructs a declaration with the MIME type \a mimeType.
       */
      inline Declaration(const QString& mimeType) : mimeType_(mimeType)
      {
      }

      /*!
       * Returns the MIME type of the item source.
       */
      inline QString mimeType() const
      {
         return mimeType_;
      }

   private:
      /*!
       * The MIME type of the item source type.
       */
      QString mimeType_;
   };

   /*!
    * Constructs an item source factory having \a ItemSourceTypes... declared.
    */
   template <typename DeclarationType, typename... DeclarationTypes>
   inline ItemSourceFactory(const DeclarationType& declaration, const DeclarationTypes&... declarations) : ItemSourceFactory(declarations...)
   {
      declare<DeclarationType::Type>(declaration.mimeType());
   }
   template <typename DeclarationType>
   inline ItemSourceFactory(const DeclarationType& declaration)
   {
      declare<DeclarationType::Type>(declaration.mimeType());
   }

   /*!
    * Declares the item source \a ItemSourceType with the factory.
    */
   template <typename ItemSourceType> inline void declare(const QString& mimeType)
   {
      types_.insert(mimeType, &ItemSourceFactory::create_<ItemSourceType>);
   }

   /*!
    * Returns an instance of the item source represented by the MIME type \a mimeType or \a
    * nullptr if no such type has been declared.
    */
   inline std::unique_ptr<ItemSource> create(const QString &mimeType, const ItemSource::Identifier& identifier) const
   {
      std::unique_ptr<ItemSource> itemSource;

      auto creator = types_.find(mimeType);
      if (creator != types_.end())
      {
         itemSource = creator.value()(identifier);
      }

      return itemSource;
   }

private:
   /*!
    * The item source type to creator mapping
    */
   QHash<QString, std::unique_ptr<ItemSource> (*)(const ItemSource::Identifier&)> types_;

   /*!
    * Creates an item source of type \a ItemSourceType.
    */
   template <typename ItemSourceType> static std::unique_ptr<ItemSource> create_(const ItemSource::Identifier& identifier)
   {
      return std::unique_ptr<ItemSource>(static_cast<ItemSource*>(new ItemSourceType(identifier)));
   }
};

#endif // ITEMSOURCEFACTORY_H
