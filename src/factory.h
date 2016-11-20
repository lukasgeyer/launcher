/*!
 * \file factory.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef FACTORY_H
#define FACTORY_H

#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

#include <QHash>
#include <QList>

/*!
 * \brief A factory for objects of type \a ObjectType identified by an ID of type \a IdType.
 */
template <typename IdType, typename ObjectType, typename... AdditionalInformationTypes> class Factory
{
public:
   /*!
    * The type of the additional information at index \a AdditionalInformationIndex.
    */
   template <std::size_t AdditionalInformationIndex> struct AdditionalInformationType
   {
      typedef typename std::tuple_element<AdditionalInformationIndex, std::tuple<AdditionalInformationTypes...>>::type type;
   };

   /*!
    * Declares the type \a DerivedObjectType with ID \a id.
    */
   template <typename DerivedObjectType> void declare(const IdType& id, AdditionalInformationTypes&&... additionalInformations)
   {
      static_assert(std::is_base_of<ObjectType, DerivedObjectType>::value, "DerivedObjectType must be an ObjectType");

      types_.insert(id, {&create_<DerivedObjectType>, std::forward<AdditionalInformationTypes>(additionalInformations)...});
   }

   /*!
    * Returns the IDs declared at this factory.
    */
   QList<IdType> ids() const
   {
      return types_.keys();
   }

   /*!
    * Returns the additional information from the additional information type at index
    * \a AdditionalInformationIndex or a default constructed value if there is no such
    * type.
    */
   template <std::size_t AdditionalInformationIndex>
   typename AdditionalInformationType<AdditionalInformationIndex>::type additionalInformation(const IdType& id) const
   {
      typename AdditionalInformationType<AdditionalInformationIndex>::type additionalInformation;

      auto type = types_.find(id);
      if (type != types_.end())
      {
         additionalInformation = std::get<1 /* create_ */ + AdditionalInformationIndex>(type.value());
      }

      return additionalInformation;
   }

   /*!
    * Returns an instance of the object type represented by the ID \a id or \a nullptr if no such
    * type has been declared.
    */
   std::unique_ptr<ObjectType> create(const IdType &id) const
   {
      std::unique_ptr<ObjectType> object;

      auto type = types_.find(id);
      if (type != types_.end())
      {
         object.reset(std::get<0>(type.value())());
      }

      return object;
   }

private:
   /*!
    * The item source type to creator mapping
    */
   QHash<IdType, std::tuple<ObjectType* (*)(), AdditionalInformationTypes...>> types_;

   /*!
    * Creates an item source of type \a ItemSourceType.
    */
   template <typename DerivedObjectType> static ObjectType* create_()
   {
      return static_cast<ObjectType*>(new DerivedObjectType);
   }
};

#endif // FACTORY_H
