/*!
 * \file itemsource.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMSOURCE_H
#define ITEMSOURCE_H

#include <QPoint>
#include <QString>

#include "imports.h"
#include "itemgroups.h"

class QIODevice;

/*!
 * \brief An abstract item source.
 */
class ItemSource
{
public:
   /*!
    * \brief An opaque identifier.
    */
   typedef QString Identifier;

   /*!
    * Constructs the item source with an opaque identifier of \a identifier.
    */
   explicit ItemSource(const Identifier& identifier);
   /*!
    * Destructs the item source.
    */
   virtual ~ItemSource();

   /*!
    * Sets the opaque identifier for this source to \a identifier.
    */
   void setIdentifier(const QString& identifier);
   /*!
    * Returns the opaque identifier for this item source.
    */
   QString identifier() const;

   /*!
    * Read the item source from the device \a device and returns \a true if the source could be
    * successfully read; \a false otherwise. The device must be already opened for reading. The
    * item group is not reset before reading, use \a reset().
    */
   virtual bool read(QIODevice *device) = 0;
   /*!
    * Writes the item source to the device \a device and returns \a true if the source could be
    * successfully written; \a false otherwise. The device must be already opened for writing.
    */
   virtual bool write(QIODevice *device) const = 0;

   /*!
    * Resets the item source, clearing all information.
    */
   virtual void reset() = 0;

   /*!
    * Returns the list of item groups represented by this item source.
    */
   virtual const ItemGroups& itemGroups() const = 0;
   /*!
    * Returns the list of imports represented by this item source.
    */
   virtual const Imports& imports() const = 0;

   /*!
    * Returns the string representation of the last error that occurred. The behaviour is undefined
    * if no error has occurred yet.
    */
   virtual QString lastError() const = 0;
   /*!
    * Returns the position within the item source of the last error that occurred. The behaviour is
    * undefined if no error has occurred yet.
    */
   virtual QPoint lastErrorPosition() const = 0;

private:
   /*!
    * The opaque identifier of the source.
    */
   Identifier identifier_;
};

#endif // ITEMSOURCE_H
