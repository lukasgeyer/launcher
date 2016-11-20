/*!
 * \file itemsourcereader.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QSharedPointer>

#include "application.h"
#include "importitemreader.h"

void ImportItemReader::run()
{
   Q_ASSERT(itemSource_);

   QFile file(importItem_.file());
   if (file.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      itemSource_->reset();

      if (itemSource_->read(&file, importItem_.file()))
      {
         emit sourceLoaded();
      }
      else
      {
         emit sourceFailedToLoad(itemSource_->errorString(), itemSource_->errorPosition());
      }
   }
   else
   {
      emit sourceFailedToLoad(file.errorString());
   }
}
