/*!
 * \file sourceeditor.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QDesktopWidget>
#include <QDialogButtonBox>
#include <QIODevice>
#include <QFile>
#include <QPlainTextEdit>
#include <QVBoxLayout>

#include "application.h"
#include "sourceeditor.h"
#include "sourcehighlighter.h"

SourceEditor::SourceEditor(QWidget* parent, Qt::WindowFlags windowFlags) : QDialog(parent, Qt::Tool | windowFlags)
{
   //
   // Create UI.
   //
   sourceEdit_ = new QPlainTextEdit(this);
   sourceEdit_->setLineWrapMode(QPlainTextEdit::NoWrap);
   sourceEdit_->setFocusPolicy(Qt::StrongFocus);
   sourceEdit_->document()->setDefaultFont(QFont(QStringLiteral("Lucida Console, Courier New")));

   setFocusPolicy(Qt::NoFocus);
   setFocusProxy(sourceEdit_);

   auto sourceHighlighter = new SourceHighlighter(this);
   sourceHighlighter->setDocument(sourceEdit_->document());

   auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
   buttonBox->connect(buttonBox, &QDialogButtonBox::accepted, [this](){
      bool result = source_->open(QIODevice::WriteOnly | QIODevice::Truncate);
      if (result == true)
      {
         source_->write(sourceEdit_->toPlainText().toUtf8());
         source_->close();

         done(Accepted);
      }
      else
      {
         done(Failed);
      }
   });
   buttonBox->connect(buttonBox, &QDialogButtonBox::rejected, [this](){
      done(Rejected);
   });

   //
   // Set layout.
   //
   auto layout = new QVBoxLayout(this);
   layout->addWidget(sourceEdit_);
   layout->addWidget(buttonBox);

   setLayout(layout);

   //
   // Register the widget with the geomerty store.
   //
   auto screenGeometry = QApplication::desktop()->screenGeometry();
   QSize defaultSize(screenGeometry.width() * 0.75, screenGeometry.height() * 0.75);
   QPoint defaultPosition((screenGeometry.width() - defaultSize.width()) / 2, (screenGeometry.height() - defaultSize.height()) / 2);

   static_cast<Application*>(Application::instance())->geometryStore().addWidget(this, QRect(defaultPosition, defaultSize));
}

bool SourceEditor::openSource(QIODevice* source)
{
   Q_ASSERT(source_ != nullptr);
   Q_ASSERT(sourceEdit_ != nullptr);

   //
   // Transfer ownership for the source to this object (it will be automatically deleted when
   // the parent is being deleted).
   //
   source_ = source;
   source_->setParent(this);

   //
   // Open the device for reading and display the content in the editor. Be aware to not open
   // the device for read/write, as this may cause to device to be truncated on for instance
   // WebDAV mounted files on Windows 10.
   //
   bool result = source_->open(QIODevice::ReadOnly);
   if (result == true)
   {
      sourceEdit_->setPlainText(QString::fromUtf8(source_->readAll()));
      source_->close();
   }

   return result;
}

void SourceEditor::selectSource(int line, int column, int size)
{
   //
   // Move the cursor to the requested location. At position zero the cursor is already at line
   // one, so move down the cursor on line less then requested. The column is zero. The text is
   // selected by advancing the cursor by the requested size, but keeping the anchor at the
   // position it was originally moved to.
   //
   auto textCursor = sourceEdit_->textCursor();
   textCursor.setPosition(0);
   textCursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line - 1);
   textCursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, column);
   textCursor.setPosition(textCursor.position() + size, QTextCursor::KeepAnchor);
   sourceEdit_->setTextCursor(textCursor);

}
