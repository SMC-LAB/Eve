#include "userinterface.h"
#include "ui_userinterface.h"
#include "tagger.h"
#include "transport.h"

UserInterface::UserInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserInterface)
{
    ui->setupUi(this);
}

UserInterface::~UserInterface()
{
    delete ui;
}

void UserInterface::init() 
{
    Tagger *tagger_ = Tagger::getInstance();
    ui->verticalLayout->insertWidget(1, tagger_);

    Transport *transport_ = Transport::getInstance();
    connect(transport_, SIGNAL(isPaused(bool)), this, SLOT(transportPaused(bool)));
    connect(ui->pushButtonNext, SIGNAL(clicked()), transport_, SLOT(playOnce()));
}

void UserInterface::deinit() 
{
    Tagger *tagger_ = Tagger::getInstance();
    ui->verticalLayout->insertWidget(1, tagger_);
}

void UserInterface::transportPaused(bool isPaused) 
{
    if (isPaused) {
        ui->pushButtonNext->setText("Next");
        ui->pushButtonNext->setDisabled(false);
    }
    else {
        ui->pushButtonNext->setText("Wait");
        ui->pushButtonNext->setDisabled(true);
    }
}
