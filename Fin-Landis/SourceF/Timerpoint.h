#ifndef TIMERPOINT_H
#define TIMERPOINT_H

class TIMERPOINT {
public :
    TIMERPOINT();
    TIMERPOINT(int tx, int ty);
    TIMERPOINT(int tx, int ty, int tt);
    ~TIMERPOINT() {}
    int operator==(const TIMERPOINT& right);
    int x, y, timer;
};

#endif