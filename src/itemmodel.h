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
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVector>
#include <QXmlStreamReader>

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
      LinkRole = Qt::UserRole + 1, /*< The link for the item of type QString. */
      LinkPositionRole = Qt::UserRole + 2, /*< The link position for the item of type QPoint. */
      TagsRole = Qt::UserRole + 3, /*< The tags for the item of type QStringList. */
      SourceRole = Qt::UserRole + 4 /*< The source for the item of type QString. */
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
   void setSource(const QString& source);
   /*!
    *  Returns the source for this model.
    */
   QString source() const;

   /*!
    * \reimp
    */
   QVariant data(const QModelIndex& index, int role) const override;

   /*!
    * \reimp
    */
   int rowCount(const QModelIndex& /* parent */) const override;

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
   void modelUpdateFailed(const QString& reason, const QString& source, const SourcePosition& sourcePosition);

private:
   /*!
    * The root source for the model.
    */
   QString rootSource_;
   /*!
    * The imported sources file the model.
    */
   QStringList importedSources_;
   /*!
    * The watcher responsible for the source files.
    */
   QFileSystemWatcher importedSourcesFileWatcher_;

   /*!
    * An item.
    */
   struct Item_
   {
      QString source;
      QString name;
      QUrl link;
      SourcePosition linkPosition;
      QStringList tags;
   };
   /*!
    *  A list of items.
    */
   typedef QVector<Item_> Items_;
   /*!
    * The list of items.
    */
   Items_ items_;

   /*!
    * Updates the model from the source and returns \a true if the items could be updated;
    * \a false otherwise.
    */
   bool updateModel_();

   /*!
    * Reads the data from the file \a sourceFile and returns \a true when the data could be
    * loaded; \a false otherwise.
    */
   bool readSource_(const QString& sourceFile);
   /*!
    * Reads an item list from the XML stream \a reader and adds it to the list of items and
    * returns \a true if the items could be read; \a false otherwise.
    */
   bool readItems_(QXmlStreamReader* reader);
   /*!
    * Reads an item from the XML stream \a reader, adds the tags \a tags and adds it to the
    * list of items and returns \a true if the item could be read; \a false otherwise.
    */
   bool readItem_(QXmlStreamReader* reader, const QStringList& tags = QStringList());
   /*!
    * Reads a group from the XML stream \a reader and add it to the list of items and returns
    * \a true if the group could be read; \a false otherwise.
    */
   bool readGroup_(QXmlStreamReader* reader);
   /*!
    * Reads an imported source from the XML stream \a stream and returns \a true if the
    * imported source could be read; \a false otherwise.
    */
   bool readImport_(QXmlStreamReader* reader);

   /*!
    * Inserts the item \a item into the stream \a stream and returns the stream.
    */
   friend QDebug operator<<(QDebug stream, const Item_ &item);
};

#endif // ITEMMODEL_H
