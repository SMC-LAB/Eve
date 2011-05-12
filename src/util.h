#ifndef UTIL_H
#define UTIL_H

#include <QLayout>
#include <vector>
#include <string>
using namespace std;

namespace Util {
    void removeLayoutChildren(QLayout* layout, int fromIndex = 0);
    vector<string> split(const string& s, char c);
}

#endif //UTIL_H
