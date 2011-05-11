#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QMainWindow>
#include <QTableView>
#include <QSqlTableModel>
#include <QtSql>
#include "transport.h"
#include "tagger.h"

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
    Tagger* getTagger();
    int getCurrentSubjectId();
    
private:
    Ui::Experiment *ui_;
    QSqlDatabase db_;
    Transport *transport_;
    Tagger *tagger_;
    QSqlRelationalTableModel *subjects_model_;
    QTableView *subjects_table_;
    
    void populateTagsTable_();
    void populateStimuliTable_();
    void populateSubjectsTable_();
    void createConnections_();

public slots:
    void openCollectionFile();
    void close();
    void updateValue(QString tag, int rating, QString note);
    void addSubject();
    void removeSubject();

signals:
    void experimentConfigured();

};

#endif // EXPERIMENT_H
