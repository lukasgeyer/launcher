/*!
 * \file itemmodel.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <chrono>

#include <QUuid>

#include "application.h"
#include "import.h"
#include "importer.h"
#include "itemmodel.h"
#include "itemsource.h"

namespace {

static const std::chrono::milliseconds DEFAULT_IMPORT_RETRY_TIMEOUT_ = std::chrono::seconds(10);

} // namespace

ItemModel::ItemModel(QObject* parent) : QAbstractListModel(parent)
{
   startTimer(static_cast<int>(DEFAULT_IMPORT_RETRY_TIMEOUT_.count()));
}

void ItemModel::read(const QString& file)
{
   startImport_(Import(file, QStringLiteral("xml")));
}

void ItemModel::reset()
{
   beginResetModel();

   identifier_ = QUuid().toString();

   itemSources_.clear();
   itemSourcesCache_.clear();

   imports_.clear();
   importsThreadPool_.clear();

   endResetModel();
}

QVariant ItemModel::data(const QModelIndex &index, int role) const
{
   QVariant data;

   if ((index.isValid() == true) && (index.row() >= 0) && (index.row() < static_cast<int>(itemSourcesCache_.size())) && (index.column() == 0))
   {
      switch (static_cast<Role>(role))
      {
      case NameRole:
         data = itemSourcesCache_[index.row()].second->name();
         break;
      case BrushRole:
         data = itemSourcesCache_[index.row()].second->brush();
         break;
      case LinkRole:
         data = itemSourcesCache_[index.row()].second->link();
         break;
      case TagsRole:
         data = itemSourcesCache_[index.row()].first->tags() + itemSourcesCache_[index.row()].second->tags();
         break;
      }
   }

   return data;
}

void ItemModel::timerEvent(QTimerEvent* /* event */)
{
   for (const auto& import : imports_)
   {
      startImport_(import);
   }
}

int ItemModel::rowCount(const QModelIndex & /* parent */) const
{
   return static_cast<int>(itemSourcesCache_.size());
}

void ItemModel::addItemSource_(const std::shared_ptr<ItemSource>& itemSource)
{
   Q_ASSERT(itemSource);

   beginResetModel();

   for (const auto &itemGroup : itemSource->itemGroups())
   {
      for (const auto &item : itemGroup.items())
      {
         itemSourcesCache_.push_back(std::make_pair(const_cast<ItemGroup*>(&itemGroup), const_cast<Item*>(&item)));
      }
   }

   itemSources_.push_back(std::move(itemSource));

   endResetModel();
}

void ItemModel::addImport_(const Import& import)
{
   imports_.append(import);
}

void ItemModel::startImport_(const Import& import)
{
   auto importer = new Importer(import, identifier_);
   if (importer != nullptr)
   {
      connect(importer, &Importer::suceeded, [this](const Import& import, const Identifier& identifier, const std::shared_ptr<ItemSource>& itemSource)
      {
         if (identifier == identifier_)
         {
            qDebug() << "import succeeded:" << import;

            addItemSource_(itemSource);

            for (const auto& import : itemSource->imports())
            {
               startImport_(import);
            }
         }
      });

      connect(importer, &Importer::failed, [this](const Import &import, const Identifier& identifier)
      {
         if (identifier == identifier_)
         {
            qDebug() << "import failed:" << import;

            addImport_(import);
         }
      });

      importsThreadPool_.start(importer);
   }
}
