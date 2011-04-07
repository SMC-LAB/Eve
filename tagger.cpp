#include "tagger.h"
#include "ui_tagger.h"
#include <QtSql>

Tagger::Tagger(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::Tagger)
{
    ui_->setupUi(this);
    init_();
}

Tagger::~Tagger()
{
    delete ui_;
}

void Tagger::init_()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/Users/psilva/Desktop/QtSimplePlayer.db");
    if(!db.open())
        exit(-1);

    QSqlTableModel model;
    model.setTable("Tags");
    model.select();

    // for (int i = 0; i < model.rowCount(); ++i) {
    //     QString name = model.record(i).value("Name").toString();
    //     int min = model.record(i).value("MinimumValue").toInt();
    //     int max = model.record(i).value("MaximumValue").toInt();
    //     QString description = model.record(i).value("Description").toString();
        
    //     qDebug() << name << endl
    //              << min << endl
    //              << max << endl
    //              << description;
    // }

    QTableView *table = ui_->tagTable;
    table->setModel(&model);
}

