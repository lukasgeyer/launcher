/*!
 * \file sourcereader.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QXmlStreamReader>

#include "sourcereader.h"

SourceReader::SourceReader(const QString& file) : file_(file)
{
}

void SourceReader::run()
{
   Source source(file_);

   ///
   /// Open the source file and parse the content.
   ///
   QFile sourceFileDevice(source.file());
   if (sourceFileDevice.open(QIODevice::ReadOnly))
   {
      qInfo() << "parse source:" << source.file();

      ///
      /// Parse the source file.
      ///
      QXmlStreamReader sourceFileReader(&sourceFileDevice);
      if ((sourceFileReader.readNextStartElement() == true) && (sourceFileReader.name() == "items"))
      {
         readItems_(&source, &sourceFileReader);
      }

      if (sourceFileReader.hasError() == true)
      {
         qCritical() << "parse source failed:" << source.file()
                                               << sourceFileReader.lineNumber() << sourceFileReader.columnNumber()
                                               << sourceFileReader.errorString();

         source.setError(SourceError(SourceError::Type::ParseError,
                                     sourceFileReader.errorString(),
                                     SourcePosition(sourceFileReader.lineNumber(), sourceFileReader.columnNumber())));
      }

      ///
      /// Close the source file.
      ///
      sourceFileDevice.close();
   }
   else
   {
      qCritical() << "open source failed:" << source.file() << sourceFileDevice.errorString();

      source.setError(SourceError(SourceError::Type::OpenError, sourceFileDevice.errorString()));
   }

   ///
   /// Emit the signal that the source has been read.
   ///
   emit sourceRead(source);
}

void SourceReader::readItems_(Source* source, QXmlStreamReader* reader)
{
   Q_ASSERT(source != nullptr);
   Q_ASSERT(reader != nullptr);

   ///
   /// Parse the <items> element.
   ///
   while (reader->readNextStartElement())
   {
      if (reader->name() == "item")
      {
         readItem_(source, reader);
      }
      else if (reader->name() == "group")
      {
         readGroup_(source, reader);
      }
      else if (reader->name() == "import")
      {
         readImport_(source, reader);
      }
      else
      {
         reader->skipCurrentElement();
      }
   }
}

void SourceReader::readItem_(Source* source, QXmlStreamReader* reader, const QStringList& tags)
{
   Q_ASSERT(source != nullptr);
   Q_ASSERT(reader != nullptr);

   Item item(source->file());

   ///
   /// Append existing tags.
   ///
   item.appendTags(tags);

   ///
   /// Parse the <item> element.
   ///
   while (reader->readNextStartElement())
   {
      if (reader->name() == "name")
      {
         item.setName(reader->readElementText().trimmed());
      }
      else if (reader->name() == "url")
      {
         SourcePosition sourcePosition;
         sourcePosition.setLineNumber(reader->lineNumber());
         sourcePosition.setColumnNumber(reader->columnNumber());

         const auto& elementText = reader->readElementText();

         sourcePosition.setSize(elementText.size());

         item.setLink(QUrl::fromUserInput(elementText.trimmed()));
         item.setLinkPosition(sourcePosition);

      }
      else if (reader->name() == "tag")
      {
         item.appendTag(reader->readElementText().trimmed());
      }
      else
      {
         reader->skipCurrentElement();
      }
   }

   ///
   /// Append the item to the source.
   ///
   qInfo() << "append item:" << item;

   source->appendItem(item);
}

void SourceReader::readGroup_(Source* source, QXmlStreamReader* reader)
{
   Q_ASSERT(source != nullptr);
   Q_ASSERT(reader != nullptr);

   QStringList tags;

   ///
   /// Parse the <group> element.
   ///
   while (reader->readNextStartElement())
   {
      if (reader->name() == "tag")
      {
         tags.append(reader->readElementText().trimmed());
      }
      else if (reader->name() == "item")
      {
         readItem_(source, reader, tags);
      }
      else
      {
         reader->skipCurrentElement();
      }
   }
}

void SourceReader::readImport_(Source* source, QXmlStreamReader *reader)
{
   Q_ASSERT(source != nullptr);
   Q_ASSERT(reader != nullptr);

   ///
   /// Parse the <import> element.
   ///
   while (reader->readNextStartElement())
   {
      if (reader->name() == "file")
      {
         source->appendImport(Import(QFileInfo(reader->readElementText().trimmed()).absoluteFilePath()));
      }
      else
      {
         reader->skipCurrentElement();
      }
   }
}
