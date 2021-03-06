/*!
 * \file itemsourceeditor.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMSOURCEFILEEDITOR_H
#define ITEMSOURCEFILEEDITOR_H

#include <QDialog>

class QIODevice;
class QPlainTextEdit;

/*!
 * \brief An editor for item source files.
 */
class ItemSourceEditor : public QDialog
{
   Q_OBJECT

public:
   /*!
    * Constructs an SourceFileEditor for the source file \a sourceFile with the parent \a parent.
    */
   ItemSourceEditor(QWidget* parent = nullptr, Qt::WindowFlags windowFlags = Qt::WindowFlags());

   /*!
    * Opens the source from the device \a source and returns \a true if the source could be opened;
    * \a false otherwise. Ownership is transferred to the source editor.
    */
   bool openSource(QIODevice* source);
   /*!
    * Selects the source at character offset \a characterOffset for a size of \a size.
    */
   void selectSource(int line, int column, int size);

private:
   /*!
    * The source.
    */
   QIODevice* source_;
   /*!
    * The source editor.
    */
   QPlainTextEdit* sourceEdit_;
};

#endif // ITEMSOURCEEDITOR_H
