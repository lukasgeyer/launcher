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
#include <QFileInfo>

#include "itemmodel.h"

ItemModel::ItemModel(QObject* parent) : QAbstractListModel(parent)
{
   ///
   /// Reset the model when the source has changed.
   ///
   importedSourcesFileWatcher_.connect(&importedSourcesFileWatcher_, &QFileSystemWatcher::fileChanged, this, &ItemModel::updateModel_);
}

void ItemModel::setSource(const QString& source)
{
   qInfo() << "set source:" << source;

   rootSource_ = QFileInfo(source).absoluteFilePath();

   updateModel_();
}

QString ItemModel::source() const
{
   return rootSource_;
}

QVariant ItemModel::data(const QModelIndex &index, int role) const
{
   Q_ASSERT(index.row() < items_.count());

   return ((role == ItemModel::NameRole)         ? (QVariant::fromValue(items_[index.row()].name))            :
           (role == ItemModel::LinkRole)         ? (QVariant::fromValue(items_[index.row()].link))            :
           (role == ItemModel::LinkPositionRole) ? (QVariant::fromValue(items_[index.row()].linkPosition))    :
           (role == ItemModel::TagsRole)         ? (QVariant::fromValue(items_[index.row()].tags))            :
           (role == ItemModel::SourceRole)       ? (QVariant::fromValue(items_[index.row()].source))          :
           (role == Qt::ToolTipRole    )         ? (QVariant::fromValue(items_[index.row()].link.toString())) : QVariant());
}

int ItemModel::rowCount(const QModelIndex &) const
{
   return items_.count();
}

bool ItemModel::updateModel_()
{
   qInfo() << "update model from source:" << rootSource_;

   ///
   /// Begin resetting the model, so that the view is automatically updated when reset has ended.
   ///
   beginResetModel();

   ///
   /// Remove current imported sources.
   ///
   importedSources_.clear();

   foreach (const auto& file, importedSourcesFileWatcher_.files())
   {
      importedSourcesFileWatcher_.removePath(file);
   }

   ///
   /// Remove current items.
   ///
   items_.clear();

   ///
   /// Parse model source, which may add additional items to the item list and files to the file
   /// system watcher.
   ///
   bool result = readSource_(rootSource_);
   if (result == true)
   {
      emit modelUpdateSucceeded();
   }

   ///
   /// End resetting the model, so that the view is automatically updated.
   ///
   endResetModel();

   return result;
}

bool ItemModel::readSource_(const QString& sourceFile)
{
   bool result = true;

   ///
   /// Use absolute path.
   ///
   auto absoluteSourceFilePath = QFileInfo(sourceFile).absoluteFilePath();

   ///
   /// Import each source once to avoid circular dependencies.
   ///
   if (importedSources_.contains(absoluteSourceFilePath) == false)
   {
      importedSources_.append(absoluteSourceFilePath);

      ///
      /// Open the source file and parse the content.
      ///
      QFile sourceFileDevice(absoluteSourceFilePath);
      result = sourceFileDevice.open(QIODevice::ReadOnly);
      if (result == true)
      {
         qInfo() << "parse source:" << sourceFile;

         ///
         /// Add the source file to the source file system watcher, so a change triggers a
         /// model update.
         ///
         importedSourcesFileWatcher_.addPath(absoluteSourceFilePath);

         ///
         /// Parse the source file.
         ///
         QXmlStreamReader sourceFileReader(&sourceFileDevice);
         if ((sourceFileReader.readNextStartElement() == true) && (sourceFileReader.name() == "items"))
         {
            result = readItems_(&sourceFileReader);
         }

         if (sourceFileReader.hasError() == true)
         {
            qCritical() << "parse source failed:" << sourceFile
                                                  << sourceFileReader.lineNumber() << sourceFileReader.columnNumber()
                                                  << sourceFileReader.errorString();

            emit modelUpdateFailed(QStringLiteral("In %1 at %2:%3 %4").arg(sourceFile)
                                                                      .arg(sourceFileReader.lineNumber()).arg(sourceFileReader.columnNumber())
                                                                      .arg(sourceFileReader.errorString()),
                                   absoluteSourceFilePath,
                                   SourcePosition(sourceFileReader.lineNumber(),
                                                  sourceFileReader.columnNumber()));
            result = false;
         }

         ///
         /// Close the source file.
         ///
         sourceFileDevice.close();
      }
      else
      {
         qCritical() << "open source failed:" << sourceFile << sourceFileDevice.errorString();

         emit modelUpdateFailed(QStringLiteral("%1: %2").arg(sourceFile).arg(sourceFileDevice.errorString()),
                                absoluteSourceFilePath,
                                SourcePosition());
      }
   }
   else
   {
      qInfo() << "ignore source:" << sourceFile;
   }

   return result;
}

bool ItemModel::readItems_(QXmlStreamReader* reader)
{
   Q_ASSERT(reader != nullptr);

   bool result = true;

   ///
   /// Parse the <items> element.
   ///
   while (reader->readNextStartElement())
   {
      if (reader->name() == "item")
      {
         result &= readItem_(reader);
      }
      else if (reader->name() == "group")
      {
         result &= readGroup_(reader);
      }
      else if (reader->name() == "import")
      {
         result &= readImport_(reader);
      }
      else
      {
         reader->skipCurrentElement();
      }
   }

   return ((result == true) && (reader->hasError() == false));
}

bool ItemModel::readItem_(QXmlStreamReader* reader, const QStringList& tags)
{
   Q_ASSERT(reader != nullptr);

   Item_ item;

   ///
   /// Store the current source file as item source (if read from a file).
   ///
   if (auto readerFile = qobject_cast<QFile*>(reader->device()))
   {
      item.source = readerFile->fileName();
   }

   ///
   /// Append the passed tags to the item.
   ///
   item.tags.append(tags);

   ///
   /// Parse the <item> element.
   ///
   while (reader->readNextStartElement())
   {
      if (reader->name() == "name")
      {
         item.name = reader->readElementText();
      }
      else if (reader->name() == "url")
      {
         item.linkPosition.setLineNumber(reader->lineNumber());
         item.linkPosition.setColumnNumber(reader->columnNumber());

         const auto& elementText = reader->readElementText();

         item.link = QUrl::fromUserInput(elementText);

         item.linkPosition.setSize(elementText.size());
      }
      else if (reader->name() == "tag")
      {
         item.tags.push_back(reader->readElementText());
      }
      else
      {
         reader->skipCurrentElement();
      }
   }

   ///
   /// Append the item to the items list if there was no error.
   ///
   bool result = (reader->hasError() == false);
   if (result == true)
   {
      qInfo() << "append item:" << item;

      items_.append(std::move(item));
   }

   return result;
}

bool ItemModel::readGroup_(QXmlStreamReader* reader)
{
   Q_ASSERT(reader != nullptr);

   bool result = true;

   QStringList tags;

   ///
   /// Parse the <group> element.
   ///
   while (reader->readNextStartElement())
   {
      if (reader->name() == "tag")
      {
         tags.append(reader->readElementText());
      }
      else if (reader->name() == "item")
      {
         result &= readItem_(reader, tags);
      }
      else
      {
         reader->skipCurrentElement();
      }
   }

   return ((result == true) && (reader->hasError() == false));
}

bool ItemModel::readImport_(QXmlStreamReader *reader)
{
   Q_ASSERT(reader != nullptr);

   bool result = false;

   ///
   /// Parse the <import> element.
   ///
   while (reader->readNextStartElement())
   {
      if (reader->name() == "file")
      {
         result &= readSource_(reader->readElementText());
      }
      else
      {
         reader->skipCurrentElement();
      }
   }

   return ((result == true) && (reader->hasError() == false));
}

QDebug operator<<(QDebug stream, const ItemModel::Item_ &item)
{
   return (stream << "name:" << item.name <<
                     "link:" << item.link.toString() <<
                     "tags:" << item.tags);
}
