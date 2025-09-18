#ifndef SERIALHANDLER_H
#define SERIALHANDLER_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <QString>
#include "mh.h"

class SerialHandler : public QObject
{
    Q_OBJECT

public:
    explicit SerialHandler(QObject *parent = nullptr);
    ~SerialHandler();
    bool isOpen() const;

public slots:
    // Slot para abrir a porta serial com o nome e baud rate
    void openSerialPort(const QString &portName, qint32 baudRate);
    
    void write(const QByteArray &data);
    
    // Slot para fechar a porta
    void closeSerialPort();

signals:
    // Sinal emitido quando novos dados brutos são recebidos
    void dataReceived(const QByteArray &data);
    
    // Sinal para informar o estado da conexão
    void portStatusChanged(bool isOpen);
    
    void eegPacketReady(quint32 timestamp, const QList<quint16>& channels);
    // void onEegPacketReady(quint32 sample_count, const QList<quint16>& channels);

private slots:
    // Slot interno para ser chamado quando há dados disponíveis na porta serial
    void handleReadyRead();

private:
    QSerialPort m_serialPort;
    mh_msg_t m_rxMessage; // Instância da estrutura de C
};

#endif // SERIALHANDLER_H
