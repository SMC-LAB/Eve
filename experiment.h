#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QMainWindow>

namespace Ui {
    class Experiment;
}

class Experiment : public QMainWindow
{
    Q_OBJECT

public:
    explicit Experiment(QWidget *parent = 0);
    ~Experiment();

private:
    Ui::Experiment *ui;
};

#endif // EXPERIMENT_H
