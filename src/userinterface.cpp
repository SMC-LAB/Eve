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
    connect(ui->pushButtonNext, SIGNAL(clicked()), transport_, SLOT(playOnce()));
}

void UserInterface::deinit() 
{
    Tagger *tagger_ = Tagger::getInstance();
    ui->verticalLayout->insertWidget(1, tagger_);
}

