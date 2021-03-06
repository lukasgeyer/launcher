/*!
 * \file importitem.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef IMPORTITEM_H
#define IMPORTITEM_H

#include <QDebug>
#include <QString>

#include "item.h"

/*!
 * \brief An import definition.
 */
class ImportItem : public Item
{
public:
   /*!
    * Constructs an import definition for the file \a file and the MIME type \a type.
    */
   explicit ImportItem(const QString& file = QString(), const QString& mimeType = QString());

   /*!
    * Sets the name of the file to be imported.
    */
   void setFile(const QString& file);
   /*!
    * Returns the name of the file to be imported.
    */
   QString file() const;

   /*!
    * Sets the type of the file to be imported.
    */
   void setMimeType(const QString& mimeType);
   /*!
    * Returns the type of the file to be imported.
    */
   QString mimeType() const;

   /*!
    * Returns \a true if the item is a or is a base of \a type.
    */
   static bool isBaseOf(Item::Type type)
   {
      return (type == Item::Type::Import);
   }

private:
   /*!
    * The name of the file to be imported.
    */
   QString file_;
   /*!
    * The MIME type of the file to be imported.
    */
   QString mimeType_;

   /*!
    * Inserts the import \a import into the stream \a stream and returns the stream.
    */
   friend QDebug operator<<(QDebug stream, const ImportItem& import);
};

#endif // IMPORTITEM_H
