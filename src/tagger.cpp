#include "ui_tagger.h"
#include "tagger.h"
#include "util.h"
#include "experiment.h"
#include "transport.h"

#include <QLabel>

Tagger::Tagger(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::Tagger)
{
    ui_->setupUi(this);
    initTagTable();
    currentFileName_ = "";
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

    QHeaderView *headerView = tags_table_->horizontalHeader(); 
    headerView->setResizeMode(QHeaderView::Stretch);
    headerView->setResizeMode(1, QHeaderView::Interactive);
    tags_table_->setHorizontalHeader(headerView);
    tags_table_->hideColumn(0);

    ui_->currentDescription->hide();
    ui_->currentTag->hide();
    ui_->verticalLayout_3->addWidget(tags_table_);
}

void Tagger::updateSliders(mrs_string fileName, QTableView* ignoreMe)
{
    if (fileName == currentFileName_)
    {
        return;
    }

    currentFileName_ = fileName;

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

int Tagger::getMaxTagWidth_()
{
    QSqlDatabase db_ = QSqlDatabase::database("Main");    
    QSqlQuery getTags("SELECT * FROM Tags;", db_);
    
    int max = -1;
    
    while (getTags.next())
    {
        int tagName = getTags.value(1).toString().size();
        if (tagName > max)
        {
            max = tagName;
        }
    }
    return max;
}

void Tagger::initTagWidget()
{
    QSqlDatabase db_ = QSqlDatabase::database("Main");    
    QSqlQuery getTags("SELECT * FROM Tags;", db_);

    ui_->currentDescription->show();
    ui_->currentTag->show();

    ui_->verticalLayout_3->removeWidget(tags_table_);
    Util::removeLayoutChildren(ui_->verticalLayout_3, 2);
    
    int maxTagWidth = getMaxTagWidth_();
    
    QWidget *labelWidget = new QWidget();
    QHBoxLayout *labelLayout = new QHBoxLayout(labelWidget);
    QLabel *minLabel = new QLabel("Not at all");
    QLabel *maxLabel = new QLabel("Entirely");

    labelLayout->addWidget(minLabel);
    labelLayout->addStretch();
    labelLayout->addWidget(maxLabel);
    
    ui_->verticalLayout_3->addWidget(labelWidget);

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
        
        QLabel *name = new QLabel(QString("%1").arg(tagName, maxTagWidth));
        QLabel *description = new QLabel(tagDesc);
        description->hide();
        
        name->setObjectName("Name");
        description->setObjectName("Description");

        QLabel *min = new QLabel(QString::number(tagMinVal));
        QLabel *max = new QLabel(QString::number(tagMaxVal));

        QWidget *completeWidget = new QWidget();
        QVBoxLayout *completeLayout = new QVBoxLayout(completeWidget);
        
        completeWidget->setAttribute(Qt::WA_Hover, true);
        completeWidget->installEventFilter(this);
        
        QWidget *labelWidget = new QWidget();
        QHBoxLayout *labelLayout = new QHBoxLayout(labelWidget);

        QWidget *sliderWidget = new QWidget();
        QHBoxLayout *sliderLayout = new QHBoxLayout(sliderWidget);

        labelLayout->addStretch();
        labelLayout->addWidget(name);
        labelLayout->addWidget(description);
        labelLayout->addStretch();

        sliderLayout->addWidget(min);
        sliderLayout->addWidget(slider);
        sliderLayout->addWidget(max);

        completeLayout->addWidget(labelWidget);
        completeLayout->addWidget(sliderWidget);

        tags_table_->hide();
        ui_->verticalLayout_3->addWidget(completeWidget);
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
    ui_->currentTag->setText(name.trimmed());
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

