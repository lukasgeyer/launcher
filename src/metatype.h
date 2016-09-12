/*!
 * \file metatype.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license the program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef METATYPE_H
#define METATYPE_H

#include <QMetaType>

#define METATYPEREGISTRATION_TYPE__(Type) #Type
#define METATYPEREGISTRATION_TYPE_(Type)  METATYPEREGISTRATION_TYPE__(Type)

#define METATYPEREGISTRATION_NAME__(Name, Id) Name##Id
#define METATYPEREGISTRATION_NAME_(Name, Id) METATYPEREGISTRATION_NAME__(Name, Id)

/*!
 * Registers the type \a type as metatype.
 */
#define REGISTER_METATYPE(Type) \
   namespace { \
   \
      MetaTypeRegistration<Type> METATYPEREGISTRATION_NAME_(MetaTypeRegistration, __COUNTER__)(METATYPEREGISTRATION_TYPE_(Type)); \
   \
   }// namespace

/*!
 * \brief A metatype registration.
 */
template <typename MetaType> class MetaTypeRegistration
{
public:
   inline MetaTypeRegistration(const char *metaTypeName)
   {
      qRegisterMetaType<MetaType>(metaTypeName);
   }
};

#endif // METATYPE_H
