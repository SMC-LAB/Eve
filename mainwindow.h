#include <QMainWindow>
#include <QtDebug>
#include "transport.h"
#include "tagger.h"
#include "experiment.h"

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

    Experiment *experiment_;
    Transport *transport_;
    Tagger *tagger_;

private slots:
    void newExperiment();
    void preferences();
    void about();
};
