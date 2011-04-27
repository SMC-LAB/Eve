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
    connect(ui_->addTagPushButton, SIGNAL(clicked()), this, SLOT(addTag()));
    connect(ui_->removeTagPushButton, SIGNAL(clicked()), this, SLOT(removeTag()));
    connect(ui_->donePushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui_->donePushButton_2, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui_->openCollectionFileButton, SIGNAL(clicked()), this, SLOT(openCollectionFile()));
}

void Experiment::openCollectionFile() 
{
    QString fileName = ui_->collectionFileLineEdit->text();
    if (fileName.isEmpty() || fileName == QString::fromStdString(transport_->getCollectionFile()))
    {
        fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
        {    
            ui_->collectionFileLineEdit->setText(fileName);
        }
    }
    
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

void Experiment::init(QString fileName)
{
    db_ = QSqlDatabase::addDatabase("QSQLITE", "Main");
    db_.setDatabaseName(fileName);

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
            "    Version TEXT NOT NULL"
            ");"
            << "INSERT INTO Metadata VALUES(1, \"0.0.1\");";

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

void Experiment::populateTagsTable_() 
{
    tags_model_ = new QSqlTableModel(this, QSqlDatabase::database("Main"));
    tags_model_->setEditStrategy(QSqlTableModel::OnFieldChange);
    tags_model_->setTable("Tags");
    tags_model_->select();

    tags_table_ = ui_->editTagsView;
    tags_table_->setModel(tags_model_);
    tags_table_->hideColumn(0);
}

void Experiment::populateStimuliTable_() 
{
    transport_ = new Transport();
    ui_->verticalLayout->addWidget(transport_);
}

void Experiment::addTag() 
{
    int row = tags_model_->rowCount();
    tags_model_->insertRow(row);
}

void Experiment::removeTag()
{
    QModelIndex index = tags_table_->currentIndex();
    tags_model_->removeRows(index.row(), 1);
}
