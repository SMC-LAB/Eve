#ifndef TAGGER_H
#define TAGGER_H

#include <QWidget>
#include <QtSql>
#include <QSqlRelationalTableModel>
#include <QTableView>

typedef std::string mrs_string;

namespace Ui {
    class Tagger;
}

class Tagger : public QWidget
{
    Q_OBJECT

public:
    static Tagger* getInstance()
    {
        static Tagger* instance = new Tagger();
        return instance;
    }
    ~Tagger();
    void initTagTable();
    void initTagWidget();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    explicit Tagger(QWidget *parent = 0);
    Ui::Tagger *ui_;
    QSqlRelationalTableModel *tags_model_;
    QTableView *tags_table_;
    mrs_string currentFileName_;
    int getMaxTagWidth_();
                                

private slots:
    void addTag();
    void removeTag();
    void setTagInfo(QString name, QString description);
    void updateValue(int val);
    void updateSliders(mrs_string fileName, QTableView* ignoreMe);

signals:
    void hoverOverTag(QString name, QString description);
    void sliderChanged(int value);
    void updatedValue(QString tag, int value, QString note);

};

#endif // TAGGER_H
