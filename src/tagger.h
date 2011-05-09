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

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void initTagTable_();
    void initTagWidget_();
    
    Ui::Tagger *ui_;
    QSqlRelationalTableModel *tags_model_;
    QTableView *tags_table_;

private slots:
    void addTag();
    void removeTag();
    void setTagInfo(QString name, QString description);
    void updateValue(int val);

signals:
    void hoverOverTag(QString name, QString description);
    void sliderChanged(int value);
    void updatedValue(QString tag, int value, QString note);

};
#endif // TAGGER_H
