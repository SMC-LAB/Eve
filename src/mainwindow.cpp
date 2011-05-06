#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
    createConnections_();
}

MainWindow::~MainWindow()
{
    delete ui_;
    delete transport_;
}

void MainWindow::init() 
{
    transport_ = experiment_->getTransport();
    tagger_ = experiment_->getTagger();

    QHBoxLayout *llayout = new QHBoxLayout(ui_->groupBoxTransport);
    QHBoxLayout *rlayout = new QHBoxLayout(ui_->groupBoxTagger);

    tagger_->setCustomDelegate();

    llayout->addWidget(transport_);
    rlayout->addWidget(tagger_);
}

void MainWindow::createConnections_()
{
    connect(ui_->actionPreferences,     SIGNAL(triggered()), this, SLOT(preferences()));
    connect(ui_->actionAbout_Eve,       SIGNAL(triggered()), this, SLOT(about()));
    connect(ui_->actionNew_Experiment,  SIGNAL(triggered()), this, SLOT(newExperiment()));    
    connect(ui_->actionOpen_Experiment, SIGNAL(triggered()), this, SLOT(openExperiment()));
    connect(ui_->actionClose,           SIGNAL(triggered()), this, SLOT(close()));
    connect(ui_->actionQuit,            SIGNAL(triggered()), this, SLOT(quit()));
}

void MainWindow::close()
{
    if (transport_)
    {
        transport_->close();
    }

}

void MainWindow::quit()
{
    close();
    exit(0);
}           

void MainWindow::preferences()
{
    qDebug() << "Preferences: ...";
}

void MainWindow::about()
{
    qDebug() << "About: ...";
    QMessageBox::about ( this, tr("About Eve"),
                         tr("Automated listening tests setup, evaluation and reporting\n"
                            "Copyright 2011 Pedro Silva <pasilva@inescporto.pt>\n"
                            "Licensed under the GNU General Public License version 3."));
}

void MainWindow::newExperiment()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Open Experiment"));

    if (!fileName.isEmpty())
    {
        QFile::remove(fileName);
        experiment_ = new Experiment();
        experiment_->init(fileName);
        experiment_->show();
    }

    connect(experiment_, SIGNAL(experimentConfigured()), this, SLOT(init()));
}

void MainWindow::openExperiment()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Experiment"), 
        tr("SQLite databases (*.db, *.sqlite, *.sqlite3)"));

    if (!fileName.isEmpty())
    {
        experiment_ = new Experiment();
        experiment_->init(fileName);
        experiment_->openCollectionFile();
        experiment_->show();
    }

    connect(experiment_, SIGNAL(experimentConfigured()), this, SLOT(init()));
}
