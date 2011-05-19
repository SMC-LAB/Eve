#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
    createConnections_();
    currentFileName_ = "";
    experiment_ = NULL;
    transport_ = NULL;
    tagger_ = NULL;
}

MainWindow::~MainWindow()
{
    delete ui_;
    delete experiment_;
}

void MainWindow::init() 
{
    static QHBoxLayout *llayout = new QHBoxLayout(ui_->groupBoxTransport);
    transport_ = experiment_->getTransport();
    llayout->addWidget(transport_);
    
    static QHBoxLayout *rlayout = new QHBoxLayout(ui_->groupBoxTagger);
    tagger_ = experiment_->getTagger();
    tagger_->initTagWidget();
    rlayout->addWidget(tagger_);

    map<QString, QString> subject = experiment_->getCurrentSubject();
    
    statusLabel_ = new QLabel(QString("Subject: " + subject["Name"]));
    statusBar()->addWidget(statusLabel_);
    connect(transport_, SIGNAL(fileChanged(mrs_string, QTableView*)), this, SLOT(updateStatusBar(mrs_string, QTableView*)));
}

void MainWindow::updateStatusBar(mrs_string fileName, QTableView* ignoreMe)
{
    if (fileName != currentFileName_)
    {
        statusBar()->showMessage(QString::fromStdString("Playing: " + fileName), 1000);
        currentFileName_ = fileName;
    }
}

void MainWindow::createConnections_()
{
    connect(ui_->actionPreferences,     SIGNAL(triggered()), this, SLOT(preferences()));
    connect(ui_->actionAbout_Eve,       SIGNAL(triggered()), this, SLOT(about()));
    connect(ui_->actionNew_Experiment,  SIGNAL(triggered()), this, SLOT(newExperiment()));    
    connect(ui_->actionOpen_Experiment, SIGNAL(triggered()), this, SLOT(openExperiment()));
    connect(ui_->actionQuit,            SIGNAL(triggered()), this, SLOT(quit()));
}

void MainWindow::quit()
{
    exit(0);
}

void MainWindow::preferences()
{
    qDebug() << "Preferences: none yet!";
}

void MainWindow::about()
{
    QMessageBox::about( this, tr("About Eve"),
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
        experiment_ = Experiment::getInstance();
        experiment_->init(fileName, true);
        experiment_->show();
        connect(experiment_, SIGNAL(experimentConfigured()), this, SLOT(init()));
    }
}

void MainWindow::openExperiment()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open Experiment"), 
        tr("SQLite databases (*.db, *.sqlite, *.sqlite3)"));

    if (!fileName.isEmpty())
    {
        experiment_ = Experiment::getInstance();
        experiment_->init(fileName, false);
        experiment_->openCollectionFile();
        experiment_->show();
        connect(experiment_, SIGNAL(experimentConfigured()), this, SLOT(init()));
    }
}
