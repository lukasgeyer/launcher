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

#include <memory>
#include <utility>
#include <vector>

#include <QAbstractListModel>
#include <QString>
#include <QThreadPool>

#include "imports.h"
#include "itemsource.h"

/*!
 * \brief An item model representing the items found in an XML-based source file.
 */
class ItemModel : public QAbstractListModel
{
   Q_OBJECT

public:
   /*!
    * The identifier for a model.
    */
   typedef QString Identifier;

   /*!
    * The custom roles provided by this model.
    */
   enum Role
   {
      NameRole = Qt::DisplayRole, /*< The name for the item of type QString. */
      BrushRole = Qt::ForegroundRole, /*< The brush for the item of type QBrush. */
      LinkRole = Qt::UserRole, /*< The link for the item of type QString. */
      TagsRole /*< The tags for the item of type QStringList. */
   };

   /*!
    * Constructs an ItemModel with the parent \a parent.
    */
   ItemModel(QObject* parent = nullptr);

   /*!
    * Reads the item model from the file \a file.
    */
   void read(const QString& file);

   /*!
    * Resets the item model, by clearing all data and emitting the required signals to trigger
    * an update of any connected views.
    */
   void reset();

   /*!
    * \reimp
    */
   int rowCount(const QModelIndex& parent) const override;

   /*!
    * \reimp
    */
   QVariant data(const QModelIndex& index, int role) const override;

signals:
   /*!
    * Is emitted when the import \a import has been added to the model.
    */
   void importSucceeded(const Import& import);
   /*!
    * Is emitted when the import \a import has not been added to the model because of the
    * error \a errorString at the position \a errorPosition.
    */
   void importFailed(const Import& import, const QString& errorString, const QPoint& errorPosition);
   /*!
    * Is emitted when the import list has been reset.
    */
   void importReset();

protected:
   /*!
    * \reimp
    */
   void timerEvent(QTimerEvent *event) override;

private:
   /*!
    * The opaque identifier of the model. This identifier is unique and is regenerated each time
    * the model is reset. The main use is to relate an in-flight asnychronous import with the
    * correct epoch of a model (each time the model is reset a new epoch is dawn, and the result
    * of an asynchronous import of an older epoch must not be used to avoid incorrect entries).
    */
   Identifier identifier_;

   /*!
    * The sources of this model.
    */
   std::vector<std::shared_ptr<ItemSource>> itemSources_;

   /*!
    * The row cache for the model, mapping a row to the repsective item within an item group.
    */
   std::vector<std::pair<ItemGroup*, Item*>> itemSourcesCache_;

   /*!
    * The list of imports that are required to be loaded asynchronously.
    */
   Imports imports_;
   /*!
    * The thread pool used to asynchronously load the imports.
    */
   QThreadPool importsThreadPool_;

   /*!
    * Adds the item source \a item source to the list of item sources, updates the row cache and
    * emits the required signals to trigger an update of any connected views.
    */
   void addItemSource_(const std::shared_ptr<ItemSource>& itemSource);

   /*!
    * Adds the import \a import to the list of imports required to be loaded asynchronously.
    */
   void addImport_(const Import& import);
   /*!
    * Starts the asynchronous import of the import \a import.
    */
   void startImport_(const Import& import);
};

#endif // ITEMMODEL_H
