#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QList>
#include "serialhandler.h"
#include "lib/QCustomPlot/qcustomplot.h" // Inclua o cabeçalho do QCustomPlot

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();
    void onEegPacketReady(quint32 timestamp, const QList<quint16>& channels);
    void onPortStatusChanged(bool isOpen);
    
private:
    QCustomPlot *m_customPlot; // Substitua o QTextEdit
    QPushButton *m_connectButton;
    SerialHandler *m_serialHandler;

    // Variáveis para armazenar os dados de plotagem
    QVector<double> m_timestamps;
    QVector<double> m_channelData[4];
};

#endif // MAINWINDOW_H
