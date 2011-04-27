#ifndef TAGGER_H
#define TAGGER_H

#include <QWidget>
#include <QtSql>
#include <QSqlTableModel>
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

private:
    void initTagTable_();

    Ui::Tagger *ui_;
    QSqlTableModel *tags_model_;
    QTableView *tags_table_;

private slots:
    void addTag();
    void removeTag();

};

#endif // TAGGER_H
