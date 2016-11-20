/*!
 * \file itemmodel.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMMODEL_H
#define ITEMMODEL_H

#include <chrono>
#include <functional>

#include <QAbstractListModel>
#include <QFileSystemWatcher>
#include <QString>
#include <QTimer>
#include <QUuid>

#include "groupitem.h"

class ImportItem;
class ImportItemReader;

/*!
 * \brief An item model representing the items found in an XML-based source file.
 */
class ItemModel : public QAbstractItemModel, public GroupItem
{
   Q_OBJECT

public:
   /*!
    * Constructs an ItemModel with the parent \a parent.
    */
   ItemModel(QObject* parent = nullptr);
   /*!
    * Destructs an ItemModel.
    */
   ~ItemModel();

   /*!
    * \reimp
    */
   QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
   /*!
    * \reimp
    */
   QModelIndex parent(const QModelIndex &child) const override;

   /*!
    * \reimp
    */
   int rowCount(const QModelIndex& parent = QModelIndex()) const override;
   /*!
    * \reimp
    */
   int columnCount(const QModelIndex &parent = QModelIndex()) const override;

   /*!
    * \reimp
    */
   QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

   /*!
    * Recursively applies the function \a function to each item of type \a Type.
    */
   template <typename Type> void apply(const std::function<void(const QModelIndex& index, Type* item)> function, const QModelIndex& parent = QModelIndex())
   {
      for (int row = 0; row < rowCount(parent); ++row)
      {
         const QModelIndex& childIndex = index(row, 0, parent);

         if (auto childItem = Item::cast<Type>(item(childIndex)))
         {
            function(childIndex, childItem);
         }

         apply<Type>(function, childIndex);
      }
   }

   /*!
    * Asynchronosly reads the model data from the file with the name \a fileName.
    */
   void read(const QString& file);
   /*!
    * Synchronously writes the model data to the file with the name \a fileName.
    */
   void write(const QString& fileName);

   /*!
    * Returns a pointer to the item at the index \a index or \a nullptr if there is no such item.
    */
   Item* item(const QModelIndex& index);
   /*!
    * Returns a pointer to the item at the index \a index or \a nullptr if there is no such item.
    */
   const Item* item(const QModelIndex& index) const;

   /*!
    * Returns the identifier the model war read from.
    */
   QString itemSourceIdentifier() const
   {
      return itemSourceFile_;
   }

signals:
   /*!
    * Is emitted when a source has been loaded.
    */
   void sourceLoaded(const QString& source);
   /*!
    * Is emitted when a source failed to load.
    */
   void sourceFailedToLoad(const QString& source, const QString& errorString = QString(), const QPoint& errorPosition = QPoint());

private:
   /*!
    * The file the model was originally populated from.
    */
   QString itemSourceFile_;

   /*!
    * The file system watcher checking for changes in any item source files.
    */
   QFileSystemWatcher itemSourceWatcher_;

   /*!
    * The list of import items to be reloaded asynchronously.
    */
   QVector<QPair<ImportItemReader*, std::chrono::steady_clock::time_point>> itemSourceReaderQueue_;
   /*!
    *
    * The opaque identifier of the model. This identifier is unique and is regenerated each time
    * the model is reset. The main use is to relate an in-flight asnychronous import with the
    * correct epoch of a model (each time the model is reset a new epoch is dawn, and the result
    * of an asynchronous import of an older epoch must not be used to avoid incorrect entries).
    */
   QUuid itemSourceReaderQueueIdentifier_;
   /*!
    * The timer triggering the processing of the item source reader queue.
    */
   QTimer itemSourceReaderQueueTimer_;

   /*!
    * Asynchronously reads the import item \a item and returns \a true if the item read process
    * could be started; \a false otherwise.
    */
   bool readItemSource_(const ImportItem& item);
};

#endif // ITEMMODEL_H
