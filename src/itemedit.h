/*!
 * \file itemedit.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMEDIT_H
#define ITEMEDIT_H

#include <QAction>
#include <QEvent>
#include <QLineEdit>
#include <QString>
#include <QVector>

#include "indicator.h"

/*!
 * \brief A line edit providing additional up-key, down-key or escape-key pressed signals.
 */
class ItemEdit : public QLineEdit, public Indicator
{
   Q_OBJECT

public:
   /*!
    * Constructs an ItemEdit with the parent \a parent.
    */
   ItemEdit(QWidget* parent = nullptr);

public slots:
   /*!
    * \reimp
    */
   void addInidication(const QString& id, Indication* indication) override;

   /*!
    * \reimp
    */
   void removeIndication(const QString& id) override;
   /*!
    * \reimp
    */
   void removeIndications() override;

signals:
   /*!
    * Is emitted when the up-key is pressed.
    */
   void keyUpPressed();
   /*!
    * Is emitted when the down-key is pressed.
    */
   void keyDownPressed();
   /*!
    * Is emitted when the escape-key is pressed.
    */
   void escapePressed();

   /*!
    * \reimp
    */
   void indicationAdded(const QString& id, const Indication& indication);
   /*!
    * \reimp
    */
   void indicationRemoved(const QString& id, const Indication& indication);

private:
   /*!
    * A list of indications.
    */
   typedef QHash<QString, Indication*> Indications_;
   /*!
    * The list of indications.
    */
   Indications_ indications_;
   /*!
    * The indication action.
    */
   QAction* indicationAction_ = nullptr;

   /*!
    * Updates the indication action.
    */
   void updateIndicationAction_();

   /*!
    * \reimp
    */
   bool event(QEvent* event);
};

#endif // ITEMEDIT_H
