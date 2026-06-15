#include <QCoreApplication>
#include <QTcpSocket>
#include <QDebug>
#include <cstdio>
#include "inputreader.h"

// Консольный клиент для проверки TCP-сервера.
// Запуск:  ./qt_tcp_client [host] [port]
// По умолчанию: 127.0.0.1 5555
//
// Всё, что приходит от сервера, выводится в консоль.
// Всё, что пользователь вводит в консоли, отправляется на сервер.

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString host = "127.0.0.1";
    quint16 port = 5555;

    if (argc > 1) {
        host = QString::fromLocal8Bit(argv[1]);
    }
    if (argc > 2) {
        port = static_cast<quint16>(QString::fromLocal8Bit(argv[2]).toUInt());
    }

    QTcpSocket socket;
    socket.connectToHost(host, port);

    if (!socket.waitForConnected(3000)) {
        qDebug() << "Connection error:" << socket.errorString();
        return 1;
    }

    qDebug() << "Connected to" << host << "port" << port;

    // Вывод сообщений сервера в консоль
    QObject::connect(&socket, &QTcpSocket::readyRead, [&socket]() {
        QByteArray data = socket.readAll();
        std::fputs(data.constData(), stdout);
        std::fflush(stdout);
    });

    // Завершение программы при разрыве соединения сервером
    QObject::connect(&socket, &QTcpSocket::disconnected, []() {
        qDebug() << "\nConnection closed by server.";
        QCoreApplication::quit();
    });

    // Чтение ввода пользователя в отдельном потоке и отправка на сервер
    InputReader reader;
    QObject::connect(&reader, &InputReader::lineRead, [&socket](const QString &line) {
        socket.write((line + "\n").toUtf8());
        socket.flush();
    });
    reader.start();

    int ret = a.exec();

    reader.terminate();
    reader.wait();

    return ret;
}