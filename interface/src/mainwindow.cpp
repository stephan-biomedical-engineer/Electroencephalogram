// #include "mainwindow.h"
// #include "kiss_fft.h"
// #include <QVBoxLayout>
// #include <QWidget>
// #include <QDebug>

// MainWindow::MainWindow(QWidget *parent)
//     : QMainWindow(parent)
// {
//     QWidget *centralWidget = new QWidget(this);
//     QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    
//     // Substitua o QTextEdit
//     m_customPlot = new QCustomPlot(this);
//     m_connectButton = new QPushButton("Conectar", this);

//     layout->addWidget(m_connectButton);
//     layout->addWidget(m_customPlot); // Adicione o QCustomPlot ao layout

//     setCentralWidget(centralWidget);

//     // Configurar o QCustomPlot
//     m_customPlot->addGraph(); // Canal 1
//     m_customPlot->graph(0)->setPen(QPen(Qt::blue));

//     m_customPlot->addGraph(); // Canal 2
//     m_customPlot->graph(1)->setPen(QPen(Qt::red));

//     m_customPlot->addGraph(); // Canal 3
//     m_customPlot->graph(2)->setPen(QPen(Qt::green));

//     m_customPlot->addGraph(); // Canal 4
//     m_customPlot->graph(3)->setPen(QPen(Qt::magenta));

//     m_customPlot->xAxis->setLabel("Timestamp");
//     m_customPlot->yAxis->setLabel("Bits (ADC)");
//     m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

//     m_fftPlot = new QCustomPlot(this);
//     layout->addWidget(m_fftPlot);

//     m_fftPlot->addGraph();
//     m_fftPlot->graph(0)->setPen(QPen(Qt::darkCyan));
//     m_fftPlot->xAxis->setLabel("Frequência (Hz)");
//     m_fftPlot->yAxis->setLabel("Magnitude");
//     m_fftPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

//     m_tracer = new QCPItemTracer(m_fftPlot);
//     m_tracer->setGraph(m_fftPlot->graph(0));
//     m_tracer->setInterpolating(true); // posição interpolada entre pontos
//     m_tracer->setStyle(QCPItemTracer::tsCircle);
//     m_tracer->setPen(QPen(Qt::red));
//     m_tracer->setBrush(Qt::red);
//     m_tracer->setSize(7);
//     m_tracer->setVisible(false); // inicialmente invisível

//     setCentralWidget(centralWidget);
//     setWindowTitle("EEG Monitor");

//     // Serial
//     m_serialHandler = new SerialHandler(this);
//     connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::on_connectButton_clicked);
//     connect(m_serialHandler, &SerialHandler::eegPacketReady, this, &MainWindow::onEegPacketReady);
//     connect(m_serialHandler, &SerialHandler::portStatusChanged, this, &MainWindow::onPortStatusChanged);

//     connect(m_fftPlot, &QCustomPlot::mouseMove, this, [=](QMouseEvent *event){
//     double x = m_fftPlot->xAxis->pixelToCoord(event->pos().x());
//     double y = m_fftPlot->yAxis->pixelToCoord(event->pos().y());

//     // Atualiza o tracer
//     m_tracer->position->setCoords(x, y);
//     m_tracer->setVisible(true);

//     // Mostrar tooltip com coordenadas
//     m_fftPlot->setToolTip(QString("x: %1 Hz\ny: %2").arg(x, 0, 'f', 2).arg(y, 0, 'f', 2));

//     m_fftPlot->replot();
//     });

// }

// MainWindow::~MainWindow()
// {
//     // A memória será gerenciada automaticamente pelo Qt
// }

// void MainWindow::on_connectButton_clicked()
// {
//     if (m_serialHandler->isOpen()) {
//         // Botão Parar: fecha a porta
//         m_serialHandler->closeSerialPort();
//     } else {
//         // Botão Conectar: abre a porta e envia START
//         m_serialHandler->openSerialPort("/dev/ttyACM0", 921600);
//         QByteArray startCommand;
//         startCommand.append(0x01); // CMD_START_ACQUISITION
//         m_serialHandler->write(startCommand);
//     }
// }

// void MainWindow::onPortStatusChanged(bool isOpen) {
//     m_connectButton->setText(isOpen ? "Parar" : "Conectar");
// }


// void MainWindow::computeFFT(const QVector<double> &samples)
// {
//     int N = samples.size();
//     if ((N & (N - 1)) != 0) {
//         qWarning() << "FFT precisa de potência de 2!";
//         return;
//     }

//     // 1. Subtrair a média (remover componente DC)
//     double mean = 0;
//     for (double v : samples) mean += v;
//     mean /= N;

//     QVector<double> centered(N);
//     for (int i = 0; i < N; ++i)
//         centered[i] = samples[i] - mean;

//     // 2. Normalizar os dados (opcional, mas recomendado)
//     // Dividir por 32768 se seus dados ADC são de 16 bits
//     for (int i = 0; i < N; ++i)
//         centered[i] /= 32768.0;

//     QVector<double> spectrum(N/2);
//     std::vector<kiss_fft_cpx> in(N), out(N);

//     kiss_fft_cfg cfg = kiss_fft_alloc(N, 0, nullptr, nullptr);

//     // Janela Hanning
//     for (int i = 0; i < N; i++) {
//         double w = 0.5 * (1 - cos(2 * M_PI * i / (N - 1)));
//         in[i].r = centered[i] * w;  // Usar dados centralizados e normalizados
//         in[i].i = 0;
//     }

//     kiss_fft(cfg, in.data(), out.data());
//     free(cfg);

//     // Magnitude
//     QVector<double> freqs, mag;
//     double fs = 4000.0; // taxa de amostragem
//     for (int i = 0; i < N/2; i++) {
//         double f = (fs * i) / N;
//         if (f <= 100.0) { // limita até 100 Hz
//             freqs.append(f);
//             // Multiplicar por 2 para compensar a perda de energia da janela
//             // (exceto para componente DC)
//             double magnitude = sqrt(out[i].r*out[i].r + out[i].i*out[i].i);
//             if (i > 0) magnitude *= 2.0; // Compensar para componentes não-DC
//             mag.append(magnitude / N);
//         }
//     }

//     // Atualiza o gráfico FFT
//     m_fftPlot->graph(0)->setData(freqs, mag);
//     m_fftPlot->xAxis->setRange(0, 100); // eixo X fixo
//     double maxY = mag.isEmpty() ? 1.0 : *std::max_element(mag.constBegin(), mag.constEnd());
//     m_fftPlot->yAxis->setRange(0, maxY * 1.1);
//     m_fftPlot->replot();
// }


// void MainWindow::onEegPacketReady(quint32 timestamp, const QList<quint16>& channels)
// {
//     double timestamp_double = static_cast<double>(timestamp);
//     int windowSize = 1024; // Aumentar a janela para ter mais dados
//     int fftSize = 1024;    // Aumentar o tamanho da FFT para melhor resolução

//     // Adiciona os novos dados
//     m_timestamps.append(timestamp_double);
//     for (int i = 0; i < 4; ++i)
//         m_channelData[i].append(static_cast<double>(channels[i]));

//     // Mantém apenas a janela mais recente
//     if (m_timestamps.size() > windowSize) {
//         m_timestamps.remove(0, m_timestamps.size() - windowSize);
//         for (int i = 0; i < 4; ++i)
//             m_channelData[i].remove(0, m_channelData[i].size() - windowSize);
//     }

//     // Atualiza o gráfico EEG
//     for (int i = 0; i < 4; ++i)
//     	{
//           m_customPlot->graph(i)->setData(m_timestamps, m_channelData[i]);
//           // m_customPlot->graph(i)->rescaleValueAxis(true); 
//         }

//     m_customPlot->yAxis->setRange(-1000, 66000); // ajusta Y
//     // m_customPlot->yAxis->setRange(30000, 37000);
    
//     if (!m_timestamps.isEmpty())
//         m_customPlot->xAxis->setRange(m_timestamps.first(), m_timestamps.last()); // janela X
//     m_customPlot->replot();

//     // --- FFT ---
//     // Usar mais pontos para melhor resolução
//     if (m_channelData[0].size() >= fftSize) {
//         QVector<double> window = m_channelData[0].mid(m_channelData[0].size() - fftSize, fftSize);
//         computeFFT(window);
//     }
// }



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

void MainWindow::onEegPacketReady(quint32 timestamp, const QList<quint16>& channels)
{
    double timestamp_double = static_cast<double>(timestamp);
    int windowSize = 256; // Reduced window size for a smaller time axis

    m_timestamps.append(timestamp_double);
    for (int i = 0; i < 4; ++i)
        m_channelData[i].append(static_cast<double>(channels[i]));

    if (m_timestamps.size() > windowSize) {
        m_timestamps.remove(0, m_timestamps.size() - windowSize);
        for (int i = 0; i < 4; ++i)
            m_channelData[i].remove(0, m_channelData[i].size() - windowSize);
    }

    // Atualizar os gráficos usando m_graphs
    for (int i = 0; i < 4; ++i) {
        m_graphs[i]->setData(m_timestamps, m_channelData[i]);
        // m_graphs[i]->rescaleValueAxis(true);

        // Atualiza o eixo X de cada subplot
        QCPAxis *xAxis = m_graphs[i]->keyAxis();
        if (!m_timestamps.isEmpty())
            xAxis->setRange(m_timestamps.first(), m_timestamps.last());

        // Define o range do eixo Y entre 0 e 65535
        QCPAxis *yAxis = m_graphs[i]->valueAxis();
        yAxis->setRange(0, 65535);
    }

    m_customPlot->replot();

    // --- Save data to file ---
    if (m_dataFile.isOpen() && channels.size() >= 4) {
        m_dataStream << timestamp << "\t"
                     << channels[0] << "\t"
                     << channels[1] << "\t"
                     << channels[2] << "\t"
                     << channels[3] << "\n";
        m_dataStream.flush(); // ensure data is written
    }
}

// Add these private members to your MainWindow class in mainwindow.h:
// QFile m_dataFile;
// QTextStream m_dataStream;

