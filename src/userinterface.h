#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <QWidget>

namespace Ui {
    class UserInterface;
}

class UserInterface : public QWidget
{
    Q_OBJECT

public:
    static UserInterface* getInstance()
    {
        static UserInterface* instance = new UserInterface();
        return instance;
    }
    ~UserInterface();

    void init();
    void deinit();
    
private:
    Ui::UserInterface *ui;
    explicit UserInterface(QWidget *parent = 0);
    UserInterface(UserInterface const& copy);
    UserInterface& operator=(UserInterface const& copy);
};

#endif // USERINTERFACE_H
