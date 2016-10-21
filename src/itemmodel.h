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

#include <functional>

#include <QAbstractListModel>
#include <QString>
#include <QThreadPool>

#include "groupitem.h"

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
   void read(const QString& fileName);
   /*!
    * Synchronously writes the model data to the file with the name \a fileName.
    */
   void write(const QString& fileName);

   /*!
    * Inserts the item \a item at row \a row with the parent \a parent. If \a row is -1 the item
    * is appended last. If the item could be appended to the model \a true is returned; \a false
    * otherwise (if for instance the parent cannot such an item type). If the item has been
    * inserted the model takes ownership of the item.
    */
   bool insertItem(Item* item, int row, const QModelIndex& parent);
   /*!
    * Removes the item at the index \a index from the model and returns \a true if the item could
    * be removed or \a false otherwise (if for instance the item could not be found).
    */
   bool removeItem(const QModelIndex& index);

   /*!
    * Returns a pointer to the item at the index \a index or \a nullptr if there is no such item.
    */
   Item* item(const QModelIndex& index);
   /*!
    * Returns a pointer to the item at the index \a index or \a nullptr if there is no such item.
    */
   const Item* item(const QModelIndex& index) const;

private:
   /*!
    * The opaque identifier of the model. This identifier is unique and is regenerated each time
    * the model is reset. The main use is to relate an in-flight asnychronous import with the
    * correct epoch of a model (each time the model is reset a new epoch is dawn, and the result
    * of an asynchronous import of an older epoch must not be used to avoid incorrect entries).
    */
   QString identifier_;
};

#endif // ITEMMODEL_H
