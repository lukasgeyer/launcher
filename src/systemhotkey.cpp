/*!
 * \file systemhotkey.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QAbstractEventDispatcher>
#include <QDebug>
#include <QHash>

#if defined(Q_OS_LINUX)
   #include <QX11Info>
   #include <xcb/xcb.h>
#elif defined(Q_OS_WIN)
   #include <qt_windows.h>
#endif // defined(Q_OS_LINUX)

#include "systemhotkey.h"

SystemHotkey::SystemHotkey(QObject* parent) : QObject(parent)
{
}

SystemHotkey::~SystemHotkey()
{
   unregisterKeySequence();
}

bool SystemHotkey::registerKeySequence(const QKeySequence &keySequence)
{
   bool keySequenceRegistered = false;

   nativeKeySequence_ = toNativeKeySequence_(keySequence);

   qDebug() << "register hotkey:" << keySequence << "native key:" << nativeKeySequence_.key << "native modifier:" << nativeKeySequence_.modifier;

#if defined(Q_OS_LINUX)
   auto result = xcb_request_check(QX11Info::connection(), xcb_grab_key_checked(QX11Info::connection(), false,
                                                                                QX11Info::appRootWindow(),
                                                                                XCB_MOD_MASK_CONTROL | XCB_MOD_MASK_SHIFT, 65 /* Space */,
                                                                                XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC));

   keySequenceRegistered = ((result == nullptr) || (result->error_code == 0));
   if (keySequenceRegistered)
   {
      QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
   }
   else
   {
      qCritical() << "register hotkey failed:" << result->error_code;
   }

#elif defined(Q_OS_WIN)
   keySequenceRegistered = (RegisterHotKey(NULL, 0, nativeKeySequence_.modifier, nativeKeySequence_.key));
   if (keySequenceRegistered)
   {
      QAbstractEventDispatcher::instance()->installNativeEventFilter(this);
   }
   else
   {
      auto lastError = GetLastError();
      auto lastErrorString = static_cast<LPSTR>(nullptr);
      auto lastErrorStringSize = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                                FORMAT_MESSAGE_FROM_SYSTEM |
                                                FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, lastError,
                                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), lastErrorString, 0, nullptr);

      qCritical() << "register hotkey failed:" << lastError << QString::fromLocal8Bit(lastErrorString, lastErrorStringSize);

      LocalFree(lastErrorString);
   }
#endif // defined(Q_OS_LINUX)

   return keySequenceRegistered;
}

void SystemHotkey::unregisterKeySequence()
{
   QAbstractEventDispatcher::instance()->removeNativeEventFilter(this);

#if defined(Q_OS_LINUX)
   xcb_ungrab_key(QX11Info::connection(), 65, QX11Info::appRootWindow(),
                    XCB_MOD_MASK_CONTROL | XCB_MOD_MASK_SHIFT);
#elif defined(Q_OS_WIN)
   UnregisterHotKey(NULL, 0);
#endif // defined(Q_OS_LINUX)
}

bool SystemHotkey::nativeEventFilter(const QByteArray& /* eventType */, void *message, long* /* result */)
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
         emit hotkeyPressed();
      }
   }
#elif defined(Q_OS_WIN)
   auto eventMessage = static_cast<MSG*>(message);
   if (eventMessage->message == WM_HOTKEY)
   {
      if ((LOWORD(eventMessage->lParam) == nativeKeySequence_.modifier) && (HIWORD(eventMessage->lParam) == nativeKeySequence_.key))
      {
         emit hotkeyPressed();
      }
   }
#endif // defined(Q_OS_LINUX)

   return false;
}

SystemHotkey::NativeKeySequence_ SystemHotkey::toNativeKeySequence_(const QKeySequence& keySequence)
{
   SystemHotkey::NativeKeySequence_ nativeKeySequence;

   if (!keySequence.isEmpty())
   {
      auto key = keySequence[0] & ~Qt::KeyboardModifierMask;

#if defined(Q_OS_LINUX)
      Q_UNUSED(key);
#elif defined(Q_OS_WIN)
      static QHash<int, int> nativeKeyMapping
      {
         { Qt::Key_Escape, VK_ESCAPE },              { Qt::Key_Tab, VK_TAB },                         { Qt::Key_Backtab, VK_TAB },
         { Qt::Key_Backspace, VK_BACK },             { Qt::Key_Return, VK_RETURN },                   { Qt::Key_Enter, VK_RETURN },
         { Qt::Key_Insert, VK_INSERT },              { Qt::Key_Delete, VK_DELETE },                   { Qt::Key_Pause, VK_PAUSE },
         { Qt::Key_Print, VK_PRINT },                { Qt::Key_Clear, VK_CLEAR },                     { Qt::Key_Home, VK_HOME },
         { Qt::Key_End, VK_END },                    { Qt::Key_Left, VK_LEFT },                       { Qt::Key_Up, VK_UP },
         { Qt::Key_Right, VK_RIGHT },                { Qt::Key_Down, VK_DOWN },                       { Qt::Key_PageUp, VK_PRIOR },
         { Qt::Key_PageDown, VK_NEXT },              { Qt::Key_CapsLock, VK_CAPITAL },                { Qt::Key_NumLock, VK_NUMLOCK },
         { Qt::Key_ScrollLock, VK_SCROLL },          { Qt::Key_F1, VK_F1 },                           { Qt::Key_F2, VK_F2 },
         { Qt::Key_F3, VK_F3 },                      { Qt::Key_F4, VK_F4 },                           { Qt::Key_F5, VK_F5 },
         { Qt::Key_F6, VK_F6 },                      { Qt::Key_F7, VK_F7 },                           { Qt::Key_F8, VK_F8 },
         { Qt::Key_F9, VK_F9 },                      { Qt::Key_F10, VK_F10 },                         { Qt::Key_F11, VK_F11 },
         { Qt::Key_F12, VK_F12 },                    { Qt::Key_F13, VK_F13 },                         { Qt::Key_F14, VK_F14 },
         { Qt::Key_F15, VK_F15 },                    { Qt::Key_F16, VK_F16 },                         { Qt::Key_F17, VK_F17 },
         { Qt::Key_F18, VK_F18 },                    { Qt::Key_F19, VK_F19 },                         { Qt::Key_F20, VK_F20 },
         { Qt::Key_F21, VK_F21 },                    { Qt::Key_F22, VK_F22 },                         { Qt::Key_F23, VK_F23 },
         { Qt::Key_F24, VK_F24 },                    { Qt::Key_Menu, VK_APPS },                       { Qt::Key_Help, VK_HELP },
         { Qt::Key_MediaNext, VK_MEDIA_NEXT_TRACK }, { Qt::Key_MediaPrevious, VK_MEDIA_PREV_TRACK },  { Qt::Key_MediaPlay, VK_MEDIA_PLAY_PAUSE },
         { Qt::Key_MediaStop, VK_MEDIA_STOP },       { Qt::Key_VolumeDown, VK_VOLUME_DOWN },          { Qt::Key_VolumeUp, VK_VOLUME_UP },
         { Qt::Key_VolumeMute, VK_VOLUME_MUTE },     { Qt::Key_Mode_switch, VK_MODECHANGE },          { Qt::Key_Select, VK_SELECT },
         { Qt::Key_Printer, VK_PRINT },              { Qt::Key_Execute, VK_EXECUTE },                 { Qt::Key_Sleep, VK_SLEEP },
         { Qt::Key_Period, VK_DECIMAL },             { Qt::Key_Play, VK_PLAY },                       { Qt::Key_Cancel, VK_CANCEL },
         { Qt::Key_Forward, VK_BROWSER_FORWARD },    { Qt::Key_Refresh, VK_BROWSER_REFRESH },         { Qt::Key_Stop, VK_BROWSER_STOP },
         { Qt::Key_Search, VK_BROWSER_SEARCH },      { Qt::Key_Favorites, VK_BROWSER_FAVORITES },     { Qt::Key_HomePage, VK_BROWSER_HOME },
         { Qt::Key_LaunchMail, VK_LAUNCH_MAIL },     { Qt::Key_LaunchMedia, VK_LAUNCH_MEDIA_SELECT }, { Qt::Key_Launch0, VK_LAUNCH_APP1 },
         { Qt::Key_Launch1, VK_LAUNCH_APP2 }
      };

      nativeKeySequence.key = nativeKeyMapping.value(key, ~0);
      if ((nativeKeySequence.key == ~0) && (key < 0xffff))
      {
          nativeKeySequence.key = static_cast<decltype(nativeKeySequence.key)>(key);
      }
#endif // defined(Q_OS_LINUX)

      auto modifiers = keySequence[0] & Qt::KeyboardModifierMask;

#if defined(Q_OS_LINUX)
      Q_UNUSED(modifiers);
#elif defined(Q_OS_WIN)
      if (modifiers & Qt::ShiftModifier)
          nativeKeySequence.modifier |= MOD_SHIFT;
      if (modifiers & Qt::ControlModifier)
          nativeKeySequence.modifier |= MOD_CONTROL;
      if (modifiers & Qt::AltModifier)
          nativeKeySequence.modifier |= MOD_ALT;
      if (modifiers & Qt::MetaModifier)
          nativeKeySequence.modifier |= MOD_WIN;
#endif // defined(Q_OS_LINUX)
   }

   return nativeKeySequence;
}
