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
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVector>
#include <QXmlStreamReader>

/*!
 * \brief An item model representing the items found in an XML-based source file.
 */
class ItemModel : public QAbstractListModel
{
public:
   /*!
    * The custom roles provided by this model.
    */
   enum Role
   {
      NameRole = Qt::DisplayRole, /*< The name for the item of type QString. */
      LinkRole = Qt::UserRole + 1, /*< The link for the item of type QString. */
      TagsRole = Qt::UserRole + 2 /*< The tags for the item of the QStringList. */
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
   bool setSource(const QString& sourceFile);

   /*!
    * \reimp
    */
   QVariant data(const QModelIndex& index, int role) const override;

   /*!
    * \reimp
    */
   int rowCount(const QModelIndex& /* parent */) const override;

private:
   /*!
    * An item.
    */
   struct Item_
   {
      QString name;
      QUrl link;
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
    * The watcher responsible for the source file.
    */
   QFileSystemWatcher sourceFileWatcher_;

   /*!
    * Reads the data from the file \a sourceFile and returns \a true when the data could be
    * loaded; \a false otherwise.
    */
   bool readSource_(const QString& sourceFile);
   /*!
    * Reads an item list from the XML stream \a reader and returns it.
    */
   Items_ readItems_(QXmlStreamReader* reader);
   /*!
    * Reads an item from the XML stream \a reader and returns it.
    */
   Item_ readItem_(QXmlStreamReader* reader);
   /*!
    * Reads a group from the XML stream \a reader and returns it.
    */
   Items_ readGroup_(QXmlStreamReader* reader);
};

#endif // ITEMMODEL_H
