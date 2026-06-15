#include <QCoreApplication>
#include <QHostAddress>
#include <QDebug>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server;

    if (!server.listen(QHostAddress::Any, 5555)) {
        qDebug() << "Server could not start:" << server.errorString();
        return 1;
    }

    qDebug() << "Server started on port 5555";

    return a.exec();
}