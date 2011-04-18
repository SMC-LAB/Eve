#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <QWidget>
#include <QSlider>
#include <QTimeEdit>
#include <QTableWidget>
#include "MarSystemQtWrapper.h"
#include "backend.h"

namespace Ui {
    class Transport;
}

class Transport : public QWidget
{
    Q_OBJECT

public:
    explicit Transport(QWidget *parent = 0);
    ~Transport();

private:
    void init_();
    void createConnections_();
    void initPlayTable();

    Ui::Transport *ui_;

    SimplePlayerBackend *backend_;
    MarsyasQt::MarSystemQtWrapper *mwr_;

    QTimer *timer_;
    QTableWidget *table_;

    MarControlPtr filenamePtr_;
    MarControlPtr posPtr_;
    MarControlPtr gainPtr_;
    MarControlPtr initAudioPtr_;
    MarControlPtr sizePtr_;
    MarControlPtr osratePtr_;

    MarControlPtr hasDataPtr_;
    MarControlPtr activePtr_;
    
    MarControlPtr numFilesPtr_;
    MarControlPtr allfilenamesPtr_;
    MarControlPtr currentlyPlayingPtr_;
    
    MarControlPtr nLabelsPtr_;
    MarControlPtr labelNamesPtr_;
    MarControlPtr currentLabelPtr_;

    MarControlPtr advancePtr_;
    MarControlPtr cindexPtr_;

private slots:
    void open();
    void close();
    void play();
    void previous();
    void next();
    void setGain(int val);
    void update();
    void setPos(int val); 
    void quit();
    void moveSlider(int val, QSlider *slider);
    void setTime(int val, QTimeEdit *time);
    void setCurrentFile(mrs_string file, QTableWidget *table);

signals:
    void sliderChanged(int val, QSlider *slider);
    void timeChanged(int val, QTimeEdit *time);
    void fileChanged(mrs_string file, QTableWidget *table);
};

#endif // TRANSPORT_H
