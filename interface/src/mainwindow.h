// #ifndef MAINWINDOW_H
// #define MAINWINDOW_H

// #include <QMainWindow>
// #include <QTextEdit>
// #include <QPushButton>
// #include <QList>
// #include "serialhandler.h"
// #include "lib/QCustomPlot/qcustomplot.h" // Inclua o cabeçalho do QCustomPlot
// #include "kiss_fft.h"

// class MainWindow : public QMainWindow
// {
//     Q_OBJECT

// public:
//     explicit MainWindow(QWidget *parent = nullptr);
//     ~MainWindow();

// private:
//     QCPItemTracer *m_tracer;


// private slots:
//     void on_connectButton_clicked();
//     void onEegPacketReady(quint32 timestamp, const QList<quint16>& channels);
//     void onPortStatusChanged(bool isOpen);
    
// private:
//     QCustomPlot *m_customPlot; // Substitua o QTextEdit
//     QCustomPlot *m_fftPlot;      // Gráfico da FFT
//     QPushButton *m_connectButton;
//     SerialHandler *m_serialHandler;

//     // Variáveis para armazenar os dados de plotagem
//     QVector<double> m_timestamps;
//     QVector<double> m_channelData[4];
//     void computeFFT(const QVector<double> &samples);
// };

// #endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QList>
#include "serialhandler.h"
#include "lib/QCustomPlot/qcustomplot.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();
    // void onEegPacketReady(quint32 timestamp, const QList<quint16>& channels);
    void onEegPacketReady(quint32 sample_count, const QList<quint16>& channels);
    void onPortStatusChanged(bool isOpen);
    
private:
    QCustomPlot *m_customPlot;
    QPushButton *m_connectButton;
    SerialHandler *m_serialHandler;
    QFile m_dataFile;
    QTextStream m_dataStream;

    QVector<double> m_timestamps;
    QVector<double> m_channelData[4];

    QVector<QCPGraph*> m_graphs;   // <-- aqui o vetor de gráficos
};

#endif // MAINWINDOW_H
