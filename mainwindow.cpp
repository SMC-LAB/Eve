#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
    newExperiment();
    init_();
    createConnections_();
}

MainWindow::~MainWindow()
{
    delete ui_;
    delete transport_;
}

void MainWindow::init_() 
{
    transport_ = new Transport();
    tagger_ = new Tagger();

    QHBoxLayout *llayout = new QHBoxLayout(ui_->groupBoxTransport);
    QHBoxLayout *rlayout = new QHBoxLayout(ui_->groupBoxTagger);

    llayout->addWidget(transport_);
    rlayout->addWidget(tagger_);
}

void MainWindow::createConnections_()
{
    connect(ui_->actionOpen,  SIGNAL(triggered()), transport_, SLOT(open()));
    connect(ui_->actionClose, SIGNAL(triggered()), transport_, SLOT(close()));
    connect(ui_->actionQuit,  SIGNAL(triggered()), transport_, SLOT(quit()));
    connect(ui_->actionPreferences,  SIGNAL(triggered()), this, SLOT(preferences()));
    connect(ui_->actionAbout_Eve, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui_->actionNew_Experiment, SIGNAL(triggered()), this, SLOT(newExperiment()));
    
}

void MainWindow::preferences()
{
    qDebug() << "Preferences: ...";
}

void MainWindow::about()
{
    qDebug() << "About: ...";
    QMessageBox::about ( this, "About Eve", "Automated listening tests setup, evaluation and reporting\n"
                                            "Copyright 2011 Pedro Silva <pasilva@inescporto.pt>\n"
                                            "Licensed under the GNU General Public License version 3.");
}

void MainWindow::newExperiment()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (!fileName.isEmpty())
    {
        QFile::remove(fileName);
        experiment_ = new Experiment();
        experiment_->init(fileName);
        experiment_->show();
    }    
}
