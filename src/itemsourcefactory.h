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

#include <QList>
#include <QString>

#include "factory.h"
#include "itemsource.h"

/*!
 * \brief A factory for item sources.
 */
class ItemSourceFactory : public Factory<QString, ItemSource>
{
public:
   /*!
    * Returns the list of MIME types registered in this factory.
    */
   QList<QString> mimeTypes() const
   {
      return ids();
   }
};

#endif // ITEMSOURCEFACTORY_H
