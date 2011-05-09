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
    
private:
    Ui::Experiment *ui_;
    QSqlDatabase db_;
    Transport *transport_;
    Tagger *tagger_;
    
    void createConnections_();
    void populateTagsTable_();
    void populateStimuliTable_();

public slots:
    void openCollectionFile();
    void close();
    void updateValue(QString tag, int rating, QString note);

signals:
    void experimentConfigured();

};

#endif // EXPERIMENT_H
