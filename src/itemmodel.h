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
#include <QFileSystemWatcher>
#include <QIODevice>
#include <QPoint>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QUuid>
#include <QVector>
#include <QXmlStreamReader>

#include "item.h"
#include "items.h"
#include "source.h"
#include "sourceposition.h"

/*!
 * \brief An item model representing the items found in an XML-based source file.
 */
class ItemModel : public QAbstractListModel
{
   Q_OBJECT

public:
   /*!
    * The custom roles provided by this model.
    */
   enum Role
   {
      NameRole = Qt::DisplayRole, /*< The name for the item of type QString. */
      LinkRole = Qt::UserRole, /*< The link for the item of type QString. */
      LinkPositionRole, /*< The link position for the item of type QPoint. */
      TagsRole, /*< The tags for the item of type QStringList. */
      SourceRole /*< The source for the item of type QString. */
   };

   /*!
    * Constructs an ItemModel with the parent \a parent.
    */
   ItemModel(QObject* parent = nullptr);

   /*!
    * Sets the source for this model to the file \a sourceFile and returns \a true when the
    * data could be loaded from the file; \a false otherwise. If the file is modified the data
    * is reloaded.
    */
   void setSourceFile(const QString& sourceFile);
   /*!
    *  Returns the source for this model.
    */
   QString sourceFile() const;

   /*!
    * \reimp
    */
   int rowCount(const QModelIndex& parent) const override;

   /*!
    * \reimp
    */
   QVariant data(const QModelIndex& index, int role) const override;

   /*!
    * Applies the source \a source to the model, adding all items and read the imported sources
    * if neccessary.
    */
   void applySource(const Source& source, const QUuid& uuid);

signals:
   /*!
    * Is emitted when the model has been successfully updated.
    */
   void modelUpdateSucceeded();
   /*!
    * Is emitted when the model has been unsuccessfully updated. The reason why updating the
    * model has failed is indicated in \a reason. The source position (if available) is stored
    * in \a sourcePosition.
    */
   void modelUpdateFailed(const QString& reason, const QString& sourceFile, const SourcePosition& sourcePosition);

private:
   /*!
    * The source file of this model.
    */
   QString sourceFile_;
   /*!
    * The source UUID os this model. This UUID will be regenerated each time the source
    * model is reset.
    */
   QUuid sourceUuid_;

   /*!
    * The list of failed sources.
    */
   QVector<QString>failedSources_;

   /*!
    * The items in this model.
    */
   Items items_;

   /*!
    * The watcher responsible for the source files.
    */
   QFileSystemWatcher sourceFileWatcher_;

   /*!
    * Resets the source for this model, effectively removing all items and imports.
    */
   void resetSource_();

   /*!
    * Updates the model from the source \a source.
    */
   void readSource_(const QString& file);

private slots:
   /*!
    * Retries failed sources.
    */
   void readFailedSources_();
};

#endif // ITEMMODEL_H
