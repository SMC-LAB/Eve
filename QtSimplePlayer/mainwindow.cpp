#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QtDebug>
#include <QFileDialog>
//#include <QtGlobal>
#include <QTableWidgetSelectionRange>

#define STARTPOS 0
#define STARTGAIN 50
#define UPDATEFREQ 250
#define samplesToTicks(pos_, size_) ((int) ((100.0f * (pos_)) / (size_)))
#define samplesToSecs(pos_, freq_) ((int) ((pos_) / (freq_)))

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
    delete timer_;
}

void MainWindow::init_() 
{
    backend_             = new SimplePlayerBackend();
    mwr_                 = new MarsyasQt::MarSystemQtWrapper(backend_->getPlaybacknet());
    timer_               = new QTimer(this);

    gainPtr_             = mwr_->getctrl("Gain/gain/mrs_real/gain");
    initAudioPtr_        = mwr_->getctrl("AudioSink/dest/mrs_bool/initAudio");

    filenamePtr_         = mwr_->getctrl("SoundFileSource/src/mrs_string/filename");
    posPtr_              = mwr_->getctrl("SoundFileSource/src/mrs_natural/pos");  
    sizePtr_             = mwr_->getctrl("SoundFileSource/src/mrs_natural/size");  
    osratePtr_           = mwr_->getctrl("SoundFileSource/src/mrs_real/osrate");

    numFilesPtr_         = mwr_->getctrl("SoundFileSource/src/mrs_natural/numFiles");
    allfilenamesPtr_     = mwr_->getctrl("SoundFileSource/src/mrs_string/allfilenames");
    currentlyPlayingPtr_ = mwr_->getctrl("SoundFileSource/src/mrs_string/currentlyPlaying");

    nLabelsPtr_          = mwr_->getctrl("SoundFileSource/src/mrs_natural/nLabels");
    labelNamesPtr_       = mwr_->getctrl("SoundFileSource/src/mrs_string/labelNames");
    currentLabelPtr_     = mwr_->getctrl("SoundFileSource/src/mrs_natural/currentLabel");
}

void MainWindow::createConnections_()
{
    connect(ui_->playButton,  SIGNAL(clicked()),        this, SLOT(play()));
    connect(ui_->pauseButton, SIGNAL(clicked()),        this, SLOT(pause()));
    connect(ui_->actionOpen,  SIGNAL(triggered()),      this, SLOT(open()));
    connect(ui_->actionClose, SIGNAL(triggered()),      this, SLOT(close()));
    connect(ui_->actionQuit,  SIGNAL(triggered()),      this, SLOT(quit()));
    connect(ui_->gainSlider,  SIGNAL(sliderMoved(int)), this, SLOT(setGain(int)));
    connect(ui_->posSlider,   SIGNAL(sliderMoved(int)), this, SLOT(setPos(int)));

    connect(this, SIGNAL(sliderChanged(int, QSlider*)),           this, SLOT(moveSlider(int, QSlider*)));
    connect(this, SIGNAL(timeChanged(int, QTimeEdit*)),           this, SLOT(setTime(int, QTimeEdit*)));
    connect(this, SIGNAL(fileChanged(mrs_string, QTableWidget*)), this, SLOT(setCurrentFile(mrs_string, QTableWidget*)));

    connect(timer_, SIGNAL(timeout()), this, SLOT(update()));
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
    {
        qDebug() << "MainWindow: Opening " << fileName;

        mwr_->updctrl(filenamePtr_, (fileName.toUtf8().constData()));
        mwr_->updctrl(initAudioPtr_, true);

        setPos(STARTPOS);
        setGain(STARTGAIN);
        initPlayTable();
 
        mwr_->start();

        timer_->start(UPDATEFREQ);
    }
}

void MainWindow::close()
{
    qDebug() << "MainWindow: Close";
    mwr_->exit();
    timer_->stop();
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

void MainWindow::update()
{
    mrs_string file = currentlyPlayingPtr_->to<mrs_string>();
    mrs_natural pos = posPtr_->to<mrs_natural>();
    mrs_natural size = sizePtr_->to<mrs_natural>();
    mrs_real freq = osratePtr_->to<mrs_real>();

    //qDebug() << pos << endl << size << endl << freq;

    int val = samplesToTicks(pos, size);
    int secs = samplesToSecs(pos, freq);

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

void MainWindow::initPlayTable()
{
    mrs_string files = allfilenamesPtr_->to<mrs_string>();
    mrs_natural nfiles = numFilesPtr_->to<mrs_natural>();
    mrs_string labels = labelNamesPtr_->to<mrs_string>();
    //mrs_natural nlabels = nLabelsPtr_->to<mrs_natural>();

    //qDebug() << nfiles << " " << nlabels << " " << QString::fromStdString(labels);
    //Q_ASSERT(nfiles == nlabels);

    QTableWidget *table = ui_->playTable;

    if (nfiles == 1) {
        mrs_string file = filenamePtr_->to<mrs_string>();
        QTableWidgetItem *title = new QTableWidgetItem(QString::fromStdString(file));
        table->setItem(0, 0, title);
    }
    else {
        
        QStringList fileList = QString::fromStdString(files).split(",");
        QStringList labelList = QString::fromStdString(labels).split(",");

        table->setRowCount(nfiles);
        table->setColumnCount(1);

        for (int i = 0; i < fileList.size(); ++i) {
        
            QTableWidgetItem *title = new QTableWidgetItem(fileList.at(i));
            table->setItem(i, 0, title);

            //QTableWidgetItem *label = new QTableWidgetItem(labelList.at(i));
            //table->setItem(i, 1, title);
        }

        table->setRangeSelected(QTableWidgetSelectionRange(0, 0, 0, 0), true);
    }
}


void MainWindow::setCurrentFile(mrs_string file, QTableWidget *table)
{
    table->clearSelection();
    table->findItems(QString::fromStdString(file), Qt::MatchExactly).at(0)->setSelected(true);   
}
