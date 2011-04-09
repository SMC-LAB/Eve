#include <QMainWindow>
#include "transport.h"

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

    Transport *transport_;
};
