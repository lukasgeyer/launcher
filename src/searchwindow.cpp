/*!
 * \file searchwindow.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <limits>

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFontDialog>
#include <QGraphicsDropShadowEffect>
#include <QHeaderView>
#include <QIcon>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QTableView>
#include <QUrl>
#include <QVBoxLayout>

#include "application.h"
#include "event.h"
#include "itemedit.h"
#include "itemeditdialog.h"
#include "itemfiltermodel.h"
#include "itemmodel.h"
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
   setAttribute(Qt::WA_TranslucentBackground, true);

   //
   // Set the window properties.
   //
   setWindowIcon(QIcon(QStringLiteral(":/images/logo.png")));
   setWindowOpacity(0.95);

   //
   // Create UI.
   //
   auto itemModel = new ItemModel(this);

   auto linkItemProxyModel = new LinkItemProxyModel(this);
   linkItemProxyModel->setSourceModel(itemModel);

   auto linkItemFilterModel = new LinkItemFilterModel(this);
   linkItemFilterModel->setSourceModel(linkItemProxyModel);
   linkItemFilterModel->sort(0);

   auto linkItemView = new QTableView(this);
   linkItemView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   linkItemView->horizontalHeader()->hide();
   linkItemView->verticalHeader()->hide();
   linkItemView->setFocusPolicy(Qt::NoFocus);
   linkItemView->setModel(linkItemFilterModel);
   linkItemView->setSelectionBehavior(QAbstractItemView::SelectRows);
   linkItemView->setShowGrid(false);
   linkItemView->setStyleSheet(QStringLiteral("QTableView { background-color: transparent; border: none; margin-left: 2px; margin-right: 2px; } "
                                          "QTableView::item { color: #ffe2e2e2; background-color: #d018435a; padding: 4px; } "
                                          "QTableView::item:selected { color: #ffe2e2e2; background-color: #ff18435a; padding: 4px; }"));

   auto searchExpressionEditShadowEffect = new QGraphicsDropShadowEffect;
   searchExpressionEditShadowEffect->setBlurRadius(16.0);
   searchExpressionEditShadowEffect->setOffset(0.0);

   searchExpressionEdit_ = new ItemEdit(this);
   searchExpressionEdit_->installEventFilter(this);
   searchExpressionEdit_->setFocus();
   searchExpressionEdit_->setFocusPolicy(Qt::StrongFocus);
   searchExpressionEdit_->setGraphicsEffect(searchExpressionEditShadowEffect);
   searchExpressionEdit_->setStyleSheet(QStringLiteral("QLineEdit { border: none; padding: 4px; }"));
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::textChanged, [this, linkItemFilterModel, linkItemView](const QString& text){
      linkItemFilterModel->setSearchExpression(text);
      linkItemView->resizeRowsToContents();
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::returnPressed, [this, linkItemFilterModel, linkItemView](){
      const auto& currentIndex = linkItemView->currentIndex();
      if (currentIndex.isValid())
      {
         //
         // If enter is pressed and an item is selected open the link and hide if the link
         // could be opened. In addition, the last URL open error is cleared. Remain shown
         // otherwise so the error can be seen.
         //
         if (openItem_(linkItemFilterModel->item(currentIndex), linkItemFilterModel->searchExpression().parameters()))
         {
            hide();
         }
      }
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::keyUpPressed, [linkItemView](){
      const auto& currentIndex = linkItemView->currentIndex();
      linkItemView->setCurrentIndex(linkItemView->model()->index(qMax(-1, currentIndex.row() - 1), 0));
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::keyDownPressed, [linkItemView](){
      const auto& currentIndex = linkItemView->currentIndex();
      linkItemView->setCurrentIndex(currentIndex.isValid() ?
                                    linkItemView->model()->index(qMin(linkItemView->model()->rowCount() - 1, currentIndex.row() + 10), 0) :
                                    linkItemView->model()->index(0, 0));
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::keyPageUpPressed, [linkItemView](){
      const auto& currentIndex = linkItemView->currentIndex();
      linkItemView->setCurrentIndex(linkItemView->model()->index(qMax(-1, currentIndex.row() - 5), 0));
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::keyPageDownPressed, [linkItemView](){
      const auto& currentIndex = linkItemView->currentIndex();
      linkItemView->setCurrentIndex(currentIndex.isValid() ?
                                    linkItemView->model()->index(qMin(linkItemView->model()->rowCount() - 1, currentIndex.row() + 10), 0) :
                                    linkItemView->model()->index(0, 0));
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::escapePressed, [this](){
      //
      // If escaped is pressed hide.
      //
      hide();
   });

   linkItemView->connect(linkItemView, &QTableView::clicked, [this, linkItemFilterModel](const QModelIndex& index)
   {
      //
      // If an item is clicked open the link and remain shown (so multiple items can be clicked).
      //
      openItem_(linkItemFilterModel->item(index), linkItemFilterModel->searchExpression().parameters());
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
   itemEditContextMenu->addAction(QIcon(QStringLiteral(":/images/edit.png")), tr("Edit items..."), [itemModel]()
   {
      ItemEditDialog itemEditDialog(itemModel);
      itemEditDialog.setModal(true);
      itemEditDialog.exec();
   });
   itemEditContextMenu->addSeparator();
   itemEditContextMenu->addAction(QIcon(QStringLiteral(":/images/font.png")), tr("Select font..."), [this, linkItemView](){
      auto settings = static_cast<Application*>(Application::instance())->settings();

      auto fontSelected = false;
      auto font = QFontDialog::getFont(&fontSelected, QFont(settings->value(QStringLiteral("font/family")).value<QString>(),
                                                            settings->value(QStringLiteral("font/size")).value<int>(),
                                                            settings->value(QStringLiteral("font/weight")).value<int>(),
                                                            settings->value(QStringLiteral("font/italic")).value<bool>()), searchExpressionEdit_);
      if (fontSelected )
      {
         settings->setValue(QStringLiteral("font/family"), font.family());
         settings->setValue(QStringLiteral("font/size"), font.pointSize());
         settings->setValue(QStringLiteral("font/weight"), font.family());
         settings->setValue(QStringLiteral("font/italic"), font.italic());

         searchExpressionEdit_->setFont(font);
         linkItemView->setFont(font);
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
   itemLayout->addWidget(linkItemView);
   itemLayout->addStretch();
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
   if (!itemHotkey->registerKeySequence())
   {
      searchExpressionEdit_->addInidication(QStringLiteral("hotkeyError"), tr("The hotkey could not be registered"));
   }

   //
   // Register the widget with the geomerty store.
   //
   auto screenGeometry = QApplication::desktop()->screenGeometry();

   QSize defaultSize(screenGeometry.width() * 0.3, screenGeometry.height() * 0.8);
   QPoint defaultPosition(screenGeometry.right() - defaultSize.width(), 0);

   static_cast<Application*>(Application::instance())->geometryStore()->addWidget(this, QRect(defaultPosition, defaultSize));

   //
   // Restore the font or use the default font is no font is stored.
   //
   auto settings = static_cast<Application*>(Application::instance())->settings();
   auto defaultFont = QApplication::font();
   auto font = QFont(settings->value(QStringLiteral("font/family"), defaultFont.family()).value<QString>(),
                     settings->value(QStringLiteral("font/size"), defaultFont.pointSize()).value<int>(),
                     settings->value(QStringLiteral("font/weight"), defaultFont.weight()).value<int>(),
                     settings->value(QStringLiteral("font/italic"), defaultFont.italic()).value<bool>());

   searchExpressionEdit_->setFont(font);
   linkItemView->setFont(font);

   //
   // Update the source model. Be aware that this must be done after creating the UI, so that
   // possible errors during the model update are properly indicated.
   //
   itemModel->read(settings->value("sourceFile", QStringLiteral("launcher.xml")).toString());
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
      if (windowModificationModifierActive_ )
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
      if (windowModificationModifierActive_ )
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
      if (windowModificationOperationActive_ )
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

   if (windowModificationModifierActive_ )
   {
      //
      // If the modifier is pressed determine the window modification operation, which is either
      // resize if the cursor is positioned near the left edge of the widget or move otherwise.
      //
      auto widget = static_cast<QWidget*>(object);
      auto widgetPosition = widget->mapToGlobal(widget->pos());

      if (widget->underMouse())
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

bool SearchWindow::openItem_(LinkItem* item, const QStringList& parameters)
{
   bool result = false;

   //
   // Replace parameter within link.
   //
   QString link = item->link();

   int parameterNumber = 0;
   for (; parameterNumber < std::numeric_limits<int>::max(); ++parameterNumber)
   {
      const QString& parameterPlaceholder = (QStringLiteral("{") + QString::number(parameterNumber + 1) + QStringLiteral("}"));
      if (link.indexOf(parameterPlaceholder) >= 0)
      {
         if (parameterNumber < parameters.size())
         {
            link.replace(parameterPlaceholder, parameters.at(parameterNumber));
         }
      }
      else
      {
         break;
      }
   }

   if (parameterNumber == parameters.size())
   {
      searchExpressionEdit_->removeIndication(QStringLiteral("parameterError"));

      //
      // Open the link with the registered default application.
      //
      result = QDesktopServices::openUrl(QUrl::fromUserInput(link));
      if (!result)
      {
         searchExpressionEdit_->addInidication(QStringLiteral("openUrlError"), tr("Failed to open URL ").append(item->link()));
      }
   }
   else if (parameterNumber > parameters.size())
   {
      searchExpressionEdit_->addInidication(QStringLiteral("parameterError"), tr("Insufficient parameter provided"));
   }
   else if (parameterNumber < parameters.size())
   {
      searchExpressionEdit_->addInidication(QStringLiteral("parameterError"), tr("Extraneous parameter provided"));
   }

   return result;
}
