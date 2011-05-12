#include <QMainWindow>
#include <QtDebug>
#include <QLabel>
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
    QLabel *statusLabel_;
                        
private slots:
    void newExperiment();
    void openExperiment();
    void preferences();
    void about();
    void quit();
    void init();
    void updateStatusBar(mrs_string fileName, QTableView* ignoreMe);
};
