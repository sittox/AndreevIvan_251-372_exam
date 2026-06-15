#ifndef INPUTREADER_H
#define INPUTREADER_H

#include <QThread>
#include <QTextStream>

// Отдельный поток для чтения строк из консоли,
// чтобы не блокировать главный цикл обработки событий Qt,
// в котором обрабатываются данные от сервера.
class InputReader : public QThread
{
    Q_OBJECT

public:
    explicit InputReader(QObject *parent = nullptr) : QThread(parent) {}

signals:
    void lineRead(const QString &line);

protected:
    void run() override
    {
        QTextStream in(stdin);
        while (!in.atEnd()) {
            QString line = in.readLine();
            emit lineRead(line);
        }
    }
};

#endif // INPUTREADER_H