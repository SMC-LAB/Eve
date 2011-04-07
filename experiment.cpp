#include "experiment.h"
#include "ui_experiment.h"

Experiment::Experiment(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Experiment)
{
    ui->setupUi(this);
}

Experiment::~Experiment()
{
    delete ui;
}
