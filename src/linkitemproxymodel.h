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
#include <QVector>

class ItemGroup;

/*!
 * \brief A proxy model providing a link item model from an item model.
 */
class LinkItemProxyModel : public QAbstractProxyModel
{
public:
   /*!
    * \reimp
    */
   void setSourceModel(QAbstractItemModel* model) override;

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
   int rowCount(const QModelIndex &parent) const override;
   /*!
    * \reimp
    */
   int columnCount(const QModelIndex &parent) const override;

   /*!
    * \reimp
    */
   QVariant data(const QModelIndex &proxyIndex, int role) const override;

private:
   /*!
    * An index cache, mapping a source model index to a proxy model index.
    */
   QVector<QModelIndex> fromSourceIndexCache_;
   /*!
    * An index cache, mapping a proxy model index to a source model index.
    */
   QVector<QModelIndex> toSourceIndexCache_;

   /*!
    * Adds each link item in the item group \a item group to the cache.
    */
   void addItemGroup_(ItemGroup* itemGroup);
};

#endif // LINKITEMPROXYMODEL_H
