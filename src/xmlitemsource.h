/*!
 * \file xmlitemsource.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef XMLITEMSOURCE_H
#define XMLITEMSOURCE_H

#include <memory>

#include "itemsource.h"

class QXmlStreamReader;

/*!
 * \brief An item source represented as XML file.
 */
class XmlItemSource : public ItemSource
{
public:
   /*!
    * \reimp
    */
   bool read(QIODevice* device, const QString& identifier) override;
   /*!
    * \reimp
    */
   bool write(QIODevice* device) const override;

   /*!
    * \reimp
    */
   QString errorString() const override;
   /*!
    * \reimp
    */
   QPoint errorPosition() const override;

private:
   /*!
    * The XML document error.
    */
   mutable QString errorString_;
   /*!
    * The XML document error position.
    */
   mutable QPoint errorPosition_;

   /*!
    * Reads an item from the XML stream \a reader and adds it to the list of items.
    */
   void readItem_(QXmlStreamReader* reader, GroupItem* parent);
   /*!
    * Reads a group from the XML stream \a reader and add it to the list of items.
    */
   void readGroup_(QXmlStreamReader* reader, GroupItem* parent);
   /*!
    * Reads an imported source from the XML stream \a stream.
    */
   void readImport_(QXmlStreamReader* reader);
};

#endif // XMLITEMSOURCE_H
