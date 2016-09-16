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

#include <QAbstractListModel>
#include <QString>
#include <QThreadPool>

#include "xmlitemsource.h"

/*!
 * \brief An item model representing the items found in an XML-based source file.
 */
class ItemModel : public QAbstractItemModel, public XmlItemSource
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
   QModelIndex index(int row, int column, const QModelIndex &parent) const override;
   /*!
    * \reimp
    */
   QModelIndex parent(const QModelIndex &child) const override;

   /*!
    * \reimp
    */
   int rowCount(const QModelIndex& parent) const override;
   /*!
    * \reimp
    */
   int columnCount(const QModelIndex &parent) const override;

   /*!
    * \reimp
    */
   QVariant data(const QModelIndex& index, int role) const override;

   /*!
    * \reimp
    */
   bool read(QIODevice* device) override;

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
