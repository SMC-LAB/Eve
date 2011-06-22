#include "ui_transport.h"
#include "transport.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QDebug>
#include <QtSql>
#include <QHeaderView>

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
    close();
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
    playOnce_            = false;
    currentFile_         = "";
    
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
    connect(ui_->playButton,     SIGNAL(clicked()),        this, SLOT(togglePlay()));
    connect(ui_->previousButton, SIGNAL(clicked()),        this, SLOT(previous()));
    connect(ui_->nextButton,     SIGNAL(clicked()),        this, SLOT(next()));
    connect(ui_->gainSlider,     SIGNAL(sliderMoved(int)), this, SLOT(setGain(int)));
    connect(ui_->posSlider,      SIGNAL(sliderMoved(int)), this, SLOT(setPos(int)));

    connect(this, SIGNAL(sliderChanged(int, QSlider*)),         this, SLOT(moveSlider(int, QSlider*)));
    connect(this, SIGNAL(timeChanged(int, QTimeEdit*)),         this, SLOT(setTime(int, QTimeEdit*)));
    connect(this, SIGNAL(fileChanged(mrs_string, QTableView*)), this, SLOT(setCurrentFile(mrs_string, QTableView*)));

    connect(timer_, SIGNAL(timeout()), this, SLOT(update()));
}

string Transport::getCollectionFile()
{
    return filenamePtr_->to<mrs_string>();
}

void Transport::open()
{
    mrs_string fileName = filenamePtr_->to<mrs_string>();
    
    if (QString::fromStdString(fileName).isEmpty())
    {
        Transport::open(QFileDialog::getOpenFileName(this));
    }
}

void Transport::open(QString fileName)
{
    if (!fileName.isEmpty() && QFile(fileName).exists())
    {
        mwr_->updctrl(filenamePtr_, (fileName.toStdString()));
        mwr_->updctrl(initAudioPtr_, true);

        setPos(START_POS);
        setGain(START_GAIN);

        initPlayTable_(fileName);
        
        mwr_->start();
        timer_->start(UPDATE_FREQ);
    }
}

void Transport::close()
{
    mwr_->pause();
    setPos(START_POS);
    mwr_->exit();
    timer_->stop();
}

void Transport::togglePlay()
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

void Transport::play()
{
    mwr_->play();
    ui_->playButton->setText("&Pause");
}

void Transport::pause()
{
    if (activePtr_->to<mrs_bool>()){
        mwr_->pause();
        ui_->playButton->setText("&Play");        
    }
}

void Transport::playOnce() 
{
    playOnce_ = true;
    play();
}


void Transport::next()
{
    setPos(START_POS);
    mwr_->updctrl(advancePtr_, 1);
}

void Transport::previous()
{
    setPos(START_POS);
    mwr_->updctrl(advancePtr_, -1);
}

void Transport::update()
{
    if (!hasDataPtr_->to<mrs_bool>())
    {
        close();
    }

    mrs_string file = currentlyPlayingPtr_->to<mrs_string>();

    mrs_natural pos = posPtr_->to<mrs_natural>();
    mrs_natural size = sizePtr_->to<mrs_natural>();
    mrs_real freq = osratePtr_->to<mrs_real>();
    
    // FIXME: size does not get updated until MarSystemQtWrapper::pause() is called
    // qDebug() << pos << endl << size << endl << freq;

    int val = SAMPLES_TO_TICKS(pos, size);
    int secs = SAMPLES_TO_SECS(pos, freq);

    emit sliderChanged(val, ui_->posSlider);
    emit timeChanged(secs, ui_->posTime);

    if (currentFile_ != file)
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

void Transport::initPlayTable_(QString fileName)
{
    QSqlDatabase db_ = QSqlDatabase::database("Main");    

    QSqlQuery getCollectionFile("SELECT CollectionFile FROM Metadata;", db_);
    getCollectionFile.next();

    if (getCollectionFile.value(0).toString().isEmpty()) {
        populateDb_(db_, true);

        QSqlQuery *setCollectionFile = new QSqlQuery(db_);
        setCollectionFile->prepare("UPDATE Metadata SET CollectionFile=:CollectionFile;");
        setCollectionFile->bindValue(":CollectionFile", fileName);

        if (!setCollectionFile->exec()){
            qDebug() << setCollectionFile->lastError() << __func__;
            exit(-1);
        }
    }
    else
    {
        populateDb_(db_, false);    
    }
    
    stimuli_model_ = new QSqlRelationalTableModel(this, db_);
    stimuli_model_->setEditStrategy(QSqlTableModel::OnFieldChange);
    stimuli_model_->setTable("Stimuli");
    stimuli_model_->select();
    stimuli_model_->setRelation(5, QSqlRelation("Annotations", "ID", "ID"));
    stimuli_model_->setRelation(6, QSqlRelation("Descriptors", "ID", "ID"));

    stimuli_table_ = ui_->playTable;

    QHeaderView *headerView = stimuli_table_->horizontalHeader(); 
    headerView->setResizeMode(QHeaderView::Stretch);
    headerView->setResizeMode(1, QHeaderView::Interactive);
    stimuli_table_->setHorizontalHeader(headerView);
    
    stimuli_table_->setModel(stimuli_model_);
    stimuli_table_->setItemDelegate(new QSqlRelationalDelegate(stimuli_table_));
    stimuli_table_->hideColumn(0);
    stimuli_table_->hideColumn(2);
    stimuli_table_->hideColumn(5);
    stimuli_table_->hideColumn(6);
}

void Transport::populateDb_(QSqlDatabase db_, bool load)
{
    vector<soundFile> collectionFileDetails = backend_->getSoundFileInfo();

    QSqlQuery countStimuli("SELECT COUNT(ID) FROM Stimuli;", db_);
    countStimuli.next();
    int row_offset = countStimuli.value(0).toInt();

    for (vector<soundFile>::iterator iter = collectionFileDetails.begin(); iter != collectionFileDetails.end(); ++iter)
    {
        int row = 0;
        
        if (load)
        {
            row = iter - collectionFileDetails.begin() + 1 + row_offset;

            QFileInfo fi(QString::fromStdString((*iter).file));
        
            QSqlQuery *stimuliQuery = new QSqlQuery(db_);
            stimuliQuery->prepare("INSERT INTO Stimuli(ID, Name, Path, Duration, Tagged) VALUES(:ID, :Name, :Path, :Duration, :Tagged);");
            stimuliQuery->bindValue(":ID", row);
            stimuliQuery->bindValue(":Name", fi.fileName());
            stimuliQuery->bindValue(":Path", QString::fromStdString((*iter).file));
            stimuliQuery->bindValue(":Duration", QString::number((*iter).size / (*iter).srate));
            stimuliQuery->bindValue(":Tagged", false);

            if (!stimuliQuery->exec()){
                qDebug() << stimuliQuery->lastError() << stimuliQuery->lastQuery() << __func__;
                exit(-1);
            }        
        }
        else 
        {
            row = iter - collectionFileDetails.begin();
        }
        collectionFilesMap_[(*iter).file] = row;
    }
}

void Transport::setCurrentFile(mrs_string file, QTableView *table)
{

   if (playOnce_) {
        playOnce_ = false;
        mwr_->pause();
    }

    currentFile_ = file;
    int row = collectionFilesMap_[file];
    table->selectRow(row);
}

int Transport::getCurrentFileId()
{
    mrs_string currentlyPlaying = currentlyPlayingPtr_->to<mrs_string>();

    QSqlDatabase db_ = QSqlDatabase::database("Main");
    QSqlQuery *idQuery = new QSqlQuery("SELECT ID FROM Stimuli WHERE Path=:Path", db_);
    idQuery->bindValue(":Path", QString::fromStdString(currentlyPlaying));
    idQuery->exec();
    idQuery->next();

    return idQuery->value(0).toInt();
}

QString Transport::getCurrentFile()
{    
    mrs_string currentlyPlaying = currentlyPlayingPtr_->to<mrs_string>();
    return QString::fromStdString(currentlyPlaying);
}
