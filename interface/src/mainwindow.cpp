#include "mainwindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    
    m_customPlot = new QCustomPlot(this);
    m_connectButton = new QPushButton("Conectar", this);

    layout->addWidget(m_connectButton);
    layout->addWidget(m_customPlot);

    setCentralWidget(centralWidget);

    // Criar múltiplos "painéis" verticais, um para cada canal
    QCPAxisRect *mainRect = m_customPlot->axisRect(); // default
    mainRect->setupFullAxesBox();
    mainRect->axis(QCPAxis::atBottom)->setVisible(false); // não mostra X no topo

    QVector<QColor> cores = {Qt::blue, Qt::red, Qt::green, Qt::magenta};

    for (int i = 0; i < 4; ++i) {
        QCPAxisRect *axisRect;
        if (i == 0) {
            axisRect = mainRect; // usa o primeiro como base
        } else {
            axisRect = new QCPAxisRect(m_customPlot, true);
            m_customPlot->plotLayout()->addElement(i, 0, axisRect);

            if (i < 3) {
                axisRect->axis(QCPAxis::atBottom)->setVisible(false); // esconde X
            } else {
                axisRect->axis(QCPAxis::atBottom)->setLabel("Timestamp"); // só o último mostra
            }
        }

        axisRect->axis(QCPAxis::atLeft)->setLabel(QString("Canal %1 (ADC)").arg(i+1));

        QCPGraph *graph = m_customPlot->addGraph(axisRect->axis(QCPAxis::atBottom),
                                                 axisRect->axis(QCPAxis::atLeft));
        graph->setPen(QPen(cores[i], 1.5));
        m_graphs.append(graph); // <-- guarda o ponteiro
    }

    m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    m_customPlot->plotLayout()->setRowStretchFactor(0, 1.0);
    m_customPlot->plotLayout()->setRowSpacing(2); // pequeno espaço entre gráficos

    setWindowTitle("EEG Monitor");

    // Serial
    m_serialHandler = new SerialHandler(this);
    connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::on_connectButton_clicked);
    connect(m_serialHandler, &SerialHandler::eegPacketReady, this, &MainWindow::onEegPacketReady);
    connect(m_serialHandler, &SerialHandler::portStatusChanged, this, &MainWindow::onPortStatusChanged);

    // --- Create folder and file for EEG data ---
    QString folderName = "eeg_data";
    QDir dir;
    if (!dir.exists(folderName)) {
        dir.mkpath(folderName);
    }
    QString timestampStr = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
    QString fileName = QString("%1/eeg_data_%2.txt").arg(folderName, timestampStr);

    m_dataFile.setFileName(fileName);
    if (!m_dataFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Não foi possível abrir" << fileName << "para escrita!";
    } else {
        m_dataStream.setDevice(&m_dataFile);
        // Optional: write header
        m_dataStream << "timestamp\tch1\tch2\tch3\tch4\n";
    }
}

MainWindow::~MainWindow()
{
    if (m_dataFile.isOpen())
        m_dataFile.close();
}

void MainWindow::on_connectButton_clicked()
{
    if (m_serialHandler->isOpen()) {
        m_serialHandler->closeSerialPort();
    } else {
        m_serialHandler->openSerialPort("/dev/ttyACM0", 921600);
        QByteArray startCommand;
        startCommand.append(0x01); // CMD_START_ACQUISITION
        m_serialHandler->write(startCommand);
    }
}

void MainWindow::onPortStatusChanged(bool isOpen) {
    m_connectButton->setText(isOpen ? "Parar" : "Conectar");
}


void MainWindow::onEegPacketReady(quint32 sample_count, const QList<quint16>& channels)
{
    // --- Conversão para tempo real ---
    const double SAMPLE_RATE_HZ = 4000.0; // 4kHz total
    double time_seconds = sample_count / SAMPLE_RATE_HZ;
    
    int windowSize = 100;

    m_timestamps.append(time_seconds);
    for (int i = 0; i < 4; ++i)
        m_channelData[i].append(static_cast<double>(channels[i]));

    // Manter apenas a janela de tempo desejada
    if (m_timestamps.size() > windowSize) {
        m_timestamps.remove(0, m_timestamps.size() - windowSize);
        for (int i = 0; i < 4; ++i)
            m_channelData[i].remove(0, m_channelData[i].size() - windowSize);
    }

    // Atualizar gráficos
    for (int i = 0; i < 4; ++i) {
        m_graphs[i]->setData(m_timestamps, m_channelData[i]);
        
        // Auto-scale no eixo Y se desejar
        // m_graphs[i]->rescaleValueAxis(true);
        
        // Range fixo no Y (0-65535 para ADC de 16 bits)
        QCPAxis *yAxis = m_graphs[i]->valueAxis();
        //yAxis->setRange(0, 65535);
        
        // Range no eixo X para mostrar a janela de tempo
        if (!m_timestamps.isEmpty()) {
            QCPAxis *xAxis = m_graphs[i]->keyAxis();
            xAxis->setRange(m_timestamps.first(), m_timestamps.last());
        }
    }

    m_customPlot->replot();

    // --- Salvar dados no arquivo ---
    if (m_dataFile.isOpen() && channels.size() >= 4) {
        m_dataStream << time_seconds << "\t"  // Tempo em segundos
                     << channels[0] << "\t"
                     << channels[1] << "\t"
                     << channels[2] << "\t"
                     << channels[3] << "\n";
        m_dataStream.flush();
    }
    
    qDebug() << "Tempo:" << time_seconds << "s, Amostra:" << sample_count;
}
