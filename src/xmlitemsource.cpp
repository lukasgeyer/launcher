/*!
 * \file xmlitemsource.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "xmlitemsource.h"

namespace {

/*!
 * The default indentation when writing the document.
 */
static const int DEFAULT_INDENT_ = 3;

} // namespace

XmlItemSource::XmlItemSource(const QString& identifier) : ItemSource(identifier)
{
   reset();
}

bool XmlItemSource::read(QIODevice* device)
{
   Q_ASSERT(itemGroups_.isEmpty() == false);

   bool result = false;

   if (device != nullptr)
   {
      QXmlStreamReader deviceReader(device);
      if ((deviceReader.readNextStartElement() == true) && (deviceReader.name() == "items"))
      {
         while (deviceReader.readNextStartElement())
         {
            if (deviceReader.name() == "item")
            {
               readItem_(&deviceReader, &itemGroups_.first());
            }
            else if (deviceReader.name() == "group")
            {
               readGroup_(&deviceReader);
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

      result = (deviceReader.hasError() == false);
      if (result == false)
      {
         documentError_ = deviceReader.errorString();
         documentErrorPosition_ = {static_cast<int>(deviceReader.lineNumber()),
                                   static_cast<int>(deviceReader.columnNumber())};
      }
   }

   return result;
}

bool XmlItemSource::write(QIODevice* device) const
{
   Q_ASSERT(itemGroups_.isEmpty() == false);

   bool result = false;

   if (device != nullptr)
   {
      QXmlStreamWriter deviceWriter(device);
      deviceWriter.setAutoFormatting(true);
      deviceWriter.setAutoFormattingIndent(DEFAULT_INDENT_);

      deviceWriter.writeStartDocument();
      deviceWriter.writeStartElement("items");

      for (const auto& itemGroup : itemGroups_)
      {
         if (&itemGroup != &itemGroups_.first())
         {
            deviceWriter.writeStartElement("group");

            for (const auto& tag : itemGroup.tags())
            {
               deviceWriter.writeTextElement("tag", tag);
            }
         }

         for(const auto& item : itemGroup.items())
         {
            deviceWriter.writeStartElement("item");

            deviceWriter.writeTextElement("name", item.name());
            deviceWriter.writeTextElement("url", item.link().toString());
            deviceWriter.writeTextElement("color", item.brush().color().name());

            for (const auto& tag : item.tags())
            {
               deviceWriter.writeTextElement("tag", tag);
            }

            deviceWriter.writeEndElement();
         }

         if (&itemGroup != &itemGroups_.first())
         {
            deviceWriter.writeEndElement();
         }
      }

      deviceWriter.writeEndElement();
      deviceWriter.writeEndDocument();

      result = (deviceWriter.hasError() == false);
      if (result == false)
      {
         documentError_ = device->errorString();
         documentErrorPosition_ = {0, 0};
      }
   }

   return result;
}

void XmlItemSource::reset()
{
   //
   // There is at least one item group, the global item group, which contains any items
   // outside of an explicit item group and a list of imports. This item group is always
   // the first item group.
   //
   itemGroups_.clear();
   itemGroups_.append(ItemGroup{});
}

const ItemGroups& XmlItemSource::itemGroups() const
{
   return itemGroups_;
}

const Imports& XmlItemSource::imports() const
{
   return imports_;
}

QString XmlItemSource::lastError() const
{
   return documentError_;
}

QPoint XmlItemSource::lastErrorPosition() const
{
   return documentErrorPosition_;
}

void XmlItemSource::readItem_(QXmlStreamReader* reader, ItemGroup *itemGroup)
{
   Q_ASSERT(reader != nullptr);
   Q_ASSERT(itemGroup != nullptr);

   Item item;

   while (reader->readNextStartElement())
   {
      if (reader->name() == "name")
      {
         item.setName(reader->readElementText().trimmed());
      }
      else if (reader->name() == "url")
      {
         item.setLink(QUrl::fromUserInput(reader->readElementText().trimmed()));
      }
      else if (reader->name() == "color")
      {
         item.setBrush(QBrush(QColor(reader->readElementText().trimmed())));
      }
      else if (reader->name() == "tag")
      {
         item.appendTag(Tag(reader->readElementText().trimmed()));
      }
      else
      {
         reader->skipCurrentElement();
      }
   }

   itemGroup->appendItem(item);
}

void XmlItemSource::readGroup_(QXmlStreamReader* reader)
{
   Q_ASSERT(reader != nullptr);

   itemGroups_.append(ItemGroup{});

   while (reader->readNextStartElement())
   {
      if (reader->name() == "item")
      {
         readItem_(reader, &itemGroups_.last());
      }
      else if (reader->name() == "color")
      {
         itemGroups_.last().setBrush(QBrush(QColor(reader->readElementText().trimmed())));
      }
      else if (reader->name() == "tag")
      {
         itemGroups_.last().appendTag(reader->readElementText().trimmed());
      }
      else
      {
         reader->skipCurrentElement();
      }
   }
}

void XmlItemSource::readImport_(QXmlStreamReader* reader)
{
   Q_ASSERT(reader != nullptr);

   while (reader->readNextStartElement())
   {
      if (reader->name() == "file")
      {
         imports_.append(Import(reader->readElementText().trimmed(), reader->attributes().hasAttribute("type") ?
                                                                     reader->attributes().value("type").toString().trimmed() :
                                                                     QStringLiteral("xml")));
      }
      else
      {
         reader->skipCurrentElement();
      }
   }
}
