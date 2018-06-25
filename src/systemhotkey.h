/*!
 * \file systemhotkey.h
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#ifndef SYSTEMHOTKEY_H
#define SYSTEMHOTKEY_H

#include <QAbstractNativeEventFilter>
#include <QKeySequence>
#include <QObject>

/*!
 * \brief A system-wide hotkey.
 */
class SystemHotkey : public QObject, public QAbstractNativeEventFilter
{
   Q_OBJECT

public:
   /*!
    * Constructs an ItemHotkey with the parent \a parent.
    */
   SystemHotkey(QObject* parent = nullptr);
   /*!
    * Destructs an ItemHotkey.
    */
   ~SystemHotkey();

   /*!
    * Registers the key sequence \a keySequence.
    */
   bool registerKeySequence(const QKeySequence& keySequence);
   /*!
    * Unregisters the currently registered key sequence.
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

private:
   /*!
    * A native key sequence.
    */
   struct NativeKeySequence_
   {
      std::uint32_t key = ~0;
      std::uint32_t modifier = 0;
   };
   /*!
    * The native key sequence.
    */
   NativeKeySequence_ nativeKeySequence_;

   /*!
    * Returns the native representation of the key of the key sequence \a keySequence.
    */
   static NativeKeySequence_ toNativeKeySequence_(const QKeySequence& keySequence);
};

#endif // SYSTEMHOTKEY_H
