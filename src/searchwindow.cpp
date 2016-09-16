/*!
 * \file searchwindow.cpp
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
#include <QUrl>
#include <QVBoxLayout>

#include "application.h"
#include "event.h"
#include "itemedit.h"
#include "itemfiltermodel.h"
#include "itemmodel.h"
#include "itemview.h"
#include "linkitemproxymodel.h"
#include "searchwindow.h"
#include "systemhotkey.h"

namespace {

/*!
 * The size of the resize area (if the cursor is not within this area the window will be moved
 * instead of resized).
 */
const int RESIZE_AREA_SIZE_ = 16;

} // namespace

SearchWindow::SearchWindow(QWidget *parent) : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint)
{
   //
   // Disable window background.
   //
   setAttribute(Qt::WA_NoSystemBackground, true);
   setAttribute(Qt::WA_TranslucentBackground, true);

   //
   // Set the window properties.
   //
   setWindowIcon(QIcon(QStringLiteral(":/images/logo.svg")));
   setWindowOpacity(0.95);

   //
   // Create UI.
   //
   auto itemModel = new ItemModel(this);

   auto linkItemProxyModel = new LinkItemProxyModel(this);
   linkItemProxyModel->setSourceModel(itemModel);

   auto itemFilterModel = new ItemFilterModel(this);
   itemFilterModel->setSourceModel(linkItemProxyModel);
   itemFilterModel->sort(0);

   auto itemView = new ItemView(this);
   itemView->setEditTriggers(QListView::NoEditTriggers);
   itemView->setFocusPolicy(Qt::NoFocus);
   itemView->setModel(itemFilterModel);
   itemView->setModelColumn(0);
   itemView->setSelectionMode(QAbstractItemView::SingleSelection);
   itemView->setContextMenuPolicy(Qt::CustomContextMenu);

   auto searchExpressionEditShadowEffect = new QGraphicsDropShadowEffect;
   searchExpressionEditShadowEffect->setBlurRadius(16.0);
   searchExpressionEditShadowEffect->setOffset(0.0);

   searchExpressionEdit_ = new ItemEdit(this);
   searchExpressionEdit_->installEventFilter(this);
   searchExpressionEdit_->setClearButtonEnabled(true);
   searchExpressionEdit_->setFocus();
   searchExpressionEdit_->setFocusPolicy(Qt::StrongFocus);
   searchExpressionEdit_->setGraphicsEffect(searchExpressionEditShadowEffect);
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::textChanged, [itemFilterModel](const QString& text){
      itemFilterModel->setSearchExpression(text);
      itemFilterModel->sortColumn();
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::returnPressed, [this, itemView](){
      const auto& currentIndex = itemView->currentIndex();
      if (currentIndex.isValid() == true)
      {
         //
         // If enter is pressed and an item is selected open the link and hide if the link
         // could be opened. In addition, the last URL open error is cleared. Remain shown
         // otherwise so the error can be seen.
         //
//         if (openUrl_(QUrl::fromUserInput(currentIndex.data(static_cast<int>(ItemModel::Role::Link)).value<QString>())) == true)
//         {
//            hide();
//         }
      }
      else
      {
         //
         // If enter is pressed and no item is selected open all links if either a filter is
         // provided (and a non-tagged item might be matched) or the item is tagged and hide
         // if all links could be opened. In addition, the last URL open error is cleared.
         // Remain shown otherwise so the error can be seen.
         //
         bool urlOpened = true;
         for (int row = 0; row < itemView->model()->rowCount(); ++row)
         {
//            const auto &rowIndex = itemView->model()->index(row, 0);
//            if ((searchExpressionEdit_->text().isEmpty() == false) ||
//                (rowIndex.data(static_cast<int>(ItemModel::Role::Tags)).value<QStringList>().empty() == false))
//            {
//               urlOpened &= openUrl_(QUrl::fromUserInput(currentIndex.data(static_cast<int>(ItemModel::Role::Link)).value<QString>()));
//            }
         }

         if (urlOpened == true)
         {
            hide();
         }
      }
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::keyUpPressed, [itemView](){
      const auto& currentIndex = itemView->currentIndex();
      itemView->setCurrentIndex(itemView->model()->index(qMax(-1, currentIndex.row() - 1), 0));
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::keyDownPressed, [itemView](){
      const auto& currentIndex = itemView->currentIndex();
      itemView->setCurrentIndex(currentIndex.isValid() ? itemView->model()->index(qMin(itemView->model()->rowCount() - 1, currentIndex.row() + 1), 0) :
                                                         itemView->model()->index(0, 0));
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::escapePressed, [this](){
      //
      // If escaped is pressed hide.
      //
      hide();
   });
//   itemModel->connect(itemModel, &ItemModel::importSucceeded, [this](const ImportItem& import) {
//      searchExpressionEdit_->removeIndication(QStringLiteral("importError:").append(import.file()));
//   });
//   itemModel->connect(itemModel, &ItemModel::importFailed, [this](const ImportItem& import, const QString& errorString, const QPoint& /* errorPosition */){
//      searchExpressionEdit_->addInidication(QStringLiteral("importError:").append(import.file()),
//                                            QStringLiteral("Import failed: ").append(import.file()).append(QStringLiteral(": ")).append(errorString));
//   });

   itemView->connect(itemView, &ItemView::clicked, [this](const QModelIndex& index)
   {
      //
      // If an item is clicked open the link and remain shown (so multiple items can be clicked).
      //
//      openUrl_(QUrl::fromUserInput(index.data(static_cast<int>(ItemModel::Role::Link)).value<QString>()));
   });
   itemView->connect(itemView, &ItemView::customContextMenuRequested, [this, itemView](const QPoint& position){
      //
      // Show the item-specific context menu if a valid item has been selected.
      //
      auto positionIndex = itemView->indexAt(position);
      if (positionIndex.isValid() == true)
      {
         QMenu itemViewContextMenu;
         itemViewContextMenu.addAction(tr("Edit item..."), [this, itemView, positionIndex](){
            // openSource
         });
         itemViewContextMenu.exec(itemView->mapToGlobal(position));
      }
   });

   setFocusPolicy(Qt::NoFocus);
   setFocusProxy(searchExpressionEdit_);

   //
   // Hide the application if it loses focus, as it cannot be activated programatically for
   // instance on Windows if another application is activated. The application will simply
   // flash in the taskbar instead. Another option would be hiding and showing the window,
   // but this leads to unpleasent flickering, so the application is simply hidden.
   //
   connect(qApp, &QApplication::applicationStateChanged, [this](Qt::ApplicationState applicationState){
      if (applicationState == Qt::ApplicationActive)
      {
         searchExpressionEdit_->selectAll();

         activateWindow();
      }
      else if (applicationState == Qt::ApplicationInactive)
      {
         hide();
      }
   });

   //
   // Decorate the standard context menu with the additional actions.
   //
   auto itemEditContextMenu = searchExpressionEdit_->createStandardContextMenu();
   itemEditContextMenu->addSeparator();
   itemEditContextMenu->addAction(tr("Select font..."), [this, itemView](){
      auto& settings = static_cast<Application*>(Application::instance())->settings();

      auto fontSelected = false;
      auto font = QFontDialog::getFont(&fontSelected, QFont(settings.value(QStringLiteral("font/family")).value<QString>(),
                                                            settings.value(QStringLiteral("font/size")).value<int>(),
                                                            settings.value(QStringLiteral("font/weight")).value<int>(),
                                                            settings.value(QStringLiteral("font/italic")).value<bool>()), searchExpressionEdit_);
      if (fontSelected == true)
      {
         settings.setValue(QStringLiteral("font/family"), font.family());
         settings.setValue(QStringLiteral("font/size"), font.pointSize());
         settings.setValue(QStringLiteral("font/weight"), font.family());
         settings.setValue(QStringLiteral("font/italic"), font.italic());

         searchExpressionEdit_->setFont(font);
         itemView->setFont(font);
      }
   });

   //
   // Show the decorated context menu instead of the standard context menu.
   //
   searchExpressionEdit_->setContextMenuPolicy(Qt::CustomContextMenu);
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::customContextMenuRequested, [this, itemEditContextMenu](const QPoint& position){
      itemEditContextMenu->exec(searchExpressionEdit_->mapToGlobal(position));
   });

   //
   // Set layout.
   //
   auto itemLayout = new QVBoxLayout;
   itemLayout->addWidget(searchExpressionEdit_);
   itemLayout->addWidget(itemView);
   itemLayout->setSpacing(0);

   setLayout(itemLayout);

   //
   // Create the hotkey.
   //
   auto itemHotkey = new SystemHotkey(this);
   itemHotkey->connect(itemHotkey, &SystemHotkey::hotkeyPressed, [this](){
      //
      // Show the window.
      //
      showNormal();
   });

   //
   // Register the hotkey sequence.
   //
   if (itemHotkey->registerKeySequence() == false)
   {
      searchExpressionEdit_->addInidication(QStringLiteral("hotkeyError"), tr("The hotkey could not be registered"));
   }

   //
   // Register the widget with the geomerty store.
   //
   auto screenGeometry = QApplication::desktop()->screenGeometry();
   QSize defaultSize(screenGeometry.width() * 0.3, screenGeometry.height() * 0.8);
   QPoint defaultPosition(screenGeometry.right() - defaultSize.width(), 0);

   static_cast<Application*>(Application::instance())->geometryStore().addWidget(this, QRect(defaultPosition, defaultSize));

   //
   // Restore the font or use the default font is no font is stored.
   //
   auto& settings = static_cast<Application*>(Application::instance())->settings();
   auto defaultFont = QApplication::font();
   auto font = QFont(settings.value(QStringLiteral("font/family"), defaultFont.family()).value<QString>(),
                     settings.value(QStringLiteral("font/size"), defaultFont.pointSize()).value<int>(),
                     settings.value(QStringLiteral("font/weight"), defaultFont.weight()).value<int>(),
                     settings.value(QStringLiteral("font/italic"), defaultFont.italic()).value<bool>());

   searchExpressionEdit_->setFont(font);
   itemView->setFont(font);

   //
   // Update the source model. Be aware that this must be done after creating the UI, so that
   // possible errors during the model update are properly indicated.
   //
//   itemModel->read(settings.value("sourceFile", QStringLiteral("launcher.xml")).toString());
}

SearchWindow::~SearchWindow()
{
}

bool SearchWindow::eventFilter(QObject* object, QEvent* event)
{
   Q_ASSERT(event != nullptr);
   Q_ASSERT(object->isWidgetType());

   //
   // If \a true, the event shall be consumed (not passed to the object).
   //
   bool consumeEvent = false;

   if (auto keyPressEvent = eventAs<QKeyEvent>(event, QEvent::KeyPress))
   {
      //
      // If the modifier is pressed the window modification operation may begin.
      //
      if ((keyPressEvent->modifiers() & Qt::ShiftModifier) != Qt::NoModifier)
      {
         windowModificationModifierActive_ = true;
      }
   }
   else if (auto keyReleaseEvent = eventAs<QKeyEvent>(event, QEvent::KeyRelease))
   {
      //
      // If the modifier is released the window modification operation must end.
      //
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
      //
      // If the mouse button is pressed and the modifier is active the window modification
      // operation begins.
      //
      if (windowModificationModifierActive_ == true)
      {
         windowModificationOperationActive_ = true;

         consumeEvent = true;
      }
   }
   else if (/* auto mouseButtonReleasedEvent = */ eventAs<QMouseEvent>(event, QEvent::MouseButtonRelease))
   {
      //
      // If the mouse button is released the window modification operation ends.
      //
      if (windowModificationModifierActive_ == true)
      {
         windowModificationOperationActive_ = false;

         consumeEvent = true;
      }
   }
   else if (auto mouseMovedEvent = eventAs<QMouseEvent>(event, QEvent::MouseMove))
   {
      //
      // If the mouse cursor is moved and the window modification operation is active perform
      // the operation by modifiying the window geometry, either position or size, relative to
      // the position of the cursor when the operation was started, the origin. If the window
      // modification operation is not active just updated the origin, so the proper operation
      // can be determined outside of this event.
      //
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
      //
      // If the modifier is pressed determine the window modification operation, which is either
      // resize if the cursor is positioned near the left edge of the widget or move otherwise.
      //
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

bool SearchWindow::openUrl_(const QUrl& url)
{
   qInfo() << "open url:" << url.toString();

   //
   // Open the URL with the registered default application.
   //
   bool result = QDesktopServices::openUrl(url);
   if (result == false)
   {
      searchExpressionEdit_->addInidication(QStringLiteral("openUrlError"), tr("Failed to open URL ").append(url.toString()));
   }

   return result;
}
