#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QMainWindow>
#include <QTableView>
#include <QSqlTableModel>
#include <QtSql>
#include "transport.h"

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
    Transport* getTransport();
    
private:
    Ui::Experiment *ui_;
    QSqlDatabase db_;
    Transport *transport_;
    
    QSqlTableModel *model_;
    QTableView *table_;

    void createConnections_();
    void populateTagsTable_();
    void populateStimuliTable_();

private slots:
    void addTag();
    void removeTag();
    void openCollectionFile();
};

#endif // EXPERIMENT_H
