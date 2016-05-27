/*!
 * \file itemmodel.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <algorithm>

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QThreadPool>

#include "itemmodel.h"
#include "sourcereader.h"

ItemModel::ItemModel(QObject* parent) : QAbstractListModel(parent)
{
   connect(&sourceFileWatcher_, &QFileSystemWatcher::fileChanged, this, &ItemModel::resetSource_);
}

void ItemModel::setSourceFile(const QString& sourceFile)
{
   qInfo() << "set source:" << sourceFile;

   sourceFile_ = sourceFile;

   resetSource_();
}

QString ItemModel::sourceFile() const
{
   return sourceFile_;
}

QVariant ItemModel::data(const QModelIndex &index, int role) const
{
   QVariant data;

   if (index.isValid() == true)
   {
      data = ((role == ItemModel::NameRole)         ? (QVariant::fromValue(items_[index.row()].name()))            :
              (role == ItemModel::LinkRole)         ? (QVariant::fromValue(items_[index.row()].link()))            :
              (role == ItemModel::LinkPositionRole) ? (QVariant::fromValue(items_[index.row()].linkPosition()))    :
              (role == ItemModel::TagsRole)         ? (QVariant::fromValue(items_[index.row()].tags()))            :
              (role == ItemModel::SourceRole)       ? (QVariant::fromValue(items_[index.row()].sourceFile()))      :
              (role == Qt::ToolTipRole)             ? (QVariant::fromValue(items_[index.row()].link().toString())) : QVariant());
   }

   return data;
}

int ItemModel::rowCount(const QModelIndex & /* parent */) const
{
   return items_.count();
}

void ItemModel::applySource(const Source& source)
{
   if (source.error().type() == SourceError::Type::None)
   {
      ///
      /// Notify any connected view that a model change is about to happend.
      ///
      beginResetModel();

      ///
      /// Remove any items from this source from the items list.
      ///
      items_.erase(std::remove_if(items_.begin(),
                                  items_.end(),
                                  [&source](const Item& item) { return (item.sourceFile() == source.file()); }), items_.end());

      ///
      /// Append all items from this source to the items list.
      ///
      items_.append(source.items());

      ///
      /// Notify any connected view that a model change has happended.
      ///
      endResetModel();

      ///
      /// Read the import if it has not been processed yet.
      ///
      for (const auto& import : source.imports())
      {
         if (sourceFileWatcher_.files().contains(import.file()) == false)
         {
            sourceFileWatcher_.addPath(import.file());

            readSource_(import.file());
         }
      }
   }
   else
   {
      ///
      /// Emit a model update failure if an import could not be loaded.
      ///
      emit modelUpdateFailed(source.error().text(), source.file(), source.error().position());
   }
}

void ItemModel::resetSource_()
{
   ///
   /// Notify any connected view that a model change is about to happend.
   ///
   beginResetModel();

   ///
   /// Remove any items.
   ///
   items_.clear();

   ///
   /// Notify any connected view that a model change has happended.
   ///
   endResetModel();

   ///
   /// Remove any watched source files and append just the root source file.
   ///
   const auto& files = sourceFileWatcher_.files();
   if (files.empty() == false)
   {
      sourceFileWatcher_.removePaths(sourceFileWatcher_.files());
   }
   const auto& directories = sourceFileWatcher_.directories();
   if (directories.empty() == false)
   {
      sourceFileWatcher_.removePaths(sourceFileWatcher_.directories());
   }

   sourceFileWatcher_.addPath(sourceFile_);

   ///
   /// Emit a successful model update so that the error is cleared.
   ///
   emit modelUpdateSucceeded();

   ///
   /// Read the source.
   ///
   readSource_(sourceFile_);
}

void ItemModel::readSource_(const QString& file)
{
   ///
   /// Create a source reader for this file.
   ///
   auto sourceReader = new SourceReader(file);
   sourceReader->setAutoDelete(true);
   sourceReader->connect(sourceReader, &SourceReader::sourceRead, this, &ItemModel::applySource);

   ///
   /// Asychnorounsly execute the source reader.
   ///
   QThreadPool::globalInstance()->start(sourceReader);
}

