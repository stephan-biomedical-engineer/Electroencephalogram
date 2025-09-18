#include "serialhandler.h"
#include <QDebug>
#include <QtEndian>
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

bool SerialHandler::isOpen() const
{
    return m_serialPort.isOpen();
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

void SerialHandler::write(const QByteArray &data)
{
    if (m_serialPort.isOpen()) {
        m_serialPort.write(data);
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
    QByteArray newData = m_serialPort.readAll();

    mh_status_t status = mh_append(&m_rxMessage,
                                   reinterpret_cast<uint8_t*>(newData.data()),
                                   static_cast<size_t>(newData.size()));

    while (status == MH_STATUS_DECODE_OK) {
        if (mh_decode(&m_rxMessage) == MH_STATUS_DECODE_OK) {

            // Sanidade: payload esperado = 4 (ts) + 4*2 (canais) = 12
            if (m_rxMessage.size < 12) {
                mh_init(&m_rxMessage);
                break;
            }

            quint32 timestamp = qFromLittleEndian<quint32>(m_rxMessage.payload);

            QList<quint16> channels;
            for (int i = 0; i < 4; ++i) {
	    	quint16 sample = qFromLittleEndian<quint16>(m_rxMessage.payload + 4 + i*2);
	    	channels.append(sample);  // mantém os 16 bits completos
	     }

            emit eegPacketReady(timestamp, channels);

            // ADICIONE ESTAS DUAS LINHAS DE VOLTA
            qDebug() << "Packet decoded. Timestamp:" << timestamp;
            qDebug() << "Channels:" << channels;

        } else {
            mh_init(&m_rxMessage);
        }

        status = mh_append(&m_rxMessage, nullptr, 0);
    }
}


// #include "serialhandler.h"
// #include <QDebug>
// #include <QtEndian>
// #include <QDataStream>
// #include <QList>

// static constexpr double FS = 10000.0; // Hz

// SerialHandler::SerialHandler(QObject *parent) : QObject(parent)
// {
//     mh_init(&m_rxMessage);
//     connect(&m_serialPort, &QSerialPort::readyRead, this, &SerialHandler::handleReadyRead);
// }

// SerialHandler::~SerialHandler()
// {
//     if (m_serialPort.isOpen()) {
//         m_serialPort.close();
//     }
// }

// bool SerialHandler::isOpen() const
// {
//     return m_serialPort.isOpen();
// }

// void SerialHandler::openSerialPort(const QString &portName, qint32 baudRate)
// {
//     m_serialPort.setPortName(portName);
//     m_serialPort.setBaudRate(baudRate);
//     m_serialPort.setDataBits(QSerialPort::Data8);
//     m_serialPort.setParity(QSerialPort::NoParity);
//     m_serialPort.setStopBits(QSerialPort::OneStop);
//     m_serialPort.setFlowControl(QSerialPort::NoFlowControl);

//     if (m_serialPort.open(QIODevice::ReadOnly)) {
//         qDebug() << "Serial port" << portName << "opened successfully.";
//         emit portStatusChanged(true);
//     } else {
//         qWarning() << "Failed to open serial port:" << m_serialPort.errorString();
//         emit portStatusChanged(false);
//     }
// }

// void SerialHandler::write(const QByteArray &data)
// {
//     if (m_serialPort.isOpen()) {
//         m_serialPort.write(data);
//     }
// }

// void SerialHandler::closeSerialPort()
// {
//     if (m_serialPort.isOpen()) {
//         m_serialPort.close();
//         qDebug() << "Serial port closed.";
//         emit portStatusChanged(false);
//     }
// }

// void SerialHandler::handleReadyRead()
// {
//     QByteArray newData = m_serialPort.readAll();

//     mh_status_t status = mh_append(&m_rxMessage,
//                                    reinterpret_cast<uint8_t*>(newData.data()),
//                                    static_cast<size_t>(newData.size()));

//     while (status == MH_STATUS_DECODE_OK) {
//         if (mh_decode(&m_rxMessage) == MH_STATUS_DECODE_OK) {

//             if (m_rxMessage.size < 16) { // header(4) + tick(4) + 8 bytes canais
//                 mh_init(&m_rxMessage);
//                 break;
//             }

//             const uint8_t *ptr = m_rxMessage.payload;

//             quint32 rawHeader = qFromLittleEndian<quint32>(ptr);
//             bool isSync = rawHeader & 0x80000000u;
//             quint32 sampleIndex = rawHeader & 0x7FFFFFFFu;
//             ptr += 4;

//             quint32 tickMs = 0;
//             if (isSync) {
//                 tickMs = qFromLittleEndian<quint32>(ptr);
//             }
//             ptr += 4;

//             QList<quint16> channels;
//             for (int i = 0; i < 4; ++i) {
//                 quint16 v = qFromLittleEndian<quint16>(ptr);
//                 channels.append(v);
//                 ptr += 2;
//             }

//             static quint32 baseIndex = 0;
//             static double baseTimeSec = 0.0;

//             if (isSync) {
//                 baseIndex = sampleIndex;
//                 baseTimeSec = tickMs / 1000.0;
//             }

//             double timeSec = baseTimeSec + double(sampleIndex - baseIndex) / FS;

//             emit eegPacketReady(timeSec, channels);

//             qDebug() << "Decoded sample" << sampleIndex
//                      << "sync?" << isSync
//                      << "timeSec" << timeSec
//                      << "channels" << channels;

//         } else {
//             mh_init(&m_rxMessage);
//         }

//         status = mh_append(&m_rxMessage, nullptr, 0);
//     }
// }
