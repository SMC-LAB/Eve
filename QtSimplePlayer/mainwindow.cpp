#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QtDebug>

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
    initPtr_ = mwr_->getctrl("AudioSink/dest/mrs_bool/initAudio");
    sizePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_natural/size");  
    freqPtr_ = mwr_->getctrl("SoundFileSource/src/mrs_real/osrate");
    curPtr_ = mwr_->getctrl("SoundFileSource/src/mrs_string/currentlyPlaying");
    labelNamesPtr_ = mwr_->getctrl("SoundFileSource/src/mrs_string/labelNames");
}

void MainWindow::createConnections_()
{
    connect(ui_->playButton, SIGNAL(clicked()), this, SLOT(play()));
    connect(ui_->pauseButton, SIGNAL(clicked()), this, SLOT(pause()));
    connect(ui_->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui_->actionQuit, SIGNAL(triggered()), this, SLOT(quit()));
    connect(ui_->gainSlider, SIGNAL(sliderMoved(int)), this, SLOT(setGain(int)));
    connect(ui_->posSlider, SIGNAL(sliderMoved(int)), this, SLOT(setPos(int)));
    connect(this, SIGNAL(sliderChanged(int, QSlider*)), this, SLOT(moveSlider(int, QSlider*)));
    connect(this, SIGNAL(timeChanged(int, QTimeEdit*)), this, SLOT(setTime(int, QTimeEdit*)));

    connect(this, SIGNAL(fileChanged(mrs_string, QTableWidget*)), this, SLOT(setCurrentFile(mrs_string, QTableWidget*)));
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
    {
        qDebug() << "MainWindow: Opening " << fileName.toUtf8().constData();
        mwr_->updctrl(filePtr_, (fileName.toUtf8().constData()));
        mwr_->updctrl(initPtr_, true);
        setPos(0);
        setGain(50);
        mwr_->start();

        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(setPos()));
        timer->start(100);
    }
}    

void MainWindow::close()
{
    qDebug() << "MainWindow: Close";
    mwr_->quit();
}

void MainWindow::play()
{
    qDebug() << "MainWindow: Play";
    mwr_->play();
}

void MainWindow::pause()
{
    qDebug() << "MainWindow: Pause";
    mwr_->pause();
}

void MainWindow::quit()
{
    qDebug() << "MainWindow: Quit";
    close();
    exit(0);
}

void MainWindow::setPos()
{
    mrs_string file = curPtr_->to<mrs_string>();
    mrs_natural pos = posPtr_->to<mrs_natural>();
    mrs_natural size = sizePtr_->to<mrs_natural>();
    mrs_real freq = freqPtr_->to<mrs_real>();

    qDebug() << pos << endl << size << endl << freq;

    int val = (int) (100.0f * pos) / size;
    int secs = (int) (pos / freq);

    emit sliderChanged(val, ui_->posSlider);
    emit timeChanged(secs, ui_->posTime);
    emit fileChanged(file, ui_->playTable);
}


void MainWindow::setPos(int val)
{
    qDebug() << "MainWindow: set pos at " << val;
    float fval = val / 100.0f;
    int pos = (int) sizePtr_->to<mrs_natural>() * fval;
    mwr_->updctrl(posPtr_, pos);
    emit sliderChanged(val, ui_->posSlider);
}

void MainWindow::setGain(int val)
{
    qDebug() << "MainWindow: set gain at " << val;
    float fval = val / 100.0f;
    mwr_->updctrl(gainPtr_, fval);
    emit sliderChanged(val, ui_->gainSlider);
}

void MainWindow::moveSlider(int val, QSlider *slider) 
{
    slider->setValue(val);
}

void MainWindow::setTime(int val, QTimeEdit *time)
{
    QTime current(0, 0, 0, 0);
    current = current.addSecs(val);    
    time->setTime(current);
}

void MainWindow::setCurrentFile(mrs_string file, QTableWidget *table)
{

    QTableWidgetItem *title = new QTableWidgetItem(QString::fromStdString(file));
    table->setItem(0, 0, title);
}
