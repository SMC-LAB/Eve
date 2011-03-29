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
    initPtr_ = mwr_->getctrl("AudioSink/dest/mrs_bool/initAudio");
    sizePtr_ = mwr_->getctrl("SoundFileSource/src/mrs_natural/size");  
    freqPtr_ = mwr_->getctrl("SoundFileSource/src/mrs_real/osrate");
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
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
    {
        cout << "MainWindow: Opening " << fileName.toUtf8().constData() << endl;
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

void MainWindow::quit()
{
    cout << "MainWindow: Quit" << endl;
    exit(0);
}

void MainWindow::setPos()
{
    mrs_natural pos = posPtr_->to<mrs_natural>();
    mrs_natural size = sizePtr_->to<mrs_natural>();
    mrs_real freq = freqPtr_->to<mrs_real>();

    int val = (int) (100.0f * pos) / size;
    int secs = (int) (pos / freq);

    emit sliderChanged(val, ui_->posSlider);
    emit timeChanged(secs, ui_->posTime);
}


void MainWindow::setPos(int val)
{
    cout << "MainWindow: set pos at " << val << endl;
    float fval = val / 100.0f;
    int pos = (int) sizePtr_->to<mrs_natural>() * fval;
    mwr_->updctrl(posPtr_, pos);
    emit sliderChanged(val, ui_->posSlider);
}

void MainWindow::setGain(int val)
{
    cout << "MainWindow: set gain at " << val << endl;
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
