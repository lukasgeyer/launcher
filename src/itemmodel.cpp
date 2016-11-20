/*!
 * \file itemmodel.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <algorithm>
#include <chrono>

#include <QFileInfo>
#include <QThreadPool>

#include "application.h"
#include "importgroupitem.h"
#include "itemmodel.h"
#include "importitemreader.h"
#include "linkgroupitem.h"
#include "xmlitemsource.h"

namespace {

/*!
 * The default timeout for retrying failed sources.
 */
static const std::chrono::milliseconds DEFAULT_IMPORT_RETRY_TIMEOUT_ = std::chrono::seconds(10);

} // namespace

ItemModel::ItemModel(QObject* parent) : QAbstractItemModel(parent)
{
   itemSourceWatcher_.connect(&itemSourceWatcher_, &QFileSystemWatcher::fileChanged, [this](const QString& file)
   {
      //
      // If the item source has changed reload the whole item model. It would be possible to
      // reload just the item source that has changed, but this requires a dependency tree
      // to be maintained, as the reloaded item source may have removed an import item, which
      // prevented the load of a former duplicate item source.
      //

      qInfo() << "item source changed" << file;

      read(itemSourceFile_);
   });

   itemSourceReaderQueueTimer_.start(1000);
   itemSourceReaderQueueTimer_.connect(&itemSourceReaderQueueTimer_, &QTimer::timeout, [this]()
   {
      //
      // The item source reader queue contains readers, which failed to load their source, either
      // because there was a parsing error or the file did not exist. Schedule this failed readers
      // periodically, so any files that have been added are reloaded. The file system watcher
      // cannot be used for this task, as it cannot be used to monitor files which do not exist.
      // In addition, it may be very possible that the file indeed exists, but is not yet reachable
      // because it resides for instance on a network share.
      //

      auto now = std::chrono::steady_clock::now();

      for (auto itemSourceReader = std::begin(itemSourceReaderQueue_); itemSourceReader != std::end(itemSourceReaderQueue_); )
      {
         if (std::chrono::duration_cast<std::chrono::milliseconds>(now - itemSourceReader->second) > DEFAULT_IMPORT_RETRY_TIMEOUT_)
         {
            qInfo() << "retry item source" << itemSourceReader->first->importItem();

            QThreadPool::globalInstance()->start(itemSourceReader->first);

            itemSourceReader = itemSourceReaderQueue_.erase(itemSourceReader);
         }
         else
         {
            ++itemSourceReader;
         }
      }
   });
}

ItemModel::~ItemModel()
{
}

void ItemModel::read(const QString& file)
{
   //
   // Store the item source file.
   //

   itemSourceFile_ = file;

   //
   // Discard any item source watcher.
   //

   if (!itemSourceWatcher_.files().isEmpty())
   {
      itemSourceWatcher_.removePaths(itemSourceWatcher_.files());
   }

   //
   // Discard any pending asynchronous item read operations,
   //

   itemSourceReaderQueue_.clear();
   itemSourceReaderQueueIdentifier_ = QUuid::createUuid();

   //
   // Discard any existing items.
   //

   GroupItem::removeItems();

   //
   // Read the item source.
   //

   readItemSource_(ImportItem(file, QStringLiteral("text/xml")));
}

void ItemModel::write(const QString& /* fileName */)
{
}

QModelIndex ItemModel::index(int row, int column, const QModelIndex& parent) const
{
   QModelIndex index;

   const GroupItem* groupItem = this;
   if (parent.isValid())
   {
      groupItem = static_cast<const GroupItem*>(parent.internalPointer());
   }

   Item* item = const_cast<Item*>(groupItem->item(row));
   if (item != nullptr)
   {
      index = createIndex(row, column, item);
   }

   return index;
}

QModelIndex ItemModel::parent(const QModelIndex& child) const
{
   QModelIndex index;

   Item* item = static_cast<GroupItem*>(child.internalPointer());
   if (item != nullptr)
   {
      GroupItem* parentItem = static_cast<GroupItem*>(item->parent());
      if (parentItem != this)
      {
         index = createIndex(0, 0, item->parent());
      }
   }

   return index;
}

int ItemModel::rowCount(const QModelIndex& parent) const
{
   int rowCount = 0;

   auto item = ((!parent.isValid()) ? (this) : (Item::cast<GroupItem>(static_cast<Item*>(parent.internalPointer()))));
   if (item != nullptr)
   {
      rowCount = item->items().count();
   }

   return rowCount;
}

int ItemModel::columnCount(const QModelIndex& /* parent */) const
{
   return 1;
}

QVariant ItemModel::data(const QModelIndex &index, int role) const
{
   QVariant data;

   if (index.isValid())
   {
      if (auto item = Item::cast<LinkItem>(static_cast<Item*>(index.internalPointer())))
      {
         switch (role)
         {
         case Qt::DisplayRole:
            data = item->name();
            break;
         case Qt::ToolTipRole:
         case Qt::StatusTipRole:
            data = item->link();
            break;
         }
      }
      else if (auto item = Item::cast<ImportItem>(static_cast<Item*>(index.internalPointer())))
      {
         switch (role)
         {
         case Qt::DisplayRole:
            data = QFileInfo(item->file()).fileName();
            break;
         case Qt::ToolTipRole:
         case Qt::StatusTipRole:
            data = item->file();
            break;
         }
      }
      else if (auto item = Item::cast<GroupItem>(static_cast<Item*>(index.internalPointer())))
      {
         switch (role)
         {
         case Qt::DisplayRole:
         case Qt::ToolTipRole:
         case Qt::StatusTipRole:
            data = item->name();
            break;
         }
      }
   }

   return data;
}

Item* ItemModel::item(const QModelIndex& index)
{
   Item* item = nullptr;

   if (index.isValid())
   {
      item = static_cast<Item*>(index.internalPointer());
   }

   return item;
}

const Item* ItemModel::item(const QModelIndex& index) const
{
   return const_cast<const Item*>(const_cast<ItemModel*>(this)->item(index));
}

bool ItemModel::readItemSource_(const ImportItem& item)
{
   bool result = false;

   //
   // Asynchronosly read the import item. If the item cannot be created because of an invalid MIME
   // time do not create an periodically retried item source reader for it, as there is no chance
   // of the item being loaded successfully ever.
   //

   qInfo() << "read item source" << item;

   auto itemSource = static_cast<Application*>(Application::instance())->itemSourceFactory()->create(item.mimeType());
   if (itemSource)
   {
      auto itemSourceReader = new ImportItemReader(item, std::move(itemSource), itemSourceReaderQueueIdentifier_);
      if (itemSourceReader)
      {
         itemSourceReader->setAutoDelete(false);
         itemSourceReader->connect(itemSourceReader, &ImportItemReader::sourceLoaded, this, [this, itemSourceReader]()
         {
            auto itemSource = itemSourceReader->releaseItemSource();

            //
            // If the item has been loaded successfully check if it is part of the current mode
            // epoch and add it to the item model if not already present (to avoid duplicate or
            // recursive entries). This has to be done after the file has been loaded, as the
            // canonical path (which is the required identifier to check for a duplicate) is
            // available for an existing file only (which must be the case if the item was read
            // successfully).
            //

            qDebug() << "item source loaded" << itemSourceReader->importItem() << itemSource->items().length();

            if (itemSourceReader->identifier() == itemSourceReaderQueueIdentifier_)
            {
               auto canonicalImportFilePath = QFileInfo(itemSourceReader->importItem().file()).canonicalFilePath();
               if (!itemSourceWatcher_.files().contains(canonicalImportFilePath))
               {
                  //
                  // Add the canonical file path to the file system watcher so any change is
                  // detected and a duplicate check can be performed (based on the fact if
                  // the file is already being watched).
                  //

                  itemSourceWatcher_.addPath(canonicalImportFilePath);

                  //
                  // Recursively parse the imported item for any other import item.
                  //

                  itemSource->apply<ImportItem, ImportGroupItem>([this](ImportItem* item) { readItemSource_(*item); });

                  //
                  // Release the item from the import item reader and add it to the model.
                  //

                  beginResetModel();
                  GroupItem::insertItem(itemSource.release(), itemCount());
                  endResetModel();

                  emit sourceLoaded(itemSourceReader->importItem().file());
               }
               else
               {
                  qInfo() << "duplicate item source discarded" << canonicalImportFilePath;
               }

               itemSourceReader->deleteLater();
            }
            else
            {
               qDebug() << "obsolete item source discarded" << itemSourceReader->identifier() << itemSourceReaderQueueIdentifier_;

               itemSourceReader->deleteLater();
            }
         }, Qt::QueuedConnection);
         itemSourceReader->connect(itemSourceReader, &ImportItemReader::sourceFailedToLoad, this,
                                   [this, itemSourceReader](const QString& errorString, const QPoint& errorPosition)
         {
            qInfo() << "item source failed to load" << itemSourceReader->importItem();

            if (itemSourceReader->identifier() == itemSourceReaderQueueIdentifier_)
            {
               //
               // If the item cannot be loaded successfully add it to the queue of readers to
               // be retried at some later point in time,
               //

               qInfo() << "item source retry scheduled";

               itemSourceReaderQueue_.append(qMakePair(itemSourceReader, std::chrono::steady_clock::now()));

               emit sourceFailedToLoad(itemSourceReader->importItem().file(), errorString, errorPosition);
            }
            else
            {
               qDebug() << "obsolete item source discarded" << itemSourceReader->identifier() << itemSourceReaderQueueIdentifier_;

               itemSourceReader->deleteLater();
            }
         }, Qt::QueuedConnection);

         QThreadPool::globalInstance()->start(itemSourceReader);
      }
   }
   else
   {
      qWarning() << "invalid MIME type" << item;

      emit sourceFailedToLoad(item.file(), tr("Invalid MIME type: ").append(item.mimeType()));
   }

   return result;
}
