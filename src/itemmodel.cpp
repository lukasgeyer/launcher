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
#include <QSettings>
#include <QThreadPool>
#include <QTimer>

#include "application.h"
#include "itemmodel.h"
#include "sourcereader.h"

ItemModel::ItemModel(QObject* parent) : QAbstractListModel(parent)
{
   const auto& settings = static_cast<Application*>(QCoreApplication::instance())->settings();

   auto readFailedSourcesTimer = new QTimer(this);
   readFailedSourcesTimer->connect(readFailedSourcesTimer, &QTimer::timeout, this, &ItemModel::readFailedSources_);
   readFailedSourcesTimer->start(settings.value(QStringLiteral("readFailedSourcesTimeout"), QStringLiteral("10000")).toInt());

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

void ItemModel::applySource(const Source& source, const QUuid& uuid)
{
   ///
   /// The UUID must match with the currently valid UUID. A mismatch might happen if for
   /// instance the source is reset while an asynchronous source reader is currently is
   /// progress. The result of such a read must not be added to the item list.
   ///
   if (sourceUuid_ == uuid)
   {
      if (source.error().type() == SourceError::Type::None)
      {
         ///
         /// Notify any connected view that a model change is about to happend.
         ///
         beginResetModel();

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

         ///
         /// Add the source to the list of failed sources, so it will be processed later
         /// on again.
         ///
         failedSources_.push_back(source.file());
      }
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
   items_ = Items();

   ///
   /// Notify any connected view that a model change has happended.
   ///
   endResetModel();

   ///
   /// Regenerate the UUID.
   ///
   sourceUuid_ = QUuid::createUuid();

   ///
   /// Clear any pending failed sources.
   ///
   failedSources_.clear();

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
   auto sourceReader = new SourceReader(file, sourceUuid_);
   sourceReader->setAutoDelete(true);
   sourceReader->connect(sourceReader, &SourceReader::sourceRead, this, &ItemModel::applySource, Qt::QueuedConnection);

   ///
   /// Asychnorounsly execute the source reader.
   ///
   QThreadPool::globalInstance()->start(sourceReader);
}

void ItemModel::readFailedSources_()
{
   ///
   /// Read each failed source.
   ///
   for (const auto& failedSource : failedSources_)
   {
      qInfo() << "retry failed source: " << failedSource;

      readSource_(failedSource);
   }

   ///
   /// Clear the list.
   ///
   failedSources_.clear();
}

