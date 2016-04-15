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
#include <QDebug>
#include <QFile>
#include <QTextStream>

#include "itemlock.h"
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
   int result = EXIT_FAILURE;

   ///
   /// Set up the application.
   ///
   QApplication application(argc, argv);
   application.setApplicationName(QStringLiteral(APPLICATION_NAME));
   application.setApplicationVersion(QStringLiteral(APPLICATION_VERSION));
   application.setOrganizationName(QStringLiteral(ORGANIZATION_NAME));
   application.setOrganizationDomain(QStringLiteral(ORGANIZATION_DOMAIN));

   ///
   /// Try to acquire the application lock and exit immediately if it cannot be acquired (so that
   /// just a single instance of the application is running at a time).
   ///
   ItemLock lock;
   if (lock.tryLock() == true)
   {
      ///
      /// Set up logging. If the log file cannot be openend (just) the default message handler is used.
      ///
      QFile logFile(QStringLiteral("launcher.log"));
      if (logFile.open(QIODevice::Text | QIODevice::Truncate | QIODevice::WriteOnly) == true)
      {
         logFileStream_.setDevice(&logFile);

         logMessageHandler_ = qInstallMessageHandler(logFileHandler);
      }

      ///
      /// Print application name and version.
      ///
      qInfo().noquote() << application.applicationName() << application.applicationVersion();

      ///
      /// Set up the item window.
      ///
      ItemWindow itemWindow;
      itemWindow.show();

      ///
      /// Execute the application.
      ///
      result = application.exec();
   }

   return result;
}
