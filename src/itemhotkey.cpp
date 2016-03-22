/*!
 * \file itemhotkey.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QAbstractEventDispatcher>
#include <QDebug>

#if defined(Q_OS_LINUX)
   #include <QX11Info>
   #include <xcb/xcb.h>
#elif defined(Q_OS_WIN)
   #include <qt_windows.h>
#endif // defined(Q_OS_LINUX)

#include "itemhotkey.h"

ItemHotkey::ItemHotkey(QObject* parent) : QObject(parent)
{
}

ItemHotkey::~ItemHotkey()
{
   unregisterKeySequence();
}

void ItemHotkey::registerKeySequence()
{
   QAbstractEventDispatcher::instance()->installNativeEventFilter(this);

   auto error = 0;
   auto errorString = QStringLiteral("Unknown error");

#if defined(Q_OS_LINUX)
   auto result = xcb_request_check(QX11Info::connection(), xcb_grab_key_checked(QX11Info::connection(), false, QX11Info::appRootWindow(),
                                                                                XCB_MOD_MASK_CONTROL | XCB_MOD_MASK_SHIFT, 65 /* Space */,
                                                                                XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC));
   if (result != nullptr)
   {
      error = result->error_code;
   }

#elif defined(Q_OS_WIN)
   if (RegisterHotKey(NULL, 0, MOD_CONTROL | MOD_SHIFT, VK_SPACE) == 0)
   {
      error = GetLastError();

      auto lastErrorString = static_cast<LPSTR>(nullptr);
      auto lastErrorStringSize = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                                FORMAT_MESSAGE_FROM_SYSTEM |
                                                FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error,
                                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), &lastErrorString, 0, nullptr);

      errorString = QString(lastErrorString, lastErrorStringSize);

      LocalFree(lastErrorString);
   }
#endif // defined(Q_OS_LINUX)

   if (error != 0)
   {
      qCritical() << "register hotkey failed:" << error << errorString;
   }
}

void ItemHotkey::unregisterKeySequence()
{
   QAbstractEventDispatcher::instance()->removeNativeEventFilter(this);

#if defined(Q_OS_LINUX)
   xcb_ungrab_key(QX11Info::connection(), 65, QX11Info::appRootWindow(),
                  XCB_MOD_MASK_CONTROL | XCB_MOD_MASK_SHIFT);
#elif defined(Q_OS_WIN)
   UnregisterHotKey(NULL, 0);
#endif // defined(Q_OS_LINUX)
}

bool ItemHotkey::nativeEventFilter(const QByteArray& /* eventType */, void *message, long* /* result */)
{
#if defined(Q_OS_LINUX)
   auto eventMessage = static_cast<xcb_generic_event_t*>(message);
   if ((eventMessage->response_type & 0x7F) == XCB_KEY_PRESS)
   {
      auto keyPressedEventMessage = static_cast<xcb_key_press_event_t*>(message);
      if ((keyPressedEventMessage->state & XCB_MOD_MASK_CONTROL) &&
          (keyPressedEventMessage->state & XCB_MOD_MASK_SHIFT) &&
          (keyPressedEventMessage->detail == 65 /* Space */))
      {
         hotkeyPressed();
      }
   }
#elif defined(Q_OS_WIN)
   auto eventMessage = static_cast<MSG*>(message);
   if (eventMessage->message == WM_HOTKEY)
   {
      if ((LOWORD(eventMessage->lParam) == (MOD_CONTROL | MOD_SHIFT)) && (HIWORD(eventMessage->lParam) == VK_SPACE))
      {
         emit hotkeyPressed();
      }
   }
#endif // defined(Q_OS_LINUX)

   return false;
}
