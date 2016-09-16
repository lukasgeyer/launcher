/*!
 * \file import.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef IMPORTER_H
#define IMPORTER_H

#include <memory>

#include <QObject>
#include <QPoint>
#include <QRunnable>
#include <QString>

#include "importitem.h"
#include "itemmodel.h"
#include "itemsource.h"

/*!
 * \brief An importer.
 */
class Importer : public QObject, public QRunnable
{
   Q_OBJECT

public:
   /*!
    * Constructs and importer for the import \a import and the item model identifier \a identifier.
    */
   explicit Importer(ImportItem* import, const QString& identifier);

   /*!
    * \reimp
    */
   void run() override;

signals:
   /*!
    * Is emitted when the import succeeded and resulted in the item source \a itemSource.
    */
   void suceeded(ImportItem* import, const QString& identifier, ItemSource* itemSource);
   /*!
    * Is emitted when the import for \a import failed.
    */
   void failed(ImportItem* import, const QString& identifier, const QString& error, const QPoint& errorPosition);

private:
   /*!
    * The import to be imported.
    */
   ImportItem* import_;
   /*!
    * The identifier of the item model that requested the import.
    */
   QString identifier_;
};

#endif // IMPORTER_H
