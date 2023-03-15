#if !defined(QUEUE_INCLUDE)
#define QUEUE_INCLUDE

#include "linkedlists.h"

typedef struct elemq {

    PROCESS info;
    struct elemq *seguinte;
    struct elemq *anterior;

}ELEMQ;

int enqueue(ELEMQ **iniLista, ELEMQ **fimLista, PROCESS newProcess);

PROCESS dequeue(ELEMQ **iniLista, ELEMQ **fimLista);

int sizeQ(ELEMQ *iniLista); 

void limpaQueue(ELEMQ **iniLista, ELEMQ **fimLista);

void orderByQueue(ELEMQ **iniLista, int (*func)(ELEMQ*, ELEMQ*));

ELEM* front(ELEMQ *fimLista);

#endif // QUEUE_INCLUDE
