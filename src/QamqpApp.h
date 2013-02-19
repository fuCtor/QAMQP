#ifndef QAMQPAPP_H
#define QAMQPAPP_H

#include <stdexcept>
#include <QCoreApplication>
#include <QDebug>
#include <QStringList>
#include <QTextStream>
#include <QTimer>

#include "qamqp/amqp.h"
#include "qamqp/amqp_exchange.h"
#include "qamqp/amqp_queue.h"

#include "sendreceive/Send.h"
#include "sendreceive/Receive.h"

namespace QAMQP
{

namespace samples
{

class QamqpApp : public QCoreApplication
{
    Q_OBJECT

    typedef QCoreApplication super;

public:
    explicit QamqpApp(int& argc, char** argv)
        : super(argc, argv)
    {
        QTimer::singleShot(0, this, SLOT(run()));
    }

    bool notify(QObject *obj, QEvent *event)
    {
        try
        {
            return super::notify(obj, event);
        }
        catch (std::exception& ex)
        {
            qWarning() << ex.what();
            return false;
        }
    }

protected slots:
    void run()
    {
        QTextStream cout(stdout);
        try
        {
            QStringList args = arguments();

            if (args.size() == 1 || "-h" == args[1] || "--help" == args[1])
            {
                printUsage(cout);
                quit();
                return;
            }

            QString command = args[1];
            QRunnable* commandImpl = 0;

            if ("send" == command)
            {
                if (args.size() < 4)
                    throw std::runtime_error("Mandatory argument(s) missing!");

                QString url = args[2];
                QString msg = args[3];
                commandImpl = new Send(url, msg, this);
            }
            else if ("receive" == command)
            {
                if (args.size() < 3)
                    throw std::runtime_error("Mandatory argument(s) missing!");

                QString url = args[2];
                commandImpl = new Receive(url, this);
            }
            else
            {
                throw std::runtime_error(QString("Unknown command: '%1'").arg(command).toStdString());
            }

            // Run command.
            commandImpl->run();
        }
        catch (std::exception& ex)
        {
            qWarning() << ex.what();
            exit(-1);
        }
    }

protected:
    void printUsage(QTextStream& out)
    {
        QString executable = arguments().at(0);
        out << QString(
"\n\
USAGE: %1 [-h|--help]                       -- Show this help message.\n\
\n\
USAGE: %1 send    <server-url> <message>    -- Send a message.\n\
       %1 receive <server-url>              -- Receive messages.\n\
\n\
Send-Receive Sample:\n\
* Producer: %1 send    amqp://guest:guest@127.0.0.1:5672/ \"Hello World\"\n\
* Consumer: %1 receive amqp://guest:guest@127.0.0.1:5672/\n\
\n").arg(executable);
    }
};

}

}

#endif // QAMQPAPP_H
