#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    createConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete backend;
    delete mwr;
}

void MainWindow::init() 
{
    backend = new SimplePlayerBackend();
    mwr = new MarsyasQt::MarSystemQtWrapper(backend->getPlaybacknet());
}

void MainWindow::createConnections()
{
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(play()));
    connect(ui->pauseButton, SIGNAL(clicked()), this, SLOT(pause()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    backend->open(fileName.toUtf8().constData());
    mwr->start();
}

void MainWindow::close()
{
    cout << "MainWindow: Close" << endl;
    backend->close();
    mwr->quit();
}


void MainWindow::play()
{
    cout << "MainWindow: Play" << endl;
    mwr->play();
}

void MainWindow::pause()
{
    cout << "MainWindow: Pause" << endl;
    mwr->pause();
}

void MainWindow::quit()
{
    cout << "MainWindow: Quit" << endl;
    exit(0);
}
