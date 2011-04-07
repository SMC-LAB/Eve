#include "ui_transport.h"
#include "transport.h"
#include <QtDebug>
#include <QFileDialog>
#include <QTableWidgetSelectionRange>

#define START_POS 0
#define START_GAIN 50
#define UPDATE_FREQ 250
#define SAMPLES_TO_TICKS(pos_, size_) ((int) ((100.0f * (pos_)) / (size_)))
#define SAMPLES_TO_SECS(pos_, freq_) ((int) ((pos_) / (freq_)))

Transport::Transport(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::Transport)
{
    ui_->setupUi(this);
    init_();
    createConnections_();
}

Transport::~Transport()
{
    delete ui_;
    delete backend_;
    delete mwr_;
    delete timer_;
}

void Transport::init_() 
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

    hasDataPtr_          = mwr_->getctrl("SoundFileSource/src/mrs_bool/hasData");
    
    activePtr_           = mwr_->getctrl("mrs_bool/active");
    
    numFilesPtr_         = mwr_->getctrl("SoundFileSource/src/mrs_natural/numFiles");
    allfilenamesPtr_     = mwr_->getctrl("SoundFileSource/src/mrs_string/allfilenames");
    currentlyPlayingPtr_ = mwr_->getctrl("SoundFileSource/src/mrs_string/currentlyPlaying");

    nLabelsPtr_          = mwr_->getctrl("SoundFileSource/src/mrs_natural/nLabels");
    labelNamesPtr_       = mwr_->getctrl("SoundFileSource/src/mrs_string/labelNames");
    currentLabelPtr_     = mwr_->getctrl("SoundFileSource/src/mrs_natural/currentLabel");

    advancePtr_          = mwr_->getctrl("SoundFileSource/src/mrs_natural/advance");
    cindexPtr_           = mwr_->getctrl("SoundFileSource/src/mrs_natural/cindex");
}

void Transport::createConnections_()
{
    connect(ui_->playButton,     SIGNAL(clicked()),        this, SLOT(play()));
    connect(ui_->previousButton, SIGNAL(clicked()),        this, SLOT(previous()));
    connect(ui_->nextButton,     SIGNAL(clicked()),        this, SLOT(next()));
    connect(ui_->gainSlider,     SIGNAL(sliderMoved(int)), this, SLOT(setGain(int)));
    connect(ui_->posSlider,      SIGNAL(sliderMoved(int)), this, SLOT(setPos(int)));

    connect(this, SIGNAL(sliderChanged(int, QSlider*)),           this, SLOT(moveSlider(int, QSlider*)));
    connect(this, SIGNAL(timeChanged(int, QTimeEdit*)),           this, SLOT(setTime(int, QTimeEdit*)));
    connect(this, SIGNAL(fileChanged(mrs_string, QTableWidget*)), this, SLOT(setCurrentFile(mrs_string, QTableWidget*)));

    connect(timer_, SIGNAL(timeout()), this, SLOT(update()));
}

void Transport::open()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
    {
        mwr_->updctrl(filenamePtr_, (fileName.toUtf8().constData()));
        mwr_->updctrl(initAudioPtr_, true);

        setPos(START_POS);
        setGain(START_GAIN);
        initPlayTable();
 
        mwr_->start();

        timer_->start(UPDATE_FREQ);
    }
}

void Transport::close()
{
    mwr_->exit();
    timer_->stop();
}

void Transport::play()
{
    if (activePtr_->to<mrs_bool>()){
        mwr_->pause();
        ui_->playButton->setText("&Play");        
    }
    else
    {
        mwr_->play();
        ui_->playButton->setText("&Pause");
    }
    
}

void Transport::next()
{
    cout << cindexPtr_->to<mrs_natural>() << endl;
    cout << numFilesPtr_->to<mrs_natural>() << endl;

    setPos(START_POS);
    mwr_->updctrl(advancePtr_, 1);
}

void Transport::previous()
{
    setPos(START_POS);
    mwr_->updctrl(advancePtr_, -1);
}

void Transport::quit()
{
    close();
    exit(0);
}

void Transport::update()
{
    if (!hasDataPtr_->to<mrs_bool>())
        close();
    
    mrs_string file = currentlyPlayingPtr_->to<mrs_string>();
    mrs_natural pos = posPtr_->to<mrs_natural>();
    mrs_natural size = sizePtr_->to<mrs_natural>();
    mrs_real freq = osratePtr_->to<mrs_real>();

    //qDebug() << pos << endl << size << endl << freq; // FIXME: size does not get updated until MarSystemQtWrapper::pause() is called

    int val = SAMPLES_TO_TICKS(pos, size);
    int secs = SAMPLES_TO_SECS(pos, freq);

    emit sliderChanged(val, ui_->posSlider);
    emit timeChanged(secs, ui_->posTime);
    emit fileChanged(file, ui_->playTable);
}


void Transport::setPos(int val)
{
    float fval = val / 100.0f;
    int pos = (int) sizePtr_->to<mrs_natural>() * fval;
    mwr_->updctrl(posPtr_, pos);
    emit sliderChanged(val, ui_->posSlider);
}

void Transport::setGain(int val)
{
    float fval = val / 100.0f;
    mwr_->updctrl(gainPtr_, fval);
    emit sliderChanged(val, ui_->gainSlider);
}

void Transport::moveSlider(int val, QSlider *slider) 
{
    slider->setValue(val);
}

void Transport::setTime(int val, QTimeEdit *time)
{
    QTime current(0, 0, 0, 0);
    current = current.addSecs(val);    
    time->setTime(current);
}

void Transport::initPlayTable()
{
    mrs_string files = allfilenamesPtr_->to<mrs_string>();
    mrs_natural nfiles = numFilesPtr_->to<mrs_natural>();
    mrs_string labels = labelNamesPtr_->to<mrs_string>();

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
        }

        table->setRangeSelected(QTableWidgetSelectionRange(0, 0, 0, 0), true);
    }
}

void Transport::setCurrentFile(mrs_string file, QTableWidget *table)
{
    table->clearSelection();
    table->findItems(QString::fromStdString(file), Qt::MatchExactly).at(0)->setSelected(true);   
}
