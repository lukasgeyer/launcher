/*!
 * \file source.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QWeakPointer>

#include "source.h"

namespace {

/*!
 * \brief A RAII object to register a type with the meta type system.
 */
struct MetaTypeRegistrar
{
   /*!
    * Registers the types with the meta type system.
    */
   MetaTypeRegistrar()
   {
      qRegisterMetaType<Source>("Source");
      qRegisterMetaType<QWeakPointer<Source>>("QWeakPointer<Source>");
   }
};

MetaTypeRegistrar metaTypeRegistrar;

} // namespace

Source::Source(const QString& file) : file_(file)
{
}

void Source::setFile(const QString& file)
{
   file_ = file;
}

QString Source::file() const
{
   return file_;
}

void Source::setError(const SourceError& error)
{
   error_ = error;
}

SourceError Source::error() const
{
   return error_;
}

void Source::appendItem(const Item& item)
{
   items_.append(item);
}

void Source::setItems(const Items& items)
{
   items_ = items;
}

Items Source::items() const
{
   return items_;
}

void Source::appendImport(const Import& import)
{
   imports_.append(import);
}

void Source::setImports(const Imports& imports)
{
   imports_ = imports;
}

Imports Source::imports() const
{
   return imports_;
}
