/*!
 * \file xmlitemsource.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QFileInfo>
#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "importitem.h"
#include "importgroupitem.h"
#include "item.h"
#include "groupitem.h"
#include "linkitem.h"
#include "linkgroupitem.h"
#include "xmlitemsource.h"

namespace {

/*!
 * The default indentation when writing the document.
 */
static const int DEFAULT_INDENT_ = 3;

} // namespace

bool XmlItemSource::read(QIODevice* device, const QString& identifier)
{
   bool result = false;

   ItemSource::setIdentifier(identifier);

   if (device != nullptr)
   {
      QXmlStreamReader deviceReader(device);
      if ((deviceReader.readNextStartElement()) && (deviceReader.name() == "items"))
      {
         while (deviceReader.readNextStartElement())
         {
            if (deviceReader.name() == "item")
            {
               readItem_(&deviceReader, this);
            }
            else if (deviceReader.name() == "group")
            {
               readGroup_(&deviceReader, this);
            }
            else if (deviceReader.name() == "import")
            {
               readImport_(&deviceReader);
            }
            else
            {
               deviceReader.skipCurrentElement();
            }
         }
      }
      else
      {
         deviceReader.raiseError(QObject::tr("No root element found"));
      }

      result = (!deviceReader.hasError());
      if (!result)
      {
         errorString_ = deviceReader.errorString();
         errorPosition_ = {static_cast<int>(deviceReader.lineNumber()), static_cast<int>(deviceReader.columnNumber())};
      }
   }

   return result;
}

bool XmlItemSource::write(QIODevice* /* device */) const
{
   return false;
}

QString XmlItemSource::errorString() const
{
   return errorString_;
}

QPoint XmlItemSource::errorPosition() const
{
   return errorPosition_;
}

void XmlItemSource::readItem_(QXmlStreamReader* reader, GroupItem* parent)
{
   Q_ASSERT(reader != nullptr);
   Q_ASSERT(parent != nullptr);

   auto item = new LinkItem;

   while (reader->readNextStartElement())
   {
      if (reader->name() == "name")
      {
         item->setName(reader->readElementText().trimmed());
      }
      else if (reader->name() == "url")
      {
         ItemSourcePosition linkPosition;
         linkPosition.setLineNumber(reader->lineNumber());
         linkPosition.setColumnNumber(reader->columnNumber());

         const auto& elementText = reader->readElementText();

         linkPosition.setSize(elementText.size());

         item->setLink(elementText);
         item->setLinkPosition(linkPosition);
      }
      else if (reader->name() == "color")
      {
         item->setBrush(QBrush(QColor(reader->readElementText().trimmed())));
      }
      else if (reader->name() == "tag")
      {
         item->appendTag(reader->readElementText().trimmed());
      }
      else
      {
         reader->skipCurrentElement();
      }
   }

   parent->insertItem(item, parent->itemCount());
}

void XmlItemSource::readGroup_(QXmlStreamReader* reader, GroupItem* parent)
{
   Q_ASSERT(reader != nullptr);

   auto itemGroup = new LinkGroupItem;

   while (reader->readNextStartElement())
   {
      if (reader->name() == "item")
      {
         readItem_(reader, itemGroup);
      }
      else if (reader->name() == "name")
      {
         itemGroup->setName(reader->readElementText().trimmed());
      }
      else if (reader->name() == "color")
      {
         itemGroup->setBrush(QBrush(QColor(reader->readElementText().trimmed())));
      }
      else if (reader->name() == "tag")
      {
         itemGroup->appendTag(reader->readElementText().trimmed());
      }
      else if (reader->name() == "group")
      {
         readGroup_(reader, parent);
      }
      else
      {
         reader->skipCurrentElement();
      }
   }

   parent->insertItem(itemGroup, parent->itemCount());
}

void XmlItemSource::readImport_(QXmlStreamReader* reader)
{
   Q_ASSERT(reader != nullptr);

   auto itemGroup = new ImportGroupItem;

   while (reader->readNextStartElement())
   {
      if (reader->name() == "file")
      {
         itemGroup->insertItem(new ImportItem(reader->readElementText().trimmed(),
                                              reader->attributes().hasAttribute(QStringLiteral("contentType")) ?
                                              reader->attributes().value(QStringLiteral("contentType")).toString().trimmed() :
                                              QStringLiteral("text/xml")), itemGroup->itemCount());
      }
      else
      {
         reader->skipCurrentElement();
      }
   }

   insertItem(itemGroup, itemCount());
}
