/*!
 * \file searchwindow.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QApplication>
#include <QAction>
#include <QActionGroup>
#include <QClipboard>
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
#include <QScrollBar>
#include <QTableView>
#include <QUrl>
#include <QVBoxLayout>

#include "application.h"
#include "event.h"
#include "itemmodel.h"
#include "itemsourceeditor.h"
#include "metatype.h"
#include "searchbarwidget.h"
#include "searchitemfiltermodel.h"
#include "searchitemproxymodel.h"
#include "searchresultwidget.h"
#include "searchwindow.h"
#include "systemhotkey.h"

namespace {

/*!
 * The size of the resize area (if the cursor is not within this area the window will be moved
 * instead of resized).
 */
const int RESIZE_AREA_SIZE_ = 16;

} // namespace

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

   searchItemFilterModelImplicitMatch_ = application->setting<SearchExpression::ImplicitMatch>(this, "implicitMatch", SearchExpression::ParameterImplicitMatch);

   auto searchResultWidgetShadowEffect = new QGraphicsDropShadowEffect;
   searchResultWidgetShadowEffect->setColor(QColor(QStringLiteral("#1f3242")));
   searchResultWidgetShadowEffect->setBlurRadius(16.0);
   searchResultWidgetShadowEffect->setOffset(0.0);

   searchResultWidget_ = new SearchResultWidget(this);
   searchResultWidget_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
   searchResultWidget_->horizontalHeader()->hide();
   searchResultWidget_->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
   searchResultWidget_->verticalHeader()->hide();
   searchResultWidget_->setContextMenuPolicy(Qt::CustomContextMenu);
   searchResultWidget_->setGraphicsEffect(searchResultWidgetShadowEffect);
   searchResultWidget_->setFocusPolicy(Qt::NoFocus);
   searchResultWidget_->setModel(searchItemFilterModel_);
   searchResultWidget_->setSelectionBehavior(QAbstractItemView::SelectRows);
   searchResultWidget_->setShowGrid(false);
   searchResultWidget_->setStyleSheet(QStringLiteral("SearchResultWidget { border: none; margin-left: 2px; margin-right: 2px; } "
                                                     "SearchResultWidget::item { border: none; padding-left: 2px; padding-right: 2px; } "
                                                     "SearchResultWidget::item:selected { background-color: #7ba8ce; } "));
   searchResultWidget_->connect(searchResultWidget_, &QTableView::customContextMenuRequested, [this](const QPoint& position)
   {
      //
      // Show the item-specific context menu if a valid item has been selected.
      //

      if (auto item = searchItemFilterModel_->item(searchResultWidget_->indexAt(position)))
      {
         if (auto itemSource = item->parent<ItemSource>())
         {
            QMenu searchResultViewContextMenu;
            searchResultViewContextMenu.addAction(QIcon(QStringLiteral(":/images/edit.png")), tr("Edit item..."), [this, item, itemSource]()
            {
               openSource_(itemSource->identifier(), item->linkPosition());
            });
            searchResultViewContextMenu.addAction(QIcon(QStringLiteral(":/images/clipboard.png")), tr("Copy URL to clipboard..."), [this, item, itemSource]()
            {
               QApplication::clipboard()->setText(item->link(searchItemFilterModel_->searchExpression().parameters()));
            });
            searchResultViewContextMenu.exec(searchResultWidget_->mapToGlobal(position));
         }
      }
   });

   auto searchBarWidgetShadowEffect = new QGraphicsDropShadowEffect;
   searchBarWidgetShadowEffect->setColor(QColor(QStringLiteral("#335675")));
   searchBarWidgetShadowEffect->setBlurRadius(16.0);
   searchBarWidgetShadowEffect->setOffset(0.0);

   searchBarWidget_ = new SearchBarWidget(this);
   searchBarWidget_->installEventFilter(this);
   searchBarWidget_->setFocus();
   searchBarWidget_->setFocusPolicy(Qt::StrongFocus);
   searchBarWidget_->setGraphicsEffect(searchBarWidgetShadowEffect);
   searchBarWidget_->setStyleSheet(QStringLiteral("QLineEdit { border: none; padding: 4px; }"));
   searchBarWidget_->connect(searchBarWidget_, &SearchBarWidget::textChanged, [this](const QString& text)
   {
      searchItemFilterModel_->setSearchExpression(text, searchItemFilterModelImplicitMatch_);
   });
   searchBarWidget_->connect(searchBarWidget_, &SearchBarWidget::returnPressed, [this](){
      const auto& currentIndex = searchResultWidget_->currentIndex();
      if (currentIndex.isValid())
      {
         //
         // If enter is pressed and an item is selected open the link and hide if the link
         // could be opened. Remain shown otherwise so the error can be seen.
         //

         if (openUrl_(searchItemFilterModel_->item(currentIndex), searchItemFilterModel_->searchExpression().parameters()))
         {
            searchBarWidget_->removeIndication(QStringLiteral("openUrlError:*"));

            hide();
         }
      }
      else if (QGuiApplication::queryKeyboardModifiers() & Qt::ShiftModifier)
      {
         //
         // If SHIFT + enter is pressed and no item is selected open all items and hide if at
         // least on link could be opened. Remain shown otherwise so the error can be seen.
         //

         bool openUrlSucceeded = true;

         for (int row = 0; row < searchItemFilterModel_->rowCount(); ++row)
         {
            openUrlSucceeded &= openUrl_(searchItemFilterModel_->item(searchItemFilterModel_->index(row, 0)),
                                         searchItemFilterModel_->searchExpression().parameters());
         }

         if (openUrlSucceeded == true)
         {
            searchBarWidget_->removeIndication(QStringLiteral("openUrlError:*"));

            hide();
         }
      }
   });
   searchBarWidget_->connect(searchBarWidget_, &SearchBarWidget::keyUpPressed, [this](){
      const auto& currentIndex = searchResultWidget_->currentIndex();
      searchResultWidget_->setCurrentIndex(searchResultWidget_->model()->index(qMax(-1, currentIndex.row() - 1), 0));
   });
   searchBarWidget_->connect(searchBarWidget_, &SearchBarWidget::keyDownPressed, [this](){
      const auto& currentIndex = searchResultWidget_->currentIndex();
      searchResultWidget_->setCurrentIndex(currentIndex.isValid() ?
                                    searchResultWidget_->model()->index(qMin(searchResultWidget_->model()->rowCount() - 1, currentIndex.row() + 1), 0) :
                                    searchResultWidget_->model()->index(0, 0));
   });
   searchBarWidget_->connect(searchBarWidget_, &SearchBarWidget::keyPageUpPressed, [this](){
      const auto& currentIndex = searchResultWidget_->currentIndex();
      searchResultWidget_->setCurrentIndex(searchResultWidget_->model()->index(qMax(-1, currentIndex.row() - 5), 0));
   });
   searchBarWidget_->connect(searchBarWidget_, &SearchBarWidget::keyPageDownPressed, [this](){
      const auto& currentIndex = searchResultWidget_->currentIndex();
      searchResultWidget_->setCurrentIndex(currentIndex.isValid() ?
                                    searchResultWidget_->model()->index(qMin(searchResultWidget_->model()->rowCount() - 1, currentIndex.row() + 10), 0) :
                                    searchResultWidget_->model()->index(0, 0));
   });
   searchBarWidget_->connect(searchBarWidget_, &SearchBarWidget::escapePressed, [this](){
      //
      // If escaped is pressed hide.
      //

      hide();
   });
   searchBarWidget_->connect(itemModel_, &ItemModel::sourceFailedToLoad,
                                  [this](const QString& source, const QString& errorString)
   {
      searchBarWidget_->addInidication(QStringLiteral("sourceError:").append(source),
                                            tr("Source failed to load: %1: %2").arg(source).arg(errorString));
   });
   searchBarWidget_->connect(itemModel_, &ItemModel::sourceLoaded, [this](const QString& source)
   {
      searchBarWidget_->removeIndication(QStringLiteral("sourceError:").append(source));
   });
   searchBarWidget_->connect(itemModel_, &ItemModel::modelReset, [this, application]()
   {
      searchBarWidget_->removeIndication(QStringLiteral("sourceError:*"));
   });

   searchResultWidget_->connect(searchResultWidget_, &QTableView::clicked, [this](const QModelIndex& index)
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
   setFocusProxy(searchBarWidget_);

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
         searchBarWidget_->selectAll();

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

   auto searchExpressionEditContextMenu = searchBarWidget_->createStandardContextMenu();
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

   auto implicitMatchMenu = searchExpressionEditContextMenu->addMenu(QIcon(QStringLiteral(":/images/expression.png")), tr("Match additional word as..."));
   auto implicitMatchMenuNameAction = implicitMatchMenu->addAction(QIcon(QStringLiteral(":/images/name.png")), tr("Term"), [this, application]()
   {
      application->setSetting(this, "implicitMatch", Enum::toString(SearchExpression::TermImplicitMatch));

      searchItemFilterModelImplicitMatch_ = SearchExpression::TermImplicitMatch;
      searchItemFilterModel_->setSearchExpression(searchBarWidget_->text(), searchItemFilterModelImplicitMatch_);
   });
   implicitMatchMenuNameAction->setCheckable(true);
   implicitMatchMenuNameAction->setChecked(searchItemFilterModelImplicitMatch_ == SearchExpression::TermImplicitMatch);
   auto implicitMatchMenuParameterAction = implicitMatchMenu->addAction(QIcon(QStringLiteral(":/images/parameter.png")), tr("Parameter"), [this, application]()
   {
      application->setSetting(this, "implicitMatch", Enum::toString(SearchExpression::ParameterImplicitMatch));

      searchItemFilterModelImplicitMatch_ = SearchExpression::ParameterImplicitMatch;
      searchItemFilterModel_->setSearchExpression(searchBarWidget_->text(), searchItemFilterModelImplicitMatch_);
   });
   implicitMatchMenuParameterAction->setCheckable(true);
   implicitMatchMenuParameterAction->setChecked(searchItemFilterModelImplicitMatch_ == SearchExpression::ParameterImplicitMatch);

   auto implicitMatchMenuActionGroup = new QActionGroup(implicitMatchMenu);
   implicitMatchMenuActionGroup->setExclusive(true);
   implicitMatchMenuActionGroup->addAction(implicitMatchMenuNameAction);
   implicitMatchMenuActionGroup->addAction(implicitMatchMenuParameterAction);

   searchExpressionEditContextMenu->addSeparator();

   searchExpressionEditContextMenu->addAction(QIcon(QStringLiteral(":/images/font.png")), tr("Select font..."), [this, application]()
   {
      auto fontSelected = false;
      auto font = QFontDialog::getFont(&fontSelected, application->setting<QFont>(this, QStringLiteral("font")), this);
      if (fontSelected)
      {
         application->setSetting(this, QStringLiteral("font"), font);

         searchBarWidget_->setFont(application->setting<QFont>(this, QStringLiteral("font"), application->font()));
         searchResultWidget_->setFont(application->setting<QFont>(this, QStringLiteral("font"), application->font()));
      }
   });

   //
   // Show the decorated context menu instead of the standard context menu.
   //

   searchBarWidget_->setContextMenuPolicy(Qt::CustomContextMenu);
   searchBarWidget_->connect(searchBarWidget_, &SearchBarWidget::customContextMenuRequested, [this, searchExpressionEditContextMenu](const QPoint& position)
   {
      searchExpressionEditContextMenu->exec(searchBarWidget_->mapToGlobal(position));
   });

   //
   // Set layout.
   //

   auto layout = new QVBoxLayout;
   layout->addWidget(searchBarWidget_);
   layout->addWidget(searchResultWidget_);
   layout->setSpacing(0);

   setLayout(layout);

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
      searchBarWidget_->addInidication(QStringLiteral("hotkeyError"), tr("The hotkey could not be registered"));
   }

   //
   // Update the widget geometry.
   //

   auto screenGeometry = QApplication::desktop()->screenGeometry();

   QSize defaultSize(screenGeometry.width() * 0.3, screenGeometry.height() * 0.8);
   QPoint defaultPosition(screenGeometry.right() - defaultSize.width(), 0);

   application->updateGeometry(this, QRect(defaultPosition, defaultSize));

   //
   // Restore the font or use the default font is no font is stored.
   //

   searchBarWidget_->setFont(application->setting<QFont>(this, QStringLiteral("font"), application->font()));
   searchResultWidget_->setFont(application->setting<QFont>(this, QStringLiteral("font"), application->font()));
}

SearchWindow::~SearchWindow()
{
}

bool SearchWindow::event(QEvent* event)
{
   if (/* auto layoutRequestEvent = */ Event::cast<QEvent>(event, QEvent::LayoutRequest))
   {
      resize(sizeHint());
   }

   return QWidget::event(event);
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

         setMaximumHeight((QApplication::desktop()->screenGeometry().height() * 0.95) - pos().y());

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

         setMaximumHeight((QApplication::desktop()->screenGeometry().height() * 0.95) - pos().y());

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

      searchBarWidget_->addInidication(QStringLiteral("openSourceError:").append(source), tr("Failed to open source ").append(source));
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
         searchBarWidget_->addInidication(QStringLiteral("openUrlError:").append(url.toString()), tr("Failed to open URL ").append(url.toString()));
      }

      break;
   }
   case LinkItem::LinkMatch::InsufficientParameters:
   {
      qInfo() << "item failed to open due to insufficient parameters" << item->link() << parameters;

      searchBarWidget_->addInidication(QStringLiteral("parameterError").append(item->link()), tr("Insufficient parameters provided"));

      break;
   }
   case LinkItem::LinkMatch::ExcessParameters:
   {
      qInfo() << "item failed to open due to excess parameters" << item->link() << parameters;

      searchBarWidget_->addInidication(QStringLiteral("parameterError").append(item->link()), tr("Excess parameters provided"));

      break;
   }
   }

   return result;
}
