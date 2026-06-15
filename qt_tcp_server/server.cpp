#include "server.h"
#include <QDebug>
#include <QRandomGenerator>

Server::Server(QObject *parent)
    : QTcpServer(parent)
    , gameStarted(false)
    , targetNumber(0)
    , currentTurn(0)
{
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        delete socket;
        return;
    }

    // Лишний клиент: сервер занят, отвечает и сразу разрывает соединение
    if (clients.size() >= MAX_CLIENTS) {
        sendToClient(socket, "Server is busy. Please try again later.\n");
        socket->flush();
        socket->disconnectFromHost();
        socket->deleteLater();
        qDebug() << "Extra client rejected. Current clients:" << clients.size();
        return;
    }

    connect(socket, &QTcpSocket::readyRead,  this, &Server::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &Server::onDisconnected);

    clients.append(socket);

    int clientNumber = clients.size();
    sendToClient(socket, QString("Welcome! You are client #%1\n").arg(clientNumber));

    broadcast(QString("Current number of connected clients: %1\n").arg(clients.size()));

    qDebug() << "Client connected. Total clients:" << clients.size();

    if (clients.size() == MAX_CLIENTS && !gameStarted) {
        startGame();
    }
}

void Server::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) {
        return;
    }

    while (socket->canReadLine()) {
        QByteArray line = socket->readLine().trimmed();

        if (!gameStarted) {
            sendToClient(socket, "Game has not started yet. Please wait.\n");
            continue;
        }

        int idx = clients.indexOf(socket);
        if (idx < 0) {
            continue;
        }

        if (idx != currentTurn) {
            sendToClient(socket, "Not your turn. Please wait.\n");
            continue;
        }

        bool ok = false;
        int guess = line.toInt(&ok);

        if (!ok) {
            sendToClient(socket, "Invalid input. Enter an integer number.\n");
            continue;
        }

        qDebug() << "Client" << (idx + 1) << "guessed:" << guess
                 << "(target =" << targetNumber << ")";

        if (guess > targetNumber) {
            sendToClient(socket, "Больше\n");
        } else if (guess < targetNumber) {
            sendToClient(socket, "Меньше\n");
        } else {
            finishGame();
            return;
        }

        currentTurn = (currentTurn + 1) % clients.size();
        announceTurn();
    }
}

void Server::onDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) {
        return;
    }

    clients.removeAll(socket);
    socket->deleteLater();

    broadcast(QString("Current number of connected clients: %1\n").arg(clients.size()));

    qDebug() << "Client disconnected. Total clients:" << clients.size();
}

void Server::sendToClient(QTcpSocket *socket, const QString &message)
{
    socket->write(message.toUtf8());
    socket->flush();
}

void Server::broadcast(const QString &message)
{
    for (QTcpSocket *c : clients) {
        sendToClient(c, message);
    }
}

void Server::startGame()
{
    gameStarted  = true;
    targetNumber = QRandomGenerator::global()->bounded(1, 101); // от 1 до 100
    currentTurn  = 0;

    broadcast("All 5 clients are connected. Game 'Guess the number' (1-100) starts!\n");

    qDebug() << "Game started. Target number =" << targetNumber;

    announceTurn();
}

void Server::announceTurn()
{
    if (clients.isEmpty()) {
        return;
    }

    broadcast(QString("It is client #%1's turn to guess.\n").arg(currentTurn + 1));
    sendToClient(clients[currentTurn], "Your turn! Enter your guess (1-100):\n");
}

void Server::finishGame()
{
    broadcast("Угадано\n");

    qDebug() << "Number guessed! Disconnecting all clients.";

    for (QTcpSocket *c : clients) {
        c->flush();
        c->disconnectFromHost();
    }

    gameStarted = false;
}