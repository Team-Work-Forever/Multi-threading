#include "queue.h"

#include "default_func.h"

int enqueue(ELEMQ **iniLista, ELEMQ **fimLista, PROCESS newProcess)
{
    ELEMQ *new = NULL;
    new = (ELEMQ*) calloc(1, sizeof(ELEMQ));

    if(new == NULL)
    {
        return -1;
    }

    new->info = newProcess;
    new->seguinte = NULL;
    new->anterior = NULL;

    if(*iniLista == NULL)
    {
        (*iniLista) = new;
        (*fimLista) = new;
    }
    else
    {
        new->seguinte = *iniLista;
        (*iniLista)->anterior = new;
        *iniLista = new;
    }
    
    return 0;
}

PROCESS dequeue(ELEMQ **iniLista, ELEMQ **fimLista)
{
    ELEMQ *aux = NULL;
    PROCESS process = {0};

    aux = (*fimLista);

    if (aux->anterior == NULL) { // vai remover o 1º elemento
        *iniLista = aux ->seguinte;
        if (*iniLista != NULL) {(*iniLista)->anterior=NULL;}
    }
    else {aux->anterior->seguinte = aux->seguinte; }

    if (aux ->seguinte == NULL) { // vai remover o ultimo elemento
        *fimLista= aux ->anterior;

        if (*fimLista!=NULL) {

            (*fimLista)->seguinte=NULL;

        }
    } else {

        aux->seguinte->anterior = aux->anterior;

    }

    process = aux->info;

    free(aux);

    return process;
}

void orderByQueue(ELEMQ **iniLista, int (*func)(ELEMQ*, ELEMQ*)) 
{

    ELEMQ * i = NULL, * j = NULL;
    PROCESS temp;

    if(*iniLista == NULL)
        return;

    for ( i = *iniLista; i != NULL; i = i->anterior )
    {
        for ( j = i->anterior; j != NULL; j = j->anterior )
        {
            if (func(i, j) > 0)
            {
                temp = i->info;
                i->info = j->info;
                j->info = temp;
            }
            
        }
    }
    
}

ELEM* front(ELEMQ *fimLista)
{
    return fimLista;
}

void limpaQueue(ELEMQ **iniLista, ELEMQ **fimLista) {
    ELEMQ *aux = *iniLista;
    ELEMQ *seguinte = NULL;

    *iniLista=NULL;
    *fimLista=NULL;

    while(aux!=NULL) {

        seguinte = aux->seguinte;
        free(aux);
        aux=seguinte;

    }

}

int sizeQ(ELEMQ *iniLista) 
{

    ELEMQ *aux = NULL;
    int i = 0;
    
    if (iniLista == NULL)
    {
        return 0;
    }

    for (aux = iniLista; aux != NULL; aux = aux->seguinte)
    {
        i++;
    }

    return i;
}