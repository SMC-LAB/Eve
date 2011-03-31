#include <QMainWindow>
#include <QSlider>
#include <QTimeEdit>
#include <QTableWidget>
#include "MarSystemQtWrapper.h"
#include "backend.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void init_();
    void createConnections_();

    Ui::MainWindow *ui_;

    SimplePlayerBackend* backend_;
    MarsyasQt::MarSystemQtWrapper* mwr_;

    QTimer *timer_;

    MarControlPtr filePtr_;
    MarControlPtr gainPtr_;
    MarControlPtr posPtr_;
    MarControlPtr initPtr_;
    MarControlPtr sizePtr_;
    MarControlPtr freqPtr_;
    MarControlPtr curPtr_;
    MarControlPtr labelNamesPtr_;
    

private slots:
    void open();
    void close();
    void play();
    void pause();
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
