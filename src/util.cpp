#include "util.h"
#include <QLayoutItem>
#include <QWidget>
#include <QDebug>

namespace Util {
    void removeLayoutChildren(QLayout* layout, int fromIndex)
    {
        QLayoutItem *child;
        QWidget *childWidget;

        while ((child = layout->takeAt(fromIndex)) != 0)
        {
            childWidget = child->widget();
            delete childWidget;
            delete child;
        }
    }

    vector<string> split(const string& s, char c)
    {
        string::size_type i = 0;
        string::size_type j = s.find(c);
        vector<string> v;
    
        while (j != string::npos) {   
            v.push_back(s.substr(i, j-i));
            i = ++j;
            j = s.find(c, j);
        
            if (j == string::npos) {
                v.push_back(s.substr(i, s.length( )));
            }
        }

        return v;
    }
}
