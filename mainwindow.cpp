#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);
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
}
