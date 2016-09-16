/*!
 * \file import.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QFile>
#include <QFileInfo>

#include "csvitemsource.h"
#include "importer.h"
#include "itemsource.h"
#include "itemsourcefactory.h"
#include "metatype.h"
#include "xmlitemsource.h"

namespace {

/*!
 * Returns a reference to the importer local item source factory. As the factory is the same for
 * all instances of the importer this instance can be used to avoid copies of the factory.
 */
const ItemSourceFactory& itemSourceFactory_()
{
   static auto itemSourceFactory = ItemSourceFactory(ItemSourceFactory::Declaration<CsvItemSource>(QStringLiteral("text/csv")),
                                                     ItemSourceFactory::Declaration<XmlItemSource>(QStringLiteral("text/xml")));

   return itemSourceFactory;
}

} // namespace

Importer::Importer(ImportItem* import, const QString& identifier) : import_(import), identifier_(identifier)
{
   Q_ASSERT(import != nullptr);
}

void Importer::run()
{
//   bool result = false;

//   QString errorString;
//   QPoint errorPosition;

//   QFile itemSourceFile(import_.file());
//   if (itemSourceFile.open(QIODevice::ReadOnly) == true)
//   {
//      auto itemSource = itemSourceFactory_().create(import_->mimeType(), QFileInfo(import_->file()).absoluteFilePath());
//      if (itemSource != nullptr)
//      {
//         result = itemSource->read(&itemSourceFile);
//         if (result == true)
//         {
//            emit suceeded(import_, identifier_, itemSource);
//         }
//         else
//         {
//            errorString = itemSource->errorString();
//            errorPosition = itemSource->errorPosition();
//         }
//      }
//      else
//      {
//         errorString = (tr("Unsupported MIME type: \"").append(import_->mimeType()).append("\""));
//      }
//   }
//   else
//   {
//      errorString = itemSourceFile.errorString();
//   }

//   if (result == false)
//   {
//      emit failed(import_, identifier_, errorString, errorPosition);
//   }
}
