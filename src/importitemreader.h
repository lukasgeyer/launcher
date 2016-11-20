/*!
 * \file importitemreader.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef IMPORTITEMREADER_H
#define IMPORTITEMREADER_H

#include <memory>

#include <QFile>
#include <QObject>
#include <QPoint>
#include <QRunnable>
#include <QString>
#include <QUuid>

#include "importitem.h"
#include "itemsource.h"

class ItemSource;

class ImportItemReader : public QObject, public QRunnable
{
   Q_OBJECT

public:
   /*!
    * Constructs an import item reader for the import item \a importItem, which shall be read
    * to the item source \a itemSource. The reader will take ownership of the item source. The
    * supplied identifier is stored and can be retrieved using identifier().
    */
   ImportItemReader(const ImportItem& importItem, std::unique_ptr<ItemSource> itemSource, const QUuid& identifier) :
      importItem_(importItem), itemSource_(std::move(itemSource)), identifier_(identifier)
   {
   }

   /*!
    * \reimp
    */
   void run() override;

   /*!
    * Returns the import item.
    */
   const ImportItem& importItem() const
   {
      return importItem_;
   }
   /*!
    * Returns the item source and passes ownership to the caller. If the item source is no
    * longer owned by the reader \a nullptr is retured.
    */
   std::unique_ptr<ItemSource> releaseItemSource()
   {
      return std::unique_ptr<ItemSource>(itemSource_.release());
   }

   /*!
    * Returns the identifier of the reader.
    */
   const QUuid& identifier() const
   {
      return identifier_;
   }

signals:
   /*!
    * Is emitted when the source with the identifier \a identifier has been successfully loaded.
    */
   void sourceLoaded();
   /*!
    * Is emitted when the source with the identifier \a identifier has failed to load.
    */
   void sourceFailedToLoad(const QString& errorString = QString(), const QPoint& errorPosition = QPoint());

private:
   /*!
    * The import item to be read.
    */
   ImportItem importItem_;
   /*!
    * The source the import item should be read to.
    */
   std::unique_ptr<ItemSource> itemSource_;

   /*!
    * The identifier to be used.
    */
   QUuid identifier_;
};

#endif // IMPORTITEMREADER_H
