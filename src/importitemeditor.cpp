/*!
 * \file importitemeditor.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QCompleter>
#include <QFormLayout>
#include <QLineEdit>

#include "application.h"
#include "importitemeditor.h"

ImportItemEditor::ImportItemEditor(QWidget* parent) : ItemEditor(parent)
{
   auto mimeTypeCompleter = new QCompleter(static_cast<Application*>(Application::instance())->itemSourceFactory()->mimeTypes(), this);
   mimeTypeCompleter->setCompletionMode(QCompleter::PopupCompletion);

   auto mimeTypeEdit = new QLineEdit;
   mimeTypeEdit->setCompleter(mimeTypeCompleter);

   auto layout = new QFormLayout;
   layout->addRow(tr("File"), new QLineEdit);
   layout->addRow(tr("Mime Type"), mimeTypeEdit);

   setLayout(layout);
}

void ImportItemEditor::read(Item* item)
{

}

void ImportItemEditor::write(Item* item)
{

}
