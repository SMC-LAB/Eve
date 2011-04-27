#include "ui_tagger.h"
#include "tagger.h"

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

    tags_model_ = new QSqlTableModel(this, db_);
    tags_model_->setEditStrategy(QSqlTableModel::OnFieldChange);
    tags_model_->setTable("Tags");
    tags_model_->select();

    tags_table_ = ui_->tagTable;
    tags_table_->setModel(tags_model_);
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
