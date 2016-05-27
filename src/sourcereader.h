/*!
 * \file sourcereader.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef SOURCEREADER_H
#define SOURCEREADER_H

#include <QObject>
#include <QRunnable>
#include <QString>

#include "item.h"
#include "items.h"
#include "source.h"

class QXmlStreamReader;

/*!
 * \brief A source reader.
 */
class SourceReader : public QObject, public QRunnable
{
   Q_OBJECT

public:
   /*!
    * Constructs a source reader to read the source found in \a file.
    */
   SourceReader(const QString& file);

   /*!
    * \reimp
    */
   void run() override;

signals:
   /*!
    * Is emitted when the source \a source has been read.
    */
   void sourceRead(const Source& source);

private:
   /*!
    * The file the source should be read from.
    */
   QString file_;

   /*!
    * Reads an item list from the XML stream \a reader and adds it to the list of items and
    * returns \a true if the items could be read; \a false otherwise.
    */
   void readItems_(Source* source, QXmlStreamReader* reader);
   /*!
    * Reads an item from the XML stream \a reader, adds the tags \a tags and adds it to the
    * list of items and returns \a true if the item could be read; \a false otherwise.
    */
   void readItem_(Source* source, QXmlStreamReader* reader, const QStringList& predefinedTags = QStringList());
   /*!
    * Reads a group from the XML stream \a reader and add it to the list of items and returns
    * \a true if the group could be read; \a false otherwise.
    */
   void readGroup_(Source* source, QXmlStreamReader* reader);
   /*!
    * Reads an imported source from the XML stream \a stream and returns \a true if the
    * imported source could be read; \a false otherwise.
    */
   void readImport_(Source* source, QXmlStreamReader* reader);
};

#endif // SOURCEREADER_H
