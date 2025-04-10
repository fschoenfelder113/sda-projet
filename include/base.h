
#ifndef BASE
#define BASE

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <stdbool.h>
#include <string.h>

typedef bool Bool;
#define faux false
#define vrai true
//#define vrai 1
//#define faux 0
//typedef unsigned char Bool;

typedef unsigned int Nat;
typedef int Ent;
typedef float Reel;
typedef float Rat;
typedef char Car;
typedef Car * Chaine;

#define MALLOC(type)        ((type*)malloc(sizeof(type)))
#define CALLOCN(type, n)    ((type*)calloc( n, sizeof(type)))
#define MALLOCN(type, n)    ((type*)malloc( (n) * sizeof(type)))
#define REALLOC(t, type, n) ((type*)realloc(t, (n) * sizeof(type)))
#define FREE(t) free(t)

// operations sur les chaines de caracteres
#define NatToChaine(x,s) sprintf(s,"%u%c",x,'\0')
#define EntToChaine(x,s) sprintf(s,"%d%c",x,'\0')
#define ReelToChaine(x,s) sprintf(s,"%g%c",x,'\0')
#define chaineLongueur(s) strlen(s)
#define chaineConcat(sa,sb) strcat(sa,sb)
#define chaineCompare(sa,sb) strcmp(sa,sb)
#define chaineCopie(sa,sb) strcpy(sa,sb)

#endif
#ifndef BASE
#define BASE

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <stdbool.h>
#include <string.h>

typedef bool Bool;
#define faux false
#define vrai true
//#define vrai 1
//#define faux 0
//typedef unsigned char Bool;

typedef unsigned int Nat;
typedef int Ent;
typedef float Reel;
typedef float Rat;
typedef char Car;
typedef Car * Chaine;

#define MALLOC(type)        ((type*)malloc(sizeof(type)))
#define CALLOCN(type, n)    ((type*)calloc( n, sizeof(type)))
#define MALLOCN(type, n)    ((type*)malloc( (n) * sizeof(type)))
#define REALLOC(t, type, n) ((type*)realloc(t, (n) * sizeof(type)))
#define FREE(t) free(t)

// operations sur les chaines de caracteres
#define NatToChaine(x,s) sprintf(s,"%u%c",x,'\0')
#define EntToChaine(x,s) sprintf(s,"%d%c",x,'\0')
#define ReelToChaine(x,s) sprintf(s,"%g%c",x,'\0')
#define chaineLongueur(s) strlen(s)
#define chaineConcat(sa,sb) strcat(sa,sb)
#define chaineCompare(sa,sb) strcmp(sa,sb)
#define chaineCopie(sa,sb) strcpy(sa,sb)

#endif