#include "linkedlists.h"
#include "default_func.h"

int printBeginList(ELEM *iniLista, void (*func)(PROCESS *))
{
    ELEM *aux = NULL;

    if (iniLista == NULL)
    {
        return -1;
    }

    for ( aux = iniLista; aux != NULL; aux = aux->seguinte)
    {
        func(&aux->info);
    }

    return LISTA_SUCCESS;

}

int addListEnd(ELEM **iniLista, PROCESS newProcess) 
{

    ELEM *aux=NULL;
    ELEM *new=NULL;
    
    new = (ELEM *) malloc(sizeof(ELEM));
    
    if (new==NULL) return -1;
    
    new->info = newProcess;
    new->seguinte = NULL;
    
    if (*iniLista == NULL) { //lista vazia – vai inserir 1º elem
        *iniLista = new; 
    }
    else {
    
        aux = *iniLista;
    
        while (aux->seguinte != NULL){ //percorre a lista até ao fim
            aux = aux->seguinte;
        }

        aux->seguinte = new; //acrescenta o new elemento
    }

    return LISTA_SUCCESS;

}

int getSize(ELEM *iniLista) 
{

    ELEM *aux = NULL;
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

// Verifica-me
int removeElem(ELEM **iniLista, PROCESS info) {

    ELEM *anterior = NULL;
    ELEM *aux = *iniLista;

    while (aux->seguinte != NULL && aux->info.id != info.id)
    {
        anterior = aux;
        aux = aux->seguinte;        
    }
    
    if (aux == NULL) return -1;

    anterior->seguinte = aux->seguinte;

    free(aux);

    return 0;

}

// Verifica-me
int removeLast(ELEM **iniLista)
{
    ELEM *aux = NULL, *ant = NULL;

    if (*iniLista == NULL)
    {
        return -1;
    }
    
    while (aux->seguinte != NULL)
    {
        ant = aux;
        aux = aux->seguinte;        
    }

    if (ant == NULL)
    {
        *iniLista = NULL;
    } else {
        ant->seguinte = NULL;
    }
    
    free(aux);

    return LISTA_SUCCESS;

}

// Limpa a lista
void limpalista(ELEM **iniLista)
{
    ELEM *aux = NULL, *proximo = NULL;

    if (*iniLista == NULL)
    {
        return;
    }
    
    aux = *iniLista;
    *iniLista = NULL;

    while (aux != NULL)
    {
        proximo = aux->seguinte;
        free(aux);
        aux = proximo;
    }
    
}

void orderBy(ELEM **iniLista, int (*func)(ELEM*, ELEM*)) {

    ELEM * i = NULL, * j = NULL;
    PROCESS temp;

    if(*iniLista == NULL)
        return;

    for ( i = *iniLista; i != NULL; i = i->seguinte )
    {
        for ( j = i->seguinte; j != NULL; j = j->seguinte )
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

ELEM* searchBy(ELEM **iniLista, int (*func)(ELEM*, PROCESS), PROCESS elem) {

    ELEM *aux = *iniLista;

    if (*iniLista == NULL)
        return NULL;

    while (aux != NULL && func(aux, elem))
    {
        aux = aux->seguinte;
    }

    if(aux) return aux;

    return NULL;
}