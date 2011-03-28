#include <QMainWindow>
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

public slots:
    void open();
    void close();
    void play();
    void pause();
    void setGain(int val);
//    void setPos(); 
    void quit();
    
private:
    void init_();
    void createConnections_();

    Ui::MainWindow *ui_;

    SimplePlayerBackend* backend_;
    MarsyasQt::MarSystemQtWrapper* mwr_;

    MarControlPtr filePtr_;
    MarControlPtr gainPtr_;
    MarControlPtr posPtr_;
};
