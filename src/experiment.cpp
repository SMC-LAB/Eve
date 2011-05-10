#include "experiment.h"
#include "ui_experiment.h"
#include <QFileDialog>

Experiment::Experiment(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::Experiment)
{
    ui_->setupUi(this);
}

Experiment::~Experiment()
{
    db_.close();
    delete &db_;
    delete ui_;
    delete transport_;
}

void Experiment::createConnections_() 
{
    connect(ui_->addTagPushButton,         SIGNAL(clicked()), tagger_, SLOT(addTag()));
    connect(ui_->removeTagPushButton,      SIGNAL(clicked()), tagger_, SLOT(removeTag()));
    connect(ui_->donePushButton,           SIGNAL(clicked()), this, SLOT(close()));
    connect(ui_->donePushButton_2,         SIGNAL(clicked()), this, SLOT(close()));
    connect(ui_->openCollectionFileButton, SIGNAL(clicked()), this, SLOT(openCollectionFile()));

    connect(tagger_, SIGNAL(updatedValue(QString, int, QString)), this, SLOT(updateValue(QString, int, QString)));
}

void Experiment::openCollectionFile() 
{
    QSqlQuery getCollectionFile("SELECT CollectionFile FROM Metadata;", db_);
    getCollectionFile.next();    

    QString fileName = getCollectionFile.value(0).toString();
   
    if (fileName.isEmpty())
    {
        fileName = ui_->collectionFileLineEdit->text();

        if (fileName.isEmpty() || fileName == QString::fromStdString(transport_->getCollectionFile()))
        {
            fileName = QFileDialog::getOpenFileName(
                this,
                tr("Open Collection File"), 
                tr("Marsyas Collection Files (*.mf)"));

            if (!fileName.isEmpty())
            {    
                ui_->collectionFileLineEdit->setText(fileName);
            }
        }

        // TODO: this should be optional, based on preferences.
        QSqlQuery *emptyTable = new QSqlQuery("DELETE * FROM Stimuli;", db_);
        emptyTable->exec();
    }

    ui_->collectionFileLineEdit->setText(fileName);

    transport_->open(fileName);
}


QSqlDatabase Experiment::getDb()
{
    return db_;
}

Transport* Experiment::getTransport()
{
    return transport_;
}

Tagger* Experiment::getTagger()
{
    return tagger_;
}

void Experiment::init(QString fileName)
{
    if (!db_.isValid())
    {
        db_ = QSqlDatabase::addDatabase("QSQLITE", "Main");
        db_.setDatabaseName(fileName);
    }

    if(!db_.open()) {
        qDebug() << db_.lastError();
        exit(-1);
    }
    
    if (!QFile(fileName).exists() || !QFile(fileName).size()) {

        QStringList ddl;

        ddl << "CREATE TABLE Subjects ( "
            "    ID          INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "    Name        TEXT,"
            "    Age         INTEGER,"
            "    Paid        BOOLEAN,"
            "    Gender      CHAR,"
            "    Nonmusician BOOLEAN,"
            "    Country     TEXT "
            ");"
            << "CREATE TABLE Annotations ( "
            "    ID        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "    Genre     TEXT,"
            "    BPM       INTEGER,"
            "    Signature TEXT "
            ");"
            << "CREATE TABLE Descriptors ( "
            "    ID                 INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "    Beats              BLOB,"
            "    EventDensity       BLOB,"
            "    BeatSalience       BLOB,"
            "    FastMetricalLevels BLOB "
            ");"
            << "CREATE TABLE Tags ( "
            "    ID           INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "    Name         TEXT    NOT NULL,"
            "    MinimumValue INTEGER NOT NULL,"
            "    MaximumValue INTEGER NOT NULL,"
            "    Description  TEXT "
            ");"
            << "CREATE TABLE Stimuli ( "
            "    ID           INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "    Name         TEXT,"
            "    Path         TEXT    NOT NULL,"
            "    Duration     INT,"
            "    Tagged       BOOLEAN DEFAULT (0),"
            "    AnnotationID INTEGER REFERENCES Annotations ( ID ),"
            "    DescriptorID INTEGER REFERENCES Descriptors ( ID ) "
            ");"
            << "CREATE TABLE Experiments ( "
            "    ID        INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "    SubjectID INTEGER REFERENCES Subjects ( ID ),"
            "    StimuliID INTEGER REFERENCES Stimuli ( ID ),"
            "    Tag       INTEGER REFERENCES Tags ( ID ),"
            "    Rating    INTEGER,"
            "    Note      TEXT "
            ");"
            << "CREATE TABLE Metadata ("
            "    ID      INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "    Version TEXT,"
            "    CollectionFile TEXT"
            ");"
            << "INSERT INTO Metadata(ID, Version) VALUES(1, \"0.0.2\");";

        QSqlQuery query(db_);

        for (QStringList::const_iterator iter = ddl.constBegin(); iter != ddl.constEnd(); ++iter)
        {
            if(!query.exec(*iter))
            {    
                qDebug() << query.lastError();
                exit(-1);
            }
        }
    }

    populateTagsTable_();
    populateStimuliTable_();
    createConnections_();    
}

void Experiment::updateValue(QString tag, int rating, QString note = "")
{
    QSqlQuery updateQuery("UPDATE ", db_);
    
    QSqlQuery *setTagRating = new QSqlQuery(db_);
    setTagRating->prepare("INSERT INTO Experiments(StimuliID, Tag, Rating, Note) VALUES(:StimuliID, :Tag, :Rating, :Note);");
    setTagRating->bindValue(":StimuliID", transport_->getCurrentFileId());
    setTagRating->bindValue(":Tag", tag);
    setTagRating->bindValue(":Rating", rating);
    setTagRating->bindValue(":Note", note);

    if (!setTagRating->exec())
    {
        qDebug() << setTagRating->lastError();
        exit(-1);
    }
}

void Experiment::populateTagsTable_() 
{
    tagger_ = Tagger::getInstance();
    ui_->verticalLayout_2->addWidget(tagger_);
}

void Experiment::populateStimuliTable_() 
{
    transport_ = Transport::getInstance();
    ui_->verticalLayout->addWidget(transport_);
}

void Experiment::close()
{
    ui_->verticalLayout->removeWidget(transport_);
    ui_->verticalLayout_2->removeWidget(tagger_);
    this->hide();
    emit experimentConfigured();
}

