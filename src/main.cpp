/*!
 * \file main.cpp
 *
 * \copyright 2016 Lukas Geyer. All rights reseverd.
 * \license This program is free software; you can redistribute it and/or modify
 *          it under the terms of the GNU General Public License version 3 as
 *          published by the Free Software Foundation.
 */

#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

#include "itemwindow.h"

namespace {

/*!
 * The default log message handler.
 */
QtMessageHandler logMessageHandler_ = nullptr;
/*!
 * The log file stream.
 */
QTextStream logFileStream_;
/*!
 * The log file handler.
 */
void logFileHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
   static const char* typeString[] = { "DEBUG", "WARNING", "CRITICAL", "FATAL", "INFO", "SYSTEM" };

   logFileStream_ << QDateTime::currentDateTime().toString(Qt::ISODate) << " " << typeString[type] << " " << message.toLocal8Bit() << endl;

   if (logMessageHandler_ != nullptr)
   {
      (*logMessageHandler_)(type, context, message);
   }
}

} // namespace

int main(int argc, char *argv[])
{
   ///
   /// Set up logging. If the log file cannot be openend the default message handler is used.
   ///
   QFile logFile(QStringLiteral("launcher.log"));
   if (logFile.open(QIODevice::Text | QIODevice::Truncate | QIODevice::WriteOnly) == true)
   {
      logFileStream_.setDevice(&logFile);

      logMessageHandler_ = qInstallMessageHandler(logFileHandler);
   }

   ///
   /// Set up the application.
   ///
   QApplication application(argc, argv);
   application.setApplicationName(QStringLiteral("Launcher"));
   application.setApplicationVersion(QStringLiteral("1.0"));
   application.setOrganizationName(QStringLiteral("Lukas Geyer"));
   application.setOrganizationDomain(QStringLiteral("https://github.com/lukasgeyer"));

   ///
   /// Set up the item window.
   ///
   ItemWindow itemWindow;
   itemWindow.show();

   return application.exec();
}
