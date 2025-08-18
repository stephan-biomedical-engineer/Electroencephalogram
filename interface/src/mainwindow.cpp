#include "mainwindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    
    // Substitua o QTextEdit
    m_customPlot = new QCustomPlot(this);
    m_connectButton = new QPushButton("Conectar", this);

    layout->addWidget(m_connectButton);
    layout->addWidget(m_customPlot); // Adicione o QCustomPlot ao layout

    setCentralWidget(centralWidget);

    // Configurar o QCustomPlot
    m_customPlot->addGraph(); // Canal 1
    m_customPlot->graph(0)->setPen(QPen(Qt::blue));

    m_customPlot->addGraph(); // Canal 2
    m_customPlot->graph(1)->setPen(QPen(Qt::red));

    m_customPlot->addGraph(); // Canal 3
    m_customPlot->graph(2)->setPen(QPen(Qt::green));

    m_customPlot->addGraph(); // Canal 4
    m_customPlot->graph(3)->setPen(QPen(Qt::magenta));

    m_customPlot->xAxis->setLabel("Timestamp");
    m_customPlot->yAxis->setLabel("Bits (ADC)");
    m_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // Conecte os sinais
    m_serialHandler = new SerialHandler(this);
    connect(m_connectButton, &QPushButton::clicked,
            this, &MainWindow::on_connectButton_clicked);
    connect(m_serialHandler, &SerialHandler::eegPacketReady,
            this, &MainWindow::onEegPacketReady);

    setWindowTitle("EEG Monitor");
}

MainWindow::~MainWindow()
{
    // A memória será gerenciada automaticamente pelo Qt
}

void MainWindow::on_connectButton_clicked()
{
    m_serialHandler->openSerialPort("/dev/ttyUSB0", 115200);
}

void MainWindow::onEegPacketReady(quint16 timestamp, const QList<quint16>& channels)
{
    // Adiciona os novos dados aos vetores de dados
    double timestamp_double = static_cast<double>(timestamp);
    
    m_timestamps.append(timestamp_double);

    for (int i = 0; i < 4; ++i) {
        m_channelData[i].append(static_cast<double>(channels[i]));
    }
    
    // Configura o eixo X para que mostre apenas a última parte dos dados
    // Isso cria o efeito de "rolagem" do gráfico
    int maxDataPoints = 1000; // Por exemplo, mostre os últimos 1000 pontos
    if (m_timestamps.size() > maxDataPoints) {
        m_timestamps.remove(0, m_timestamps.size() - maxDataPoints);
        for (int i = 0; i < 4; ++i) {
            m_channelData[i].remove(0, m_channelData[i].size() - maxDataPoints);
        }
    }
    
    // Atualiza os dados de cada gráfico
    for (int i = 0; i < 4; ++i) {
        m_customPlot->graph(i)->setData(m_timestamps, m_channelData[i]);
    }

    // Auto-escalar os eixos X e Y para se ajustarem aos novos dados
    m_customPlot->rescaleAxes();
    
    // Redesenhar o gráfico para mostrar as mudanças
    m_customPlot->replot();
}
