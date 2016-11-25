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
#include <QAction>
#include <QActionGroup>
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
#include "itemmodel.h"
#include "itemsourceeditor.h"
#include "metatype.h"
#include "searchitemfiltermodel.h"
#include "searchitemproxymodel.h"
#include "searchwindow.h"
#include "systemhotkey.h"

namespace {

/*!
 * The size of the resize area (if the cursor is not within this area the window will be moved
 * instead of resized).
 */
const int RESIZE_AREA_SIZE_ = 16;

} // namespace

#include <QStandardItemModel>

SearchWindow::SearchWindow(ItemModel* itemModel, QWidget *parent) : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowSystemMenuHint), itemModel_(itemModel)
{
   Q_ASSERT(itemModel_);

   Application* application = static_cast<Application*>(Application::instance());

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

   searchItemProxyModel_ = new SearchItemProxyModel(this);
   searchItemProxyModel_->setSourceModel(itemModel_);

   auto sortAlgorithm = application->setting<SearchItemFilterModel::SortAlgorithm>(this, "sortAlgorithm", SearchItemFilterModel::TagSortAlgorithm);

   searchItemFilterModel_ = new SearchItemFilterModel(this);
   searchItemFilterModel_->setSourceModel(searchItemProxyModel_);
   searchItemFilterModel_->sort(sortAlgorithm);

   searchResultView_ = new QTableView(this);
   searchResultView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   searchResultView_->horizontalHeader()->hide();
   searchResultView_->verticalHeader()->hide();
   searchResultView_->setContextMenuPolicy(Qt::CustomContextMenu);
   searchResultView_->setFocusPolicy(Qt::NoFocus);
   searchResultView_->setModel(searchItemFilterModel_);
   searchResultView_->setSelectionBehavior(QAbstractItemView::SelectRows);
   searchResultView_->setShowGrid(false);
   searchResultView_->setStyleSheet(QStringLiteral("QTableView { background-color: transparent; border: none; margin-left: 2px; margin-right: 2px; } "
                                                   "QTableView::item { background-color: #ffffffff; padding: 4px; } "
                                                   "QTableView::item:selected { background-color: #ff5bc0de; padding: 4px; }"));
   searchResultView_->connect(searchResultView_, &QTableView::customContextMenuRequested, [this](const QPoint& position)
   {
      //
      // Show the item-specific context menu if a valid item has been selected.
      //

      if (auto item = searchItemFilterModel_->item(searchResultView_->indexAt(position)))
      {
         if (auto itemSource = item->parent<ItemSource>())
         {
            QMenu searchResultViewContextMenu;
            searchResultViewContextMenu.addAction(QIcon(QStringLiteral(":/images/edit.png")), tr("Edit item..."), [this, item, itemSource]()
            {
               openSource_(itemSource->identifier(), item->linkPosition());
            });
            searchResultViewContextMenu.exec(searchResultView_->mapToGlobal(position));
         }
      }
   });

   auto searchExpressionEditShadowEffect = new QGraphicsDropShadowEffect;
   searchExpressionEditShadowEffect->setColor(QColor(QStringLiteral("#428bca")));
   searchExpressionEditShadowEffect->setBlurRadius(24.0);
   searchExpressionEditShadowEffect->setOffset(0.0);

   searchExpressionEdit_ = new ItemEdit(this);
   searchExpressionEdit_->installEventFilter(this);
   searchExpressionEdit_->setFocus();
   searchExpressionEdit_->setFocusPolicy(Qt::StrongFocus);
   searchExpressionEdit_->setGraphicsEffect(searchExpressionEditShadowEffect);
   searchExpressionEdit_->setStyleSheet(QStringLiteral("QLineEdit { border: none; padding: 4px; }"));
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::textChanged, [this](const QString& text)
   {
      searchItemFilterModel_->setSearchExpression(text);
      searchResultView_->resizeRowsToContents();
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::returnPressed, [this](){
      const auto& currentIndex = searchResultView_->currentIndex();
      if (currentIndex.isValid())
      {
         //
         // If enter is pressed and an item is selected open the link and hide if the link
         // could be opened. Remain shown otherwise so the error can be seen.
         //

         if (openUrl_(searchItemFilterModel_->item(currentIndex), searchItemFilterModel_->searchExpression().parameters()))
         {
            searchExpressionEdit_->removeIndication(QStringLiteral("openUrlError:*"));

            hide();
         }
      }
      else
      {
         //
         // If enter is pressed and no item is selected open all items and hide if at least
         // on link could be opened. Remain shown otherwise so the error can be seen.
         //

         bool openUrlSucceeded = true;

         for (int row = 0; row < searchItemFilterModel_->rowCount(); ++row)
         {
            openUrlSucceeded &= openUrl_(searchItemFilterModel_->item(searchItemFilterModel_->index(row, 0)),
                                         searchItemFilterModel_->searchExpression().parameters());
         }

         if (openUrlSucceeded == true)
         {
            searchExpressionEdit_->removeIndication(QStringLiteral("openUrlError:*"));

            hide();
         }
      }
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::keyUpPressed, [this](){
      const auto& currentIndex = searchResultView_->currentIndex();
      searchResultView_->setCurrentIndex(searchResultView_->model()->index(qMax(-1, currentIndex.row() - 1), 0));
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::keyDownPressed, [this](){
      const auto& currentIndex = searchResultView_->currentIndex();
      searchResultView_->setCurrentIndex(currentIndex.isValid() ?
                                    searchResultView_->model()->index(qMin(searchResultView_->model()->rowCount() - 1, currentIndex.row() + 1), 0) :
                                    searchResultView_->model()->index(0, 0));
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::keyPageUpPressed, [this](){
      const auto& currentIndex = searchResultView_->currentIndex();
      searchResultView_->setCurrentIndex(searchResultView_->model()->index(qMax(-1, currentIndex.row() - 5), 0));
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::keyPageDownPressed, [this](){
      const auto& currentIndex = searchResultView_->currentIndex();
      searchResultView_->setCurrentIndex(currentIndex.isValid() ?
                                    searchResultView_->model()->index(qMin(searchResultView_->model()->rowCount() - 1, currentIndex.row() + 10), 0) :
                                    searchResultView_->model()->index(0, 0));
   });
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::escapePressed, [this](){
      //
      // If escaped is pressed hide.
      //

      hide();
   });
   searchExpressionEdit_->connect(itemModel_, &ItemModel::sourceFailedToLoad,
                                  [this](const QString& source, const QString& errorString)
   {
      searchExpressionEdit_->addInidication(QStringLiteral("sourceError:").append(source),
                                            tr("Source failed to load: %1: %2").arg(source).arg(errorString));
   });
   searchExpressionEdit_->connect(itemModel_, &ItemModel::sourceLoaded, [this](const QString& source)
   {
      searchExpressionEdit_->removeIndication(QStringLiteral("sourceError:").append(source));
   });
   searchExpressionEdit_->connect(itemModel_, &ItemModel::modelReset, [this, application]()
   {
      searchExpressionEdit_->removeIndication(QStringLiteral("sourceError:*"));
   });

   searchResultView_->connect(searchResultView_, &QTableView::clicked, [this](const QModelIndex& index)
   {
      //
      // If an item is clicked open the link and remain shown (so multiple items can be clicked).
      //

      openUrl_(searchItemFilterModel_->item(index), searchItemFilterModel_->searchExpression().parameters());
   });

   //
   // Set focus policy so that the search expression window remains focused.
   //

   setFocusPolicy(Qt::NoFocus);
   setFocusProxy(searchExpressionEdit_);

   //
   // Hide the application if it loses focus, as it cannot be activated programatically for
   // instance on Windows if another application is activated. The application will simply
   // flash in the taskbar instead. Another option would be hiding and showing the window,
   // but this leads to unpleasent flickering, so the application is simply hidden.
   //

   connect(qApp, &QApplication::applicationStateChanged, [this](Qt::ApplicationState applicationState)
   {
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

   auto searchExpressionEditContextMenu = searchExpressionEdit_->createStandardContextMenu();
   searchExpressionEditContextMenu->addSeparator();
   searchExpressionEditContextMenu->addAction(QIcon(QStringLiteral(":/images/edit.png")), tr("Edit items..."), [this]()
   {
      openSource_(itemModel_->itemSourceIdentifier());
   });
   searchExpressionEditContextMenu->addAction(QIcon(QStringLiteral(":/images/reload.png")), tr("Reload items..."), [this]()
   {
      itemModel_->read(itemModel_->itemSourceIdentifier());
   });

   searchExpressionEditContextMenu->addSeparator();

   auto sortAlgorithmMenu = searchExpressionEditContextMenu->addMenu(QIcon(QStringLiteral(":/images/order.png")), tr("Sort order"));
   auto sortAlgorithmMenuNameAction = sortAlgorithmMenu->addAction(QIcon(QStringLiteral(":/images/name.png")), tr("Name"), [this, application]()
   {
      application->setSetting(this, "sortAlgorithm", Enum::toString(SearchItemFilterModel::NameSortAlgorithm));

      searchItemFilterModel_->sort(SearchItemFilterModel::NameSortAlgorithm);
   });
   sortAlgorithmMenuNameAction->setCheckable(true);
   sortAlgorithmMenuNameAction->setChecked(sortAlgorithm == SearchItemFilterModel::NameSortAlgorithm);
   auto sortAlgorithmMenuTagAction = sortAlgorithmMenu->addAction(QIcon(QStringLiteral(":/images/tag.png")), tr("Tag"), [this, application]()
   {
      application->setSetting(this, "sortAlgorithm", Enum::toString(SearchItemFilterModel::TagSortAlgorithm));

      searchItemFilterModel_->sort(SearchItemFilterModel::TagSortAlgorithm);
   });
   sortAlgorithmMenuTagAction->setCheckable(true);
   sortAlgorithmMenuTagAction->setChecked(sortAlgorithm == SearchItemFilterModel::TagSortAlgorithm);

   auto sortAlgorithmMenuActionGroup = new QActionGroup(sortAlgorithmMenu);
   sortAlgorithmMenuActionGroup->setExclusive(true);
   sortAlgorithmMenuActionGroup->addAction(sortAlgorithmMenuNameAction);
   sortAlgorithmMenuActionGroup->addAction(sortAlgorithmMenuTagAction);

   searchExpressionEditContextMenu->addSeparator();

   searchExpressionEditContextMenu->addAction(QIcon(QStringLiteral(":/images/font.png")), tr("Select font..."), [this, application]()
   {
      auto fontSelected = false;
      auto font = QFontDialog::getFont(&fontSelected, application->setting<QFont>(this, QStringLiteral("font")), this);
      if (fontSelected)
      {
         application->setSetting(this, QStringLiteral("font"), font);

         searchExpressionEdit_->setFont(application->setting<QFont>(this, QStringLiteral("font"), application->font()));
         searchResultView_->setFont(application->setting<QFont>(this, QStringLiteral("font"), application->font()));
      }
   });

   //
   // Show the decorated context menu instead of the standard context menu.
   //

   searchExpressionEdit_->setContextMenuPolicy(Qt::CustomContextMenu);
   searchExpressionEdit_->connect(searchExpressionEdit_, &ItemEdit::customContextMenuRequested, [this, searchExpressionEditContextMenu](const QPoint& position)
   {
      searchExpressionEditContextMenu->exec(searchExpressionEdit_->mapToGlobal(position));
   });

   //
   // Set layout.
   //

   auto itemLayout = new QVBoxLayout;
   itemLayout->addWidget(searchExpressionEdit_);
   itemLayout->addWidget(searchResultView_);
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

   application->updateGeometry(this, QRect(defaultPosition, defaultSize));

   //
   // Restore the font or use the default font is no font is stored.
   //

   searchExpressionEdit_->setFont(application->setting<QFont>(this, QStringLiteral("font"), application->font()));
   searchResultView_->setFont(application->setting<QFont>(this, QStringLiteral("font"), application->font()));
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

   if (auto keyPressEvent = Event::cast<QKeyEvent>(event, QEvent::KeyPress))
   {
      //
      // If the modifier is pressed the window modification operation may begin.
      //

      if ((keyPressEvent->modifiers() & Qt::ShiftModifier) != Qt::NoModifier)
      {
         windowModificationModifierActive_ = true;
      }
   }
   else if (auto keyReleaseEvent = Event::cast<QKeyEvent>(event, QEvent::KeyRelease))
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
   else if (/* auto mouseButtonPressedEvent = */ Event::cast<QMouseEvent>(event, QEvent::MouseButtonPress))
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
   else if (/* auto mouseButtonReleasedEvent = */ Event::cast<QMouseEvent>(event, QEvent::MouseButtonRelease))
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
   else if (auto mouseMovedEvent = Event::cast<QMouseEvent>(event, QEvent::MouseMove))
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

bool SearchWindow::openSource_(const QString& source, const ItemSourcePosition& position)
{
   qInfo() << "open source" << source << position.lineNumber() << position.columnNumber() << position.size();

   //
   // Open an editor for the source.
   //

   auto itemSourceEditor = new ItemSourceEditor(this);
   bool result = itemSourceEditor->openSource(new QFile(source, this));
   if (result == true)
   {
      itemSourceEditor->setAttribute(Qt::WA_DeleteOnClose);
      itemSourceEditor->setWindowModality(Qt::ApplicationModal);

      itemSourceEditor->selectSource(position.lineNumber(), position.columnNumber(), position.size());

      itemSourceEditor->show();
      itemSourceEditor->activateWindow();
   }
   else
   {
      itemSourceEditor->deleteLater();

      searchExpressionEdit_->addInidication(QStringLiteral("openSourceError:").append(source), tr("Failed to open source ").append(source));
   }

   return result;
}

bool SearchWindow::openUrl_(LinkItem* item, const QStringList& parameters)
{
   bool result = false;

   switch (item->linkMatches(parameters))
   {
   case LinkItem::LinkMatch::SufficientParameters:
   {
      auto url = QUrl::fromUserInput(item->link(parameters));

      qInfo() << "open url" << url;

      result = QDesktopServices::openUrl(url);
      if (!result)
      {
         searchExpressionEdit_->addInidication(QStringLiteral("openUrlError:").append(url.toString()), tr("Failed to open URL ").append(url.toString()));
      }

      break;
   }
   case LinkItem::LinkMatch::InsufficientParameters:
   {
      qInfo() << "item failed to open due to insufficient parameters" << item->link() << parameters;

      searchExpressionEdit_->addInidication(QStringLiteral("parameterError").append(item->link()), tr("Insufficient parameters provided"));

      break;
   }
   case LinkItem::LinkMatch::ExcessParameters:
   {
      qInfo() << "item failed to open due to excess parameters" << item->link() << parameters;

      searchExpressionEdit_->addInidication(QStringLiteral("parameterError").append(item->link()), tr("Excess parameters provided"));

      break;
   }
   }

   return result;
}
