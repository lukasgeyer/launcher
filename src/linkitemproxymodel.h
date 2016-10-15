/*!
 * \file linkitemproxymodel.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license the program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef LINKITEMPROXYMODEL_H
#define LINKITEMPROXYMODEL_H

#include <QAbstractProxyModel>
#include <QBrush>
#include <QModelIndex>
#include <QStringList>
#include <QVector>

#include "itemmodel.h"
#include "linkitem.h"

/*!
 * \brief A proxy model providing a link item model from an item model.
 */
class LinkItemProxyModel : public QAbstractProxyModel
{
public:
   /*!
    * Constructs a proxy model with the parent \a parent.
    */
   LinkItemProxyModel(QObject* parent = nullptr);

   /*!
    * \reimp
    */
   void setSourceModel(QAbstractItemModel* sourceModel);

   /*!
    * \reimp
    */
   QModelIndex index(int row, int column, const QModelIndex &parent) const override;
   /*!
    * \reimp
    */
   QModelIndex parent(const QModelIndex &child) const override;

   /*!
    * \reimp
    */
   QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
   /*!
    * \reimp
    */
   QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

   /*!
    * \reimp
    */
   int rowCount(const QModelIndex &parent = QModelIndex()) const override;
   /*!
    * \reimp
    */
   int columnCount(const QModelIndex &parent = QModelIndex()) const override;

   /*!
    * \reimp
    */
   QVariant data(const QModelIndex &proxyIndex, int role) const override;

   /*!
    * Returns a pointer to the item at the index \a proxyIndex or \a nullptr if there is no such item.
    */
   LinkItem* item(const QModelIndex& proxyIndex);
   /*!
    * Returns a pointer to the item at the index \a proxyIndex or \a nullptr if there is no such item.
    */
   const LinkItem* item(const QModelIndex& proxyIndex) const;

private:
   /*!
    * An entry in the cache.
    */
   struct CacheEntry_
   {
      QModelIndex index;
      QString tagString;
      QStringList tagStringList;
      QBrush brush;
   };
   /*!
    * The cache mapping a source model item properties.
    */
   QVector<CacheEntry_> cache_;

   /*!
    * Resets the model, removing all groups from the cache and adding all
    * item groups found in the source model.
    */
   void reset_();

   /*!
    * Returns \a true if \a row and \a column are valid; \a false otherwise.
    */
   inline bool isValid_(int row, int column) const
   {
      return ((row >= 0) && (row < cache_.size()) && (column >= 0) && (column < 2));
   }
   /*!
    * Returns \a true if the model index \a proxyIndex is valid; \a false otherwise.
    */
   inline bool isValid_(const QModelIndex& proxyIndex) const
   {
      return ((proxyIndex.isValid() ) && isValid_(proxyIndex.row(), proxyIndex.column()));
   }
};

#endif // LINKITEMPROXYMODEL_H
