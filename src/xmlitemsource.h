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

#include "itemsource.h"

class QXmlStreamReader;

/*!
 * \brief An item source represented as XML file.
 */
class XmlItemSource : public ItemSource
{
public:
   /*!
    * Constructs an empty XML item source with the identifier \a identifier.
    */
   explicit XmlItemSource(const QString& identifier);

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

private:
   /*!
    * The item groups in the source. Any item that is not in a group is represented in an
    * surrogate group, which is the first group in this list.
    */
   ItemGroups itemGroups_;

   /*!
    * The imports in the source.
    */
   Imports imports_;

   /*!
    * The XML document error.
    */
   mutable QString documentError_;
   /*!
    * The XML document error position.
    */
   mutable QPoint documentErrorPosition_;

   /*!
    * Reads an item from the XML stream \a reader and adds it to the list of items.
    */
   void readItem_(QXmlStreamReader* reader, ItemGroup* itemGroup);
   /*!
    * Reads a group from the XML stream \a reader and add it to the list of items.
    */
   void readGroup_(QXmlStreamReader* reader);
   /*!
    * Reads an imported source from the XML stream \a stream.
    */
   void readImport_(QXmlStreamReader* reader);
};

#endif // XMLITEMSOURCE_H
