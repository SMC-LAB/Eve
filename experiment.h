#ifndef EXPERIMENT_H
#define EXPERIMENT_H
#include <QMainWindow>
#include <QTableView>
#include <QSqlTableModel>
#include <QtSql>

namespace Ui {
    class Experiment;
}

class Experiment : public QMainWindow
{
    Q_OBJECT

public:
    explicit Experiment(QWidget *parent = 0);
    ~Experiment();
    void init(QString fileName);
    QSqlDatabase getDb();
    
private:
    Ui::Experiment *ui_;
    QSqlDatabase db_;

    QSqlTableModel *model_;
    QTableView *table_;

    void createConnections_();
    void populateTagsTable_();

private slots:
    void addTag();
    void removeTag();
    
};

#endif // EXPERIMENT_H
