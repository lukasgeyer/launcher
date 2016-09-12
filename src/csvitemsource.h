/*!
 * \file csvitemsource.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef CSVITEMSOURCE_H
#define CSVITEMSOURCE_H

#include "itemsource.h"

class QXmlStreamReader;

/*!
 * \brief An item source represented as CSV file.
 */
class CsvItemSource : public ItemSource
{
public:
   /*!
    * Constructs an empty CSV item source with the identifier \a identifier.
    */
   explicit CsvItemSource(const QString& identifier);

   /*!
    * \reimp
    */
   bool read(QIODevice* device) override;
   /*!
    * \reimp
    */
   bool write(QIODevice* device) const override;

   /*!
    * \reimp
    */
   void reset() override;

   /*!
    * \reimp
    */
   const ItemGroups& itemGroups() const override;

   /*!
    * \reimp
    */
   const Imports& imports() const override;

   /*!
    * \reimp
    */
   QString lastError() const override;
   /*!
    * \reimp
    */
   QPoint lastErrorPosition() const override;
};

#endif // CSVITEMSOURCE_H
