#include "ui_experiment.h"
#include "experiment.h"
#include "util.h"
#include <QFileDialog>

Experiment::Experiment(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::Experiment)
{
    ui_->setupUi(this);
    db_ = QSqlDatabase();
}

Experiment::~Experiment()
{
    delete transport_;
    delete tagger_;
    delete ui_;
    db_.close();
}

void Experiment::createConnections_() 
{
    connect(ui_->addTagPushButton,      SIGNAL(clicked()), tagger_, SLOT(addTag()));
    connect(ui_->removeTagPushButton,   SIGNAL(clicked()), tagger_, SLOT(removeTag()));

    connect(ui_->addSubjectPushButton,    SIGNAL(clicked()), this, SLOT(addSubject()));
    connect(ui_->removeSubjectPushButton, SIGNAL(clicked()), this, SLOT(removeSubject()));

    connect(ui_->doneTagPushButton,     SIGNAL(clicked()), this, SLOT(close()));
    connect(ui_->doneStimuliPushButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui_->doneSubjectPushButton, SIGNAL(clicked()), this, SLOT(close()));

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

void Experiment::init(QString fileName, bool overwrite)
{
    if (!db_.isValid())
    {
        db_ = QSqlDatabase::addDatabase("QSQLITE", "Main");
        db_.setDatabaseName(fileName);
    }

    if(!db_.open()) {
        qDebug() << db_.lastError() << __func__;
        exit(-1);
    }

    if (overwrite || !QFile(fileName).exists() || !QFile(fileName).size()) {

        QStringList ddl;

        ddl << "CREATE TABLE Subjects ( "
            "    ID          INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "    Name        TEXT,"
            "    Age         INTEGER,"
            "    Paid        BOOLEAN,"
            "    Gender      CHAR,"
            "    Nonmusician BOOLEAN,"
            "    Country     TEXT,"
            "    Active      BOOLEAN"
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
                qDebug() << query.lastError() << __func__;
                exit(-1);
            }
        }
    }

    populateTagsTable_();
    populateStimuliTable_();
    populateSubjectsTable_();
    createConnections_();    
}

void Experiment::updateNote(QString note)
{
    QSqlQuery *setTagNote = new QSqlQuery(db_);

    setTagNote->prepare("UPDATE Experiments SET Note=:Note WHERE SubjectID=:SubjectID AND StimuliID=:StimuliID;");
    setTagNote->bindValue(":SubjectID", getCurrentSubjectId());
    setTagNote->bindValue(":StimuliID", transport_->getCurrentFileId());

    if (!setTagNote->exec())
    {
        qDebug() << setTagNote->lastError() << __func__;
        exit(-1);
    }
}

void Experiment::updateValue(QString tag, int rating, QString note)
{
    QSqlQuery *setTagRating = new QSqlQuery(db_);

    if (rowExists(tag))
    {
        setTagRating->prepare("UPDATE Experiments SET Rating=:Rating, Note=:Note WHERE SubjectID=:SubjectID AND StimuliID=:StimuliID AND Tag=:Tag;");
    }
    else
    {
        setTagRating->prepare("INSERT INTO Experiments(SubjectID, StimuliID, Tag, Rating, Note) VALUES(:SubjectID, :StimuliID, :Tag, :Rating, :Note);");
    }

    setTagRating->bindValue(":SubjectID", getCurrentSubjectId());
    setTagRating->bindValue(":StimuliID", transport_->getCurrentFileId());
    setTagRating->bindValue(":Tag", tag);
    setTagRating->bindValue(":Rating", rating);
    setTagRating->bindValue(":Note", note);

    if (!setTagRating->exec())
    {
        qDebug() << setTagRating->lastError() << __func__;
        exit(-1);
    }
}

bool Experiment::rowExists(QString tag)
{
    QSqlQuery *getRow = new QSqlQuery(db_);
    getRow->prepare("SELECT Rating FROM Experiments WHERE SubjectID=:SubjectID AND StimuliID=:StimuliID AND Tag=:Tag;");
    getRow->bindValue(":SubjectID", getCurrentSubjectId());
    getRow->bindValue(":StimuliID", transport_->getCurrentFileId());
    getRow->bindValue(":Tag", tag);

    if (!getRow->exec())
    {
        qDebug() << getRow->lastError() << __func__ << getRow->lastQuery();
        exit(-1);
    }

    if (getRow->next())
    {
        return true;
    }
}

int Experiment::getValue(QString tag)
{
    QSqlQuery *getRating = new QSqlQuery(db_);
    getRating->prepare("SELECT Rating FROM Experiments WHERE SubjectID=:SubjectID AND StimuliID=:StimuliID AND Tag=:Tag;");
    getRating->bindValue(":SubjectID", getCurrentSubjectId());
    getRating->bindValue(":StimuliID", transport_->getCurrentFileId());
    getRating->bindValue(":Tag", tag);

    if (!getRating->exec())
    {
        qDebug() << getRating->lastError() << getRating->lastQuery() << __func__;
        exit(-1);
    }

    if (getRating->next())
    {
        return getRating->value(0).toInt();
    }
}

void Experiment::populateTagsTable_() 
{
    tagger_ = Tagger::getInstance();
    tagger_->initTagTable();
    ui_->verticalLayout_2->addWidget(tagger_);
}

void Experiment::populateStimuliTable_() 
{
    transport_ = Transport::getInstance();
    ui_->verticalLayout->addWidget(transport_);
}

void Experiment::populateSubjectsTable_()
{
    subjects_model_ = new QSqlRelationalTableModel(this, db_);
    subjects_model_->setEditStrategy(QSqlTableModel::OnFieldChange);
    subjects_model_->setTable("Subjects");
    subjects_model_->select();

    subjects_table_ = new QTableView();
    subjects_table_->setModel(subjects_model_);
    subjects_table_->setItemDelegate(new QSqlRelationalDelegate(subjects_table_));

    QHeaderView *headerView = subjects_table_->horizontalHeader(); 
    headerView->setResizeMode(QHeaderView::Stretch);
    headerView->setResizeMode(1, QHeaderView::Interactive);
    subjects_table_->setHorizontalHeader(headerView);
    subjects_table_->hideColumn(0);

    Util::removeLayoutChildren(ui_->verticalLayout_3, 1);
    ui_->verticalLayout_3->addWidget(subjects_table_);
}

int Experiment::getCurrentSubjectId()
{
    QSqlQuery *idQuery = new QSqlQuery("SELECT ID FROM Subjects WHERE Active='true'", db_);
    idQuery->exec();
    idQuery->next();

    return idQuery->value(0).toInt();
}

map<QString, QString> Experiment::getCurrentSubject()
{
    QSqlQuery *subjectQuery = new QSqlQuery("SELECT * FROM Subjects WHERE ID=:ID", db_);
    subjectQuery->bindValue(":ID", getCurrentSubjectId());
    subjectQuery->exec();
    subjectQuery->next();

    QSqlRecord subjectRecord = subjectQuery->record();

    map<QString, QString> subject;

    for (int i = 0; i < subjectRecord.count(); ++i)
    {        
        QString column = subjectRecord.fieldName(i);
        subject[column] = subjectQuery->value(i).toString();
    }
    
    return subject;
}

void Experiment::addSubject() 
{
    int row = subjects_model_->rowCount();
    subjects_model_->insertRow(row);
}

void Experiment::addSubject(QString name, int age, bool paid, QString gender, bool nonmusician, QString country, bool active)
{
    QSqlQuery *insertSubject = new QSqlQuery(db_);
    insertSubject->prepare("INSERT INTO Subjects(Name, Age, Paid, Gender, Nonmusician, Country, Active) VALUES(:Name, :Age, :Paid, :Gender, :Nonmusician, :Country, :Active)");
    
}

void Experiment::removeSubject()
{
    QModelIndex index = subjects_table_->currentIndex();
    subjects_model_->removeRows(index.row(), 1);
}

void Experiment::close()
{
    ui_->verticalLayout->removeWidget(transport_);
    ui_->verticalLayout_2->removeWidget(tagger_);
    this->hide();
    emit experimentConfigured();
}

