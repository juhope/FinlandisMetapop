#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
//#include "mem.h"
#include "error.h"
#include "system1.h"

#define DELIMIT    '#'
//#ifndef __ZTC__
#define PI         3.141592654
//#endif

static int stringCommentFlag;
static int charCommentFlag;

//////////////////////////////////////////////////////////////////////////////
//                      GENERAL FUNCTIONS CODE                              //
//////////////////////////////////////////////////////////////////////////////

int isStringComment(char *s)
// Given a string, if it is part of comment return TRUE, otherwise FALSE

{
int count=0;

for (s; *s; s++)
	 if (*s==DELIMIT)
		 {
		 count+=1;
		 stringCommentFlag=(stringCommentFlag+1)%2;
		 }
if (stringCommentFlag==1 || count>0) return true;
else return false;
}

int isCharComment(char ch)
// Given a charactor, if it is part of comment retunn TRUE, otherwise FALSE

{
if (ch==DELIMIT)
	charCommentFlag=(charCommentFlag+1)%2;

if ( charCommentFlag==1 || ch==DELIMIT ) return true;
else return false;
 }

int fscanc(FILE *fp, char *fmt, ...)
// fscanc: fscanf function with one more feature: jump the comment delimited
// by # pairs. It'll return TRUE, if success, otherwise FALSE.

{
va_list ap;                 /* points to each unnamed arg in turn  */
char *p, *sval, temp[255];  //Nim: changed 80 to 255
int* ival;       //Nim: changed int to int*
long* lval;      //Nim: changed long to long*
float* fval;    //Nim: added this line
double* dval;    //Nim: changed double to double*

va_start(ap, fmt);          /* make ap point to 1st unnamed arg */
for (p=fmt; *p; p++)
	 {
	 if (*p != '%') continue;
	 while (fscanf(fp, "%s", temp), isStringComment(temp))
		  ;
	 switch (*++p)
		 {
		 case 'd':
			  ival = va_arg(ap, int*);   //Nim: changed int to int*
			  sscanf(temp, "%d", ival);
			  break;
		 case 'f':
			  fval = va_arg(ap, float*); //Nim: changed dval to fval and double to float*
			  sscanf(temp, "%f", fval);  //Nim: changed dval to fval
			  break;
       case 's':
			  sval = va_arg(ap, char*);
			  strcpy(sval, temp);
	   break;
       case 'l':
	   switch (*++p)
	       {
	       case 'd':
		   lval = va_arg(ap, long*); //Nim: changed long to long*
		   sscanf(temp, "%ld", lval);
						 break;
					case 'f':
						 dval = va_arg(ap, double*); //Nim: changed double to double*
						 sscanf(temp, "%lf", dval);   //Nim: changed %f to %lf
						 break;
					default:
						 break;
					}
			  break;
		 default:
			  break;
		 };
	 }
	 va_end(ap);             /* clean up when done    */

return true;
}


void skipblanks(FILE* infile)
//This will skip all white space and the comment delimited by # pairs in infile.

{
char ch;

//while (((ch=getc(infile))!=EOF)&&(isspace(ch))) {}     old code
while (((ch=(char)getc(infile))!=EOF) && ((isspace(ch)) || (isCharComment(ch)))) {}

if (!feof(infile))
	 ungetc(ch, infile);
}

int eof(FILE* infile)
//This will return TRUE if eof is encountered and FALSE otherwise.  It is
//different from the feof library function in that it skips all white space
//and the comment delimited by # pairs before detecting the end of file
//condition.

{
char ch;

// while (((ch=getc(infile))!=EOF)&&(isspace(ch))) {}     old code
while (((ch=(char)getc(infile))!=EOF) && ((isspace(ch)) || (isCharComment(ch)))) {}
if (!feof(infile))
	{
	 ungetc(ch,infile);
	 return false;
	}
else
	 return true;
}

int min(int a, int b)
//Returns minimun of a and b.

{
if (a<b)
	return a;
else
	return b;
}

float min(float a, float b)
//Returns minimum of a and b.

{
if (a<b)
	return a;
else
	return b;
}

int max(int a, int b)
//Returns maximum of a and b.

{
if (a>b)
	 return a;
else
	 return b;
}

float max(float a, float b)
//Returns maximum of a and b.

{
if (a>b)
	 return a;
else
	 return b;
}

void fseed(int seed)
//This will seed the random number generator.

{
#ifdef __UNIX__
srand48(seed);
#else
srand(seed);
#endif
}

float frand()
//This will return a random number.

{
//dos_gettime(&t);
//fseed(t.hsecond);
#ifdef __UNIX__
	{
	return (float)drand48();
	}
#else
	{
	return (float) rand() / (float) RAND_MAX;
	}
#endif
}

double drand()
//This will return a random number.

{
//dos_gettime(&t);
//fseed(t.hsecond);
#ifdef __UNIX__
	{
	return drand48();
	}
#else
	{
	return (double) rand() / (double) RAND_MAX;
	}
#endif
}

int irand(int a, int b)
//This will return a random integer between a and b inclusive.
{
 return (int) (rand() % (b-a+1) + a);
}

float frandNorm()
//This returns a random number normally distributed around 0.0.
{
 float a1=0, a2=0;
 while (a1==0)
 {
 	a1=frand();
 	a2=frand();
 }
 return ( sqrt(-0.75*log(a1))*sin(PI*PI*a2) )-0.14;
}


float franCos()
{
 float a1=frand();
 return cos(PI*PI*a1);
}

//float franII()
//{
// float a1=rand();
// return sin(PI*PI*a1);
//}

void swap(float &a, float &b)
//This will swap the values of a and b.

{
float t;
t=a;
a=b;
b=t;
}

double k(int n)
//This is needed to assist poisson. JP.
{
if (n==0) return (unsigned long)1;
else return n*k(n-1);
}

int pseudoPoisson(float a)
//This returns a Poisson-distributed random variable with mean a,
//when a<=100. With larger values Poisson is simulated by normal distribution
//with same mean and variation. Probably not optimal algorithm, but only needed
//once every iteration. JP.

{
if (a==0)
    return 0;
if (a>100)
    return frandNorm()*sqrt(a)/0.59+a;
float x=frand();
double s=0;
int i=0;
do s+=pow(a,i)*exp(-a)/k(i);
while (s<x && i++<155);  //last one to avoid overflow
return i;
}

