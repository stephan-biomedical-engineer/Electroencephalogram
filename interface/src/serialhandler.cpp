#include "serialhandler.h"
#include <QDebug>
#include "mh.h"
#include <QList>

SerialHandler::SerialHandler(QObject *parent) : QObject(parent)
{
    // Inicialize a estrutura de mensagem
    mh_init(&m_rxMessage);
    
    // Conecta o sinal readyRead() do QSerialPort ao nosso slot interno handleReadyRead()
    // Isso garante que handleReadyRead() será chamado sempre que houver dados na porta
    connect(&m_serialPort, &QSerialPort::readyRead, this, &SerialHandler::handleReadyRead);
}

SerialHandler::~SerialHandler()
{
    // Garante que a porta seja fechada ao destruir o objeto
    if (m_serialPort.isOpen()) {
        m_serialPort.close();
    }
}

void SerialHandler::openSerialPort(const QString &portName, qint32 baudRate)
{
    // Configura a porta
    m_serialPort.setPortName(portName);
    m_serialPort.setBaudRate(baudRate);
    m_serialPort.setDataBits(QSerialPort::Data8);
    m_serialPort.setParity(QSerialPort::NoParity);
    m_serialPort.setStopBits(QSerialPort::OneStop);
    m_serialPort.setFlowControl(QSerialPort::NoFlowControl);

    if (m_serialPort.open(QIODevice::ReadOnly)) {
        qDebug() << "Serial port" << portName << "opened successfully.";
        // Emite sinal para notificar a GUI que a porta foi aberta
        emit portStatusChanged(true);
    } else {
        qDebug() << "Failed to open serial port:" << m_serialPort.errorString();
        // Emite sinal para notificar a GUI sobre o erro
        emit portStatusChanged(false);
    }
}

void SerialHandler::closeSerialPort()
{
    if (m_serialPort.isOpen()) {
        m_serialPort.close();
        qDebug() << "Serial port closed.";
        // Emite sinal para notificar a GUI que a porta foi fechada
        emit portStatusChanged(false);
    }
}

void SerialHandler::handleReadyRead()
{
    // Lê os novos dados brutos da porta serial
    QByteArray newData = m_serialPort.readAll();
    
    // Adiciona os dados ao buffer de decodificação
    mh_status_t status = mh_append(&m_rxMessage, (uint8_t*)newData.constData(), newData.size());
    
    // Loop para processar todos os pacotes completos no buffer
    while (status == MH_STATUS_DECODE_OK) {
        
        // Decodifica o pacote
        if (mh_decode(&m_rxMessage) == MH_STATUS_DECODE_OK) {
            
            // Pacote válido, extrai os dados
            quint16 timestamp = *(quint16*)m_rxMessage.payload;
            
            QList<quint16> channels;
            for (int i = 0; i < 4; ++i) { // 4 canais
                quint16 sample = *(quint16*)(m_rxMessage.payload + sizeof(quint16) + (i * 2));
                channels.append(sample & 0x0FFF); // Mantenha apenas os 12 bits
            }

            // AQUI: Emita o sinal com os dados decodificados
            emit eegPacketReady(timestamp, channels);
            
            // Log de debug para o terminal
            qDebug() << "Packet decoded. Timestamp:" << timestamp;
            qDebug() << "Channels:" << channels;
            
        } else {
            // Erro de CRC ou COBS, limpa a mensagem
            mh_init(&m_rxMessage);
        }
        
        // Tenta processar o próximo pacote no buffer
        status = mh_append(&m_rxMessage, nullptr, 0);
    }
}
