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
//   if (itemSourceFile.open(QIODevice::ReadOnly))
//   {
//      auto itemSource = itemSourceFactory_().create(import_->mimeType(), QFileInfo(import_->file()).absoluteFilePath());
//      if (itemSource != nullptr)
//      {
//         result = itemSource->read(&itemSourceFile);
//         if (result )
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
