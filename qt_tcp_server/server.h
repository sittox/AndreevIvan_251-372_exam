#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);

protected:
    // Вызывается автоматически при каждом новом подключении
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    static const int MAX_CLIENTS = 5;

    QList<QTcpSocket *> clients;

    bool gameStarted;
    int  targetNumber;   // загаданное число
    int  currentTurn;    // индекс клиента, чья сейчас очередь

    void sendToClient(QTcpSocket *socket, const QString &message);
    void broadcast(const QString &message);

    void startGame();
    void announceTurn();
    void finishGame();
};

#endif // SERVER_H
