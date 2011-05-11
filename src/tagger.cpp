#include "ui_tagger.h"
#include "tagger.h"
#include "util.h"
#include "experiment.h"
#include "transport.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

Tagger::Tagger(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::Tagger)
{
    ui_->setupUi(this);
    initTagTable();
}

Tagger::~Tagger()
{
    delete ui_;
}

void Tagger::initTagTable()
{
    QSqlDatabase db_ = QSqlDatabase::database("Main");    

    Util::removeLayoutChildren(ui_->verticalLayout_3, 2);

    tags_model_ = new QSqlRelationalTableModel(this, db_);
    tags_model_->setEditStrategy(QSqlTableModel::OnFieldChange);
    tags_model_->setTable("Tags");
    tags_model_->select();

    tags_table_ = new QTableView();
    tags_table_->setModel(tags_model_);
    tags_table_->setItemDelegate(new QSqlRelationalDelegate(tags_table_));

    ui_->currentDescription->hide();
    ui_->currentTag->hide();
    ui_->verticalLayout_3->addWidget(tags_table_);
}

void Tagger::updateSliders(mrs_string, QTableView* ignoreMe)
{
    QSqlDatabase db_ = QSqlDatabase::database("Main");    
    QSqlQuery getTags("SELECT * FROM Tags;", db_);

    while (getTags.next())
    {
        QString tagName = getTags.value(1).toString();
        int tagMinVal = getTags.value(2).toInt();
        QSlider *slider = ui_->scrollAreaWidgetContents->findChild<QSlider *>(tagName);
            
        if (int rating = Experiment::getInstance()->getValue(tagName))
        {                        
            slider->setValue(rating);
        }
        else
        {
            slider->setValue(tagMinVal);
        }
    }       
}


void Tagger::initTagWidget()
{
    QSqlDatabase db_ = QSqlDatabase::database("Main");    
    QSqlQuery getTags("SELECT * FROM Tags;", db_);

    ui_->currentDescription->show();
    ui_->currentTag->show();
    ui_->verticalLayout_3->removeWidget(tags_table_);
    Util::removeLayoutChildren(ui_->verticalLayout_3, 2);
    
    while (getTags.next())
    {
        QString tagName = getTags.value(1).toString();
        QString tagDesc = getTags.value(4).toString();
        int tagMinVal = getTags.value(2).toInt();
        int tagMaxVal = getTags.value(3).toInt();
    
        QSlider *slider = new QSlider(Qt::Horizontal);
        slider->setMinimum(tagMinVal);
        slider->setMaximum(tagMaxVal);
        slider->setTickInterval((tagMaxVal - tagMinVal) / 10);
        slider->setTickPosition(QSlider::TicksBelow);
        slider->setTracking(false);
        slider->setPageStep(1);
        slider->setObjectName(tagName);
        
        if (int rating = Experiment::getInstance()->getValue(tagName))
        {                        
            slider->setValue(rating);
        }
        else
        {
            slider->setValue(tagMinVal);
        }

        connect(slider, SIGNAL(valueChanged(int)), this, SLOT(updateValue(int)));
        
        QLabel *name = new QLabel(tagName);
        QLabel *description = new QLabel(tagDesc);
        description->hide();
        
        name->setObjectName("Name");
        description->setObjectName("Description");

        QLabel *min = new QLabel(QString::number(tagMinVal));
        QLabel *max = new QLabel(QString::number(tagMaxVal));

        QWidget *nestedWidget = new QWidget();
        QHBoxLayout *hlayout = new QHBoxLayout(nestedWidget);
        
        nestedWidget->setAttribute(Qt::WA_Hover, true);
        nestedWidget->installEventFilter(this);

        hlayout->addWidget(min);
        hlayout->addWidget(slider);
        hlayout->addWidget(max);
        hlayout->addWidget(name);
        hlayout->addWidget(description);

        tags_table_->hide();
        ui_->verticalLayout_3->addWidget(nestedWidget);
    }
    
    ui_->verticalLayout_3->addStretch();

    connect(this, SIGNAL(hoverOverTag(QString, QString)), this, SLOT(setTagInfo(QString, QString)));
    connect(Transport::getInstance(), SIGNAL(fileChanged(mrs_string, QTableView*)), this, SLOT(updateSliders(mrs_string, QTableView*)));
}

void Tagger::updateValue(int value)
{
    QString name = ui_->currentTag->text();
    //ui_->currentTag->setText(name + " - " + QString::number(value));

    emit updatedValue(name, value, "");
}


void Tagger::setTagInfo(QString name, QString description)
{
    ui_->currentTag->setText(name);
    ui_->currentDescription->setText(description);
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

bool Tagger::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::HoverEnter)
    {
        QLabel *description = obj->findChild<QLabel *>("Description");
        QLabel *name = obj->findChild<QLabel *>("Name");
        
        emit hoverOverTag(name->text(), description->text());

        return true;
    }
    else
    {
        return QObject::eventFilter(obj, event);
    }
}

