#ifndef ERROR_H
#define ERROR_H

#define STOP     0
#define CONTINUE 1

void errorSys(const char*, int);
//This prints an error message on the CRT screen and then either halts, or
//returns control to the program depending on the state of the second
//parameter.  The first parameter is the error message.  The second parameter
//is either set to STOP or CONTINUE.

#endif

