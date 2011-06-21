#include <QHBoxLayout>
#include <QMainWindow>
#include <QtDebug>
#include <QLabel>
#include "experiment.h"
#include "userinterface.h"

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
    UserInterface *user_;

    QString currentExperimentFileName_;
    mrs_string currentFileName_;
    QLabel *statusLabel_;

    QHBoxLayout *rlayout_;
    QHBoxLayout *llayout_;
                        
private slots:
    void newExperiment();
    void openExperiment();
    void preferences();
    void about();
    void quit();
    void init();
    void updateStatusBar(mrs_string fileName, QTableView* ignoreMe);
    void toggleFullScreen();
};
