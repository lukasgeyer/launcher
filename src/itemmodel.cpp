/*!
 * \file itemmodel.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QDebug>
#include <QFile>

#include "itemmodel.h"

ItemModel::ItemModel(QObject* parent) : QAbstractListModel(parent)
{
   ///
   /// Reset the model when the source has changed.
   ///
   sourceFileWatcher_.connect(&sourceFileWatcher_, &QFileSystemWatcher::fileChanged, this, &ItemModel::readSource_);
}

bool ItemModel::setSource(const QString& sourceFile)
{
   qInfo() << "set source:" << sourceFile;

   ///
   /// Remove previous model source from watcher.
   ///
   foreach (const auto& file, sourceFileWatcher_.files())
   {
      sourceFileWatcher_.removePath(file);
   }

   ///
   /// Add current model source to watcher.
   ///
   sourceFileWatcher_.addPath(sourceFile);

   ///
   /// Parse model source.
   ///
   return readSource_(sourceFile);
}

QVariant ItemModel::data(const QModelIndex &index, int role) const
{
   Q_ASSERT(index.row() < items_.count());

   return ((role == ItemModel::NameRole) ? (items_[index.row()].name) :
           (role == ItemModel::LinkRole) ? (items_[index.row()].link) :
           (role == ItemModel::TagsRole) ? (items_[index.row()].tags) : QVariant());
}

int ItemModel::rowCount(const QModelIndex &) const
{
   return items_.count();
}

bool ItemModel::readSource_(const QString& sourceFile)
{
   bool result = false;

   ///
   /// Reset the model, so that the view is automatically updated when reset has ended (and
   /// endResetModel() is called).
   ///
   beginResetModel();

   ///
   /// Open the source file and parse the content.
   ///
   QFile sourceFileDevice(sourceFile);
   if (sourceFileDevice.open(QIODevice::ReadOnly) == true)
   {
      qInfo() << "parse source:" << sourceFile;

      QXmlStreamReader reader(&sourceFileDevice);

      if ((reader.readNextStartElement() == true) && (reader.name() == "items"))
      {
         items_ = readItems_(&reader);
      }

      result = reader.hasError();
      if (result == true)
      {
         qCritical() << "parse source failed:" << sourceFile << "-" << reader.errorString() << "at" << reader.lineNumber() << reader.columnNumber();
      }
   }
   else
   {
      qCritical() << "open source failed:" << sourceFile << "-" << sourceFileDevice.errorString();
   }

   endResetModel();

   return result;
}

ItemModel::Items_ ItemModel::readItems_(QXmlStreamReader* reader)
{
   Q_ASSERT(reader != nullptr);

   Items_ links;

   while (reader->readNextStartElement())
   {
      if (reader->name() == "item")
      {
         links.append(readItem_(reader));

         qInfo() << "source item read:" << links.back().name << links.back().link.toString() << links.back().tags;
      }
      else if (reader->name() == "group")
      {
         links.append(readGroup_(reader));
      }
      else
      {
         reader->skipCurrentElement();
      }
   }

   return links;
}

ItemModel::Item_ ItemModel::readItem_(QXmlStreamReader* reader)
{
   Q_ASSERT(reader != nullptr);

   Item_ link;

   while (reader->readNextStartElement())
   {
      if (reader->name() == "name")
      {
         link.name = reader->readElementText();
      }
      else if (reader->name() == "url")
      {
         link.link = QUrl::fromUserInput(reader->readElementText());
      }
      else if (reader->name() == "tag")
      {
         link.tags.push_back(reader->readElementText());
      }
      else
      {
         reader->skipCurrentElement();
      }
   }

   return link;
}

ItemModel::Items_ ItemModel::readGroup_(QXmlStreamReader* reader)
{
   Q_ASSERT(reader != nullptr);

   Items_ links;
   QStringList tags;

   while (reader->readNextStartElement())
   {
      if (reader->name() == "tag")
      {
         tags.append(reader->readElementText());
      }
      else if (reader->name() == "item")
      {
         links.append(readItem_(reader));
         links.back().tags.append(tags);

         qInfo() << "source item read:" << links.back().name << links.back().link.toString() << links.back().tags;
      }
      else
      {
         reader->skipCurrentElement();
      }
   }

   return links;
}
