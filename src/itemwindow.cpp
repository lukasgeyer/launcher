/*!
 * \file itemwindow.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFontDialog>
#include <QGraphicsDropShadowEffect>
#include <QIcon>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QSettings>
#include <QUrl>
#include <QVBoxLayout>

#include "itemfiltermodel.h"
#include "itemedit.h"
#include "itemhotkey.h"
#include "itemmodel.h"
#include "itemview.h"
#include "itemwindow.h"

namespace {

/*!
 * The size of the resize area (if the cursor is not within this area the window will be moved
 * instead of resized).
 */
const int RESIZE_AREA_SIZE_ = 16;


/*!
 * Returns the event \a event as \a EventType if the type is \a type; \a nullptr otherwise.
 */
template <typename EventType> EventType* eventAs(QEvent* event, QEvent::Type type)
{
   return ((event != nullptr) ? ((event->type() == type) ? (static_cast<EventType*>(event)) : (nullptr)) : (nullptr));
}

} // namespace

ItemWindow::ItemWindow(QWidget *parent) : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowSystemMenuHint)
{
   ///
   /// Disable window background.
   ///
   setAttribute(Qt::WA_NoSystemBackground, true);
   setAttribute(Qt::WA_TranslucentBackground, true);

   ///
   /// Set the window properties.
   ///
   setWindowIcon(QIcon(QStringLiteral(":/images/logo.svg")));
   setWindowOpacity(0.95);

   ///
   /// Create UI.
   ///
   auto itemModel = new ItemModel(this);

   auto itemFilterModel = new ItemFilterModel(this);
   itemFilterModel->setSourceModel(itemModel);
   itemFilterModel->sort(0);

   auto itemView = new ItemView(this);
   itemView->setEditTriggers(QListView::NoEditTriggers);
   itemView->setFocusPolicy(Qt::NoFocus);
   itemView->setModel(itemFilterModel);
   itemView->setModelColumn(0);
   itemView->setSelectionMode(QAbstractItemView::SingleSelection);
   itemView->connect(itemView, &ItemView::clicked, [this](const QModelIndex& index)
   {
      ///
      /// If an item is clicked open the link and remain shown (so multiple items can be clicked).
      ///
      openUrl_(index.data(ItemModel::LinkRole).toUrl());
   });

   auto itemEditShadowEffect = new QGraphicsDropShadowEffect;
   itemEditShadowEffect->setBlurRadius(16.0);
   itemEditShadowEffect->setOffset(0.0);

   auto itemEdit = new ItemEdit(this);
   itemEdit->installEventFilter(this);
   itemEdit->setClearButtonEnabled(true);
   itemEdit->setFocus();
   itemEdit->setFocusPolicy(Qt::StrongFocus);
   itemEdit->setGraphicsEffect(itemEditShadowEffect);
   itemEdit->connect(itemEdit, &ItemEdit::textChanged, [itemFilterModel](const QString& text){
      itemFilterModel->setFilterRegularExpressionPattern((text.isEmpty() ? "" : "^") + text);
      itemFilterModel->sortColumn();
   });
   itemEdit->connect(itemEdit, &ItemEdit::returnPressed, [this, itemEdit, itemView](){
      const auto& currentIndex = itemView->currentIndex();
      if (currentIndex.isValid() == true)
      {
         ///
         /// If enter is pressed and an item is selected open the link and hide.
         ///
         hide();
         openUrl_(currentIndex.data(ItemModel::LinkRole).toUrl());
      }
      else
      {
         ///
         /// If enter is pressed and no item is selected open all links if either a filter is
         /// provided (and a non-tagged item might be matched) or the item is tagged and hide.
         ///
         hide();
         for (int row = 0; row < itemView->model()->rowCount(); ++row)
         {
            const auto &rowIndex = itemView->model()->index(row, 0);
            if ((itemEdit->text().isEmpty() == false) || (rowIndex.data(ItemModel::TagsRole).toStringList().empty() == false))
            {
               openUrl_(rowIndex.data(ItemModel::LinkRole).toUrl());
            }
         }
      }
   });
   itemEdit->connect(itemEdit, &ItemEdit::keyUpPressed, [itemView](){
      const auto& currentIndex = itemView->currentIndex();
      itemView->setCurrentIndex(itemView->model()->index(qMax(-1, currentIndex.row() - 1), 0));
   });
   itemEdit->connect(itemEdit, &ItemEdit::keyDownPressed, [itemView](){
      const auto& currentIndex = itemView->currentIndex();
      itemView->setCurrentIndex(currentIndex.isValid() ? itemView->model()->index(qMin(itemView->model()->rowCount() - 1, currentIndex.row() + 1), 0) :
                                                         itemView->model()->index(0, 0));
   });
   itemEdit->connect(itemEdit, &ItemEdit::escapePressed, [this](){
      ///
      /// Is escaped is pressed hide.
      ///
      hide();
   });
   itemEdit->connect(itemModel, &ItemModel::modelUpdateSucceeded, [itemEdit](){
      itemEdit->removeError(QStringLiteral("modelError"));
   });
   itemEdit->connect(itemModel, &ItemModel::modelUpdateFailed, [itemEdit](const QString& reason) {
      itemEdit->addError(QStringLiteral("modelError"), reason);
   });

   ///
   /// Hide the application if it loses focus, as it cannot be activated programatically for
   /// instance on Windows if another application is activated. The application will simply
   /// flash in the taskbar instead. Another option would be hiding and showing the window,
   /// but this leads to unpleasent flickering, so the application is simply hidden.
   ///
   connect(qApp, &QApplication::applicationStateChanged, [this, itemEdit](Qt::ApplicationState applicationState){
      if (applicationState == Qt::ApplicationActive)
      {
         itemEdit->selectAll();
         itemEdit->setFocus();

         activateWindow();
      }
      else if (applicationState == Qt::ApplicationInactive)
      {
         hide();
      }
   });

   ///
   /// Decorate the standard context menu with the additional font selection action.
   ///
   auto itemEditContextMenu = itemEdit->createStandardContextMenu();
   itemEditContextMenu->addSeparator();
   itemEditContextMenu->addAction(tr("Select font..."), [itemEdit, itemView](){
      QSettings settings;

      auto fontSelected = false;
      auto font = QFontDialog::getFont(&fontSelected, QFont(settings.value(QStringLiteral("font/family")).toString(),
                                                            settings.value(QStringLiteral("font/size")).toInt(),
                                                            settings.value(QStringLiteral("font/weight")).toInt(),
                                                            settings.value(QStringLiteral("font/italic")).toBool()), itemEdit);
      if (fontSelected == true)
      {
         settings.setValue(QStringLiteral("font/family"), font.family());
         settings.setValue(QStringLiteral("font/size"), font.pointSize());
         settings.setValue(QStringLiteral("font/weight"), font.family());
         settings.setValue(QStringLiteral("font/italic"), font.italic());

         itemEdit->setFont(font);
         itemView->setFont(font);
      }
   });

   ///
   /// Show the decorated context menu instead of the standard context menu.
   ///
   itemEdit->setContextMenuPolicy(Qt::CustomContextMenu);
   itemEdit->connect(itemEdit, &ItemEdit::customContextMenuRequested, [itemEdit, itemEditContextMenu](const QPoint& position){
      itemEditContextMenu->popup(itemEdit->mapToGlobal(position));
   });

   ///
   /// Set layout.
   ///
   auto itemLayout = new QVBoxLayout;
   itemLayout->addWidget(itemEdit);
   itemLayout->addWidget(itemView);
   itemLayout->setSpacing(0);

   setLayout(itemLayout);

   ///
   /// Create the hotkey.
   ///
   auto itemHotkey = new ItemHotkey(this);
   itemHotkey->connect(itemHotkey, &ItemHotkey::hotkeyPressed, [this, itemEdit](){
      ///
      /// Show the window.
      ///
      showNormal();
   });

   ///
   /// Register the hotkey sequence.
   ///
   if (itemHotkey->registerKeySequence() == false)
   {
      itemEdit->addError(QStringLiteral("hotkeyError"), tr("The hotkey could not be registered."));
   }

   ///
   /// Restore settings.
   ///
   QSettings settings;

   ///
   /// Restore the geometry or resize and move the window to the default value if no geometry is stored.
   ///
   auto geometry = settings.value("itemWindow/geometry");
   if (geometry.isValid() == true)
   {
      restoreGeometry(geometry.toByteArray());
   }
   else
   {
      auto screenGeometry = QApplication::desktop()->screenGeometry();

      resize(screenGeometry.width() * 0.3, screenGeometry.height() * 0.6);
      move(screenGeometry.right() - width(), 0);
   }

   ///
   /// Restore the font or use the default font is no font is stored.
   ///
   auto defaultFont = QApplication::font();
   auto font = QFont(settings.value(QStringLiteral("font/family"), defaultFont.family()).toString(),
                     settings.value(QStringLiteral("font/size"), defaultFont.pointSize()).toInt(),
                     settings.value(QStringLiteral("font/weight"), defaultFont.weight()).toInt(),
                     settings.value(QStringLiteral("font/italic"), defaultFont.italic()).toBool());

   itemEdit->setFont(font);
   itemView->setFont(font);

   ///
   /// Update the source model. Be aware that this must be done after creating the UI, so that
   /// possible errors during the model update are properly indicated.
   ///
   itemModel->setSource(settings.value("sourceFile", QStringLiteral("launcher.xml")).toString());
}

ItemWindow::~ItemWindow()
{
}

void ItemWindow::closeEvent(QCloseEvent* event)
{
   ///
   /// Store the current window geometry.
   ///
   QSettings().setValue("itemWindow/geometry", saveGeometry());

   QWidget::closeEvent(event);
}

bool ItemWindow::eventFilter(QObject* object, QEvent* event)
{
   Q_ASSERT(event != nullptr);
   Q_ASSERT(object->isWidgetType());

   ///
   /// If \a true, the event shall be consumed (not passed to the object).
   ///
   bool consumeEvent = false;

   if (auto keyPressEvent = eventAs<QKeyEvent>(event, QEvent::KeyPress))
   {
      ///
      /// If the modifier is pressed the window modification operation may begin.
      ///
      if ((keyPressEvent->modifiers() & Qt::ShiftModifier) != Qt::NoModifier)
      {
         windowModificationModifierActive_ = true;
      }
   }
   else if (auto keyReleaseEvent = eventAs<QKeyEvent>(event, QEvent::KeyRelease))
   {
      ///
      /// If the modifier is released the window modification operation must end.
      ///
      if ((keyReleaseEvent->modifiers() & Qt::ShiftModifier) == Qt::NoModifier)
      {
         windowModificationModifierActive_ = false;

         windowModificationOperationActive_ = false;
         windowModificationOperation_ = WindowModificationOperation::None;

         static_cast<QWidget*>(object)->unsetCursor();
      }
   }
   else if (/* auto mouseButtonPressedEvent = */ eventAs<QMouseEvent>(event, QEvent::MouseButtonPress))
   {
      ///
      /// If the mouse button is pressed and the modifier is active the window modification
      /// operation begins.
      ///
      if (windowModificationModifierActive_ == true)
      {
         windowModificationOperationActive_ = true;

         consumeEvent = true;
      }
   }
   else if (/* auto mouseButtonReleasedEvent = */ eventAs<QMouseEvent>(event, QEvent::MouseButtonRelease))
   {
      ///
      /// If the mouse button is released the window modification operation ends.
      ///
      if (windowModificationModifierActive_ == true)
      {
         windowModificationOperationActive_ = false;

         consumeEvent = true;
      }
   }
   else if (auto mouseMovedEvent = eventAs<QMouseEvent>(event, QEvent::MouseMove))
   {
      ///
      /// If the mouse cursor is moved and the window modification operation is active perform
      /// the operation by modifiying the window geometry, either position or size, relative to
      /// the position of the cursor when the operation was started, the origin. If the window
      /// modification operation is not active just updated the origin, so the proper operation
      /// can be determined outside of this event.
      ///
      if (windowModificationOperationActive_ == true)
      {
         if (windowModificationOperation_ == WindowModificationOperation::Move)
         {
            setGeometry(geometry().x() + (mouseMovedEvent->x() - windowModificationOrigin_.x()),
                        geometry().y() + (mouseMovedEvent->y() - windowModificationOrigin_.y()),
                        geometry().width(),
                        geometry().height());
         }
         else if (windowModificationOperation_ == WindowModificationOperation::Resize)
         {
            setGeometry(geometry().x() + (mouseMovedEvent->x() - windowModificationOrigin_.x()),
                        geometry().y(),
                        geometry().width() - (mouseMovedEvent->x() - windowModificationOrigin_.x()),
                        geometry().height());
         }

         consumeEvent = true;
      }
      else
      {
         windowModificationOrigin_ = { mouseMovedEvent->x(), mouseMovedEvent->y() };
      }
   }

   if (windowModificationModifierActive_ == true)
   {
      ///
      /// If the modifier is pressed determine the window modification operation, which is either
      /// resize if the cursor is positioned near the left edge of the widget or move otherwise.
      ///
      auto widget = static_cast<QWidget*>(object);
      auto widgetPosition = widget->mapToGlobal(widget->pos());

      if (widget->underMouse() == true)
      {
         if ((widget->mapToGlobal(windowModificationOrigin_).x() >= (widgetPosition.x() - RESIZE_AREA_SIZE_)) &&
             (widget->mapToGlobal(windowModificationOrigin_).x() <= (widgetPosition.x() + RESIZE_AREA_SIZE_)))
         {
            if (windowModificationOperation_ != WindowModificationOperation::Resize)
            {
               windowModificationOperation_ = WindowModificationOperation::Resize;

               widget->setCursor(Qt::SizeHorCursor);
            }
         }
         else
         {
            if (windowModificationOperation_ != WindowModificationOperation::Move)
            {
               windowModificationOperation_ = WindowModificationOperation::Move;

               widget->setCursor(Qt::SizeAllCursor);
            }
         }
      }
   }

   return consumeEvent;
}

void ItemWindow::openUrl_(const QUrl& url)
{
   qInfo() << "url opened:" << url;

   ///
   /// Open the URL with the registered default application.
   ///
   QDesktopServices::openUrl(url);
}
