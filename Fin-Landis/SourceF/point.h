#ifndef POINT_H
#define POINT_H

class FL_POINT {
public :
    FL_POINT();
    FL_POINT(int tx, int ty);
    ~FL_POINT() {}
    int operator==(const FL_POINT& right);
    int x, y;
};

#endif
