#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QFileDialog>

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
    delete backend_;
    delete mwr_;
}

void MainWindow::init_() 
{
    backend_ = new SimplePlayerBackend();
    mwr_ = new MarsyasQt::MarSystemQtWrapper(backend_->getPlaybacknet());
    filePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_string/filename");
    gainPtr_ = mwr_->getctrl("Gain/gain/mrs_real/gain");
    posPtr_ = mwr_->getctrl("SoundFileSource/src/mrs_natural/pos");  
}

void MainWindow::createConnections_()
{
    connect(ui_->playButton, SIGNAL(clicked()), this, SLOT(play()));
    connect(ui_->pauseButton, SIGNAL(clicked()), this, SLOT(pause()));
    connect(ui_->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui_->actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(ui_->gainSlider, SIGNAL(valueChanged(int)), this, SLOT(setGain(int)));
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
    {
        cout << "MainWindow: Opening " << fileName.toUtf8().constData() << endl;
        mwr_->updctrl(filePtr_, (fileName.toUtf8().constData()));
        mwr_->updctrl(posPtr_, 0);
        mwr_->start();
    }
}

void MainWindow::close()
{
    cout << "MainWindow: Close" << endl;
    backend_->close();
    mwr_->quit();
}


void MainWindow::play()
{
    cout << "MainWindow: Play" << endl;
    mwr_->play();
}

void MainWindow::pause()
{
    cout << "MainWindow: Pause" << endl;
    mwr_->pause();
}

void MainWindow::setGain(int val)
{
    cout << "MainWindow: set gain at " << val << endl;
    float fval = val / 100.0f;
    mwr_->updctrl(gainPtr_, fval);
}

void MainWindow::quit()
{
    cout << "MainWindow: Quit" << endl;
    exit(0);
}
