#ifndef TAGGER_H
#define TAGGER_H

#include <QWidget>

namespace Ui {
    class Tagger;
}

class Tagger : public QWidget
{
    Q_OBJECT

public:
    explicit Tagger(QWidget *parent = 0);
    ~Tagger();

private:
    void init_();
    Ui::Tagger *ui_;    
};

#endif // TAGGER_H
