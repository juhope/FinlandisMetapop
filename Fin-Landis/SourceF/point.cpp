#include "point.h"

// Also change "struct FL_POINT" to "FL_POINT"

FL_POINT::FL_POINT() {
    x = y = 0;
}

FL_POINT::FL_POINT(int tx, int ty) : x(tx), y(ty) {}

int FL_POINT::operator==(const FL_POINT& right) {
    return (x == right.x) && (y == right.y);
}

