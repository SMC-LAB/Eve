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
    
private:
    void init();
    void createConnections();

    Ui::MainWindow *ui;

    SimplePlayerBackend* backend;
    MarsyasQt::MarSystemQtWrapper* mwr;
};
