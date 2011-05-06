#ifndef TAGGER_H
#define TAGGER_H

#include <QWidget>
#include <QtSql>
#include <QSqlRelationalTableModel>
#include <QTableView>

namespace Ui {
    class Tagger;
}

class Tagger : public QWidget
{
    Q_OBJECT

public:
    explicit Tagger(QWidget *parent = 0);
    ~Tagger();
    void setCustomDelegate();

private:
    void initTagTable_();
    void initTagWidget_();
    
    Ui::Tagger *ui_;
    QSqlRelationalTableModel *tags_model_;
    QTableView *tags_table_;

private slots:
    void addTag();
    void removeTag();

};

#endif // TAGGER_H
