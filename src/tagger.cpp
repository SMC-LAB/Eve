#include "ui_tagger.h"
#include "tagger.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

Tagger::Tagger(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::Tagger)
{
    ui_->setupUi(this);
    initTagTable_();
}

Tagger::~Tagger()
{
    delete ui_;
}

void Tagger::initTagTable_()
{
    QSqlDatabase db_ = QSqlDatabase::database("Main");    

    tags_model_ = new QSqlRelationalTableModel(this, db_);
    tags_model_->setEditStrategy(QSqlTableModel::OnFieldChange);
    tags_model_->setTable("Tags");
    tags_model_->select();

    tags_table_ = ui_->tagTable;
    tags_table_->setModel(tags_model_);
    tags_table_->setItemDelegate(new QSqlRelationalDelegate(tags_table_));
}

void Tagger::setCustomDelegate()
{
    initTagWidget_();
}

void Tagger::initTagWidget_()
{
    QSqlDatabase db_ = QSqlDatabase::database("Main");    
    QSqlQuery getTags("SELECT * FROM Tags;", db_);

    QVBoxLayout *vlayout = new QVBoxLayout(this);

    while (getTags.next())
    {
        QString tagName = getTags.value(1).toString();
        QString tagDesc = getTags.value(4).toString();
        int tagMinVal = getTags.value(2).toInt();
        int tagMaxVal = getTags.value(3).toInt();
    
        QSlider *slider = new QSlider(Qt::Horizontal);
        slider->setMinimum(tagMinVal);
        slider->setMaximum(tagMaxVal);
        slider->setTickInterval(tagMaxVal - tagMinVal);

        QHBoxLayout *hlayout = new QHBoxLayout();

        hlayout->addWidget(new QLabel(QString::number(tagMinVal)));
        hlayout->addWidget(slider);
        hlayout->addWidget(new QLabel(QString::number(tagMaxVal)));
        hlayout->addWidget(new QLabel(tagName));

        vlayout->addLayout(hlayout);
    }
}

void Tagger::addTag() 
{
    int row = tags_model_->rowCount();
    tags_model_->insertRow(row);
}

void Tagger::removeTag()
{
    QModelIndex index = tags_table_->currentIndex();
    tags_model_->removeRows(index.row(), 1);
}
