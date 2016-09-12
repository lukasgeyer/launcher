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
#include "xmlitemsource.h"

namespace {

/*!
 * Returns a reference to the importer local item source factory. As the factory is the same for
 * all instances of the importer this instance can be used to avoid copies of the factory.
 */
const ItemSourceFactory& itemSourceFactory_()
{
   static auto itemSourceFactory = ItemSourceFactory(ItemSourceFactory::Declaration<CsvItemSource>(QStringLiteral("csv")),
                                                     ItemSourceFactory::Declaration<XmlItemSource>(QStringLiteral("xml")));

   return itemSourceFactory;
}

} // namespace

Importer::Importer(const Import& import, const ItemModel::Identifier& identifier) : import_(import), identifier_(identifier)
{
}

void Importer::run()
{
   bool result = false;

   QFile itemSourceFile(import_.file());
   if (itemSourceFile.open(QIODevice::ReadOnly) == true)
   {
      auto itemSource = itemSourceFactory_().create(import_.type(), QFileInfo(import_.file()).absoluteFilePath());
      if (itemSource)
      {
         result = itemSource->read(&itemSourceFile);
         if (result == true)
         {
            emit suceeded(import_, identifier_, std::shared_ptr<ItemSource>(itemSource.release()));
         }
      }
   }

   if (result == false)
   {
      emit failed(import_, identifier_);
   }
}
