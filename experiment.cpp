#include "experiment.h"
#include "ui_experiment.h"

Experiment::Experiment(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::Experiment)
{
    ui_->setupUi(this);
    createConnections_();
}

Experiment::~Experiment()
{
    db_.close();
    delete &db_;
    delete ui_;
}

QSqlDatabase Experiment::getDb()
{
    return db_;
}

void Experiment::init(QString fileName)
{
    db_ = QSqlDatabase::addDatabase("QSQLITE", "Main");
    db_.setDatabaseName(fileName);

    
    if(!db_.open()) {
        qDebug() << db_.lastError();
        exit(-1);
    }
    
    QSqlQuery query(db_);

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

    for (QStringList::const_iterator iter = ddl.constBegin(); iter != ddl.constEnd(); ++iter)
    {
        if(!query.exec(*iter))
        {    
            qDebug() << query.lastError();
            exit(-1);
        }
    }    

    populateTagsTable_();
}

void Experiment::populateTagsTable_() 
{
    model_ = new QSqlTableModel(this, QSqlDatabase::database("Main"));
    model_->setEditStrategy(QSqlTableModel::OnFieldChange);
    model_->setTable("Tags");
    model_->select();

    table_ = ui_->editTagsView;
    table_->setModel(model_);
    table_->hideColumn(0);
}

void Experiment::addTag() 
{
    int row = model_->rowCount();
    model_->insertRow(row);
}

void Experiment::removeTag()
{
    QModelIndex index = table_->currentIndex();
    model_->removeRows(index.row(), 1);
}


void Experiment::createConnections_() 
{
    connect(ui_->addTagPushButton, SIGNAL(clicked()), this, SLOT(addTag()));
    connect(ui_->removeTagPushButton, SIGNAL(clicked()), this, SLOT(removeTag()));
    connect(ui_->donePushButton, SIGNAL(clicked()), this, SLOT(close()));
}
