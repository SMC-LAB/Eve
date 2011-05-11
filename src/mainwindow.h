#include <QMainWindow>
#include <QtDebug>
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
    void createConnections_();

    Ui::MainWindow *ui_;

    Experiment *experiment_;
    Transport *transport_;
    Tagger *tagger_;

    mrs_string currentFileName_;

private slots:
    void newExperiment();
    void openExperiment();
    void close();
    void quit();
    void preferences();
    void about();
    void init();
    void updateStatusBar(mrs_string fileName, QTableView* ignoreMe);
};
