/*!
 * \file itemhotkey.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef ITEMHOTKEY_H
#define ITEMHOTKEY_H

#include <QAbstractNativeEventFilter>
#include <QObject>

/*!
 * \brief A system-wide hotkey.
 */
class ItemHotkey : public QObject, public QAbstractNativeEventFilter
{
   Q_OBJECT

public:
   /*!
    * Constructs an ItemHotkey with the parent \a parent.
    */
   ItemHotkey(QObject* parent = nullptr);
   /*!
    * Destructs an ItemHotkey.
    */
   ~ItemHotkey();

   /*!
    * Registers the predefined key sequence.
    */
   bool registerKeySequence();
   /*!
    * Unregisters the predefined key sequence.
    */
   void unregisterKeySequence();

   /*!
    * \reimp
    */
   bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

signals:
   /*!
    * Is emitted when the hotkey has been pressed.
    */
   void hotkeyPressed();
};

#endif // ITEMHOTKEY_H
