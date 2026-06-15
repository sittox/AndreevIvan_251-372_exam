# Базовый образ
FROM alpine:3.14

# Метка
LABEL description="QT TCP Server Container"

# Установка Qt5, qmake и компилятора
RUN apk update && \
    apk add --no-cache qt5-qtbase-dev qt5-qttools-dev g++ make

# Рабочая директория
WORKDIR /home/server

# Копирование файлов проекта
COPY . /home/server

# Найти qmake и собрать проект
RUN find /usr -name "qmake" 2>/dev/null || echo "qmake not found" && \
    /usr/lib/qt5/bin/qmake qt_tcp_server.pro && make

# Открываем порт сервера
EXPOSE 5555

# Автоматический запуск сервера
CMD ["./qt_tcp_server"]