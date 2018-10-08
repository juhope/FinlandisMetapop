#include "timerpoint.h"


TIMERPOINT::TIMERPOINT() {
    x = y = 0; timer = 1;
}

TIMERPOINT::TIMERPOINT(int tx, int ty) : x(tx), y(ty), timer(1) {}

TIMERPOINT::TIMERPOINT(int tx, int ty, int tt) : x(tx), y(ty), timer(tt) {}

int TIMERPOINT::operator==(const TIMERPOINT& right)
{
    return (x == right.x) && (y == right.y);
}
