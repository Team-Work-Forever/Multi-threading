#if !defined(DOUBLY_LINKED_LITS_INCLUDE)
#define DOUBLY_LINKED_LITS_INCLUDE

#define LISTA_SUCCESS 0
#define LISTA_FAILURE -1
#define LISTA_NOT_FOUND -2

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct process
{
    long id;
    char type;
    char name[100];
    int tempoEspera;
    struct tm tmini;
    struct tm tmfim;
    double tempoExe;
}PROCESS;

typedef struct elem
{
    PROCESS info;
    struct elem *seguinte;
}ELEM;

// Apresenta a listagem da lista
int printBeginList(ELEM *iniLista, void (*func)(PROCESS *));

// Adiciona um item no final da lista
// retorna -1, se ocorrer qualquer erro
// retorna 0, se não ocorrer nenhum erro
int addListEnd(ELEM **iniLista, PROCESS newProcess);

// Obtem o tamanho da lista
int getSize(ELEM *iniLista);

// Liberta a memoria ocupada por cada lista
void limpalista(ELEM **iniLista);
// Obtem o processo por id
// retorna NULL se não encontrar nenhum
// retorna o apontador do processo se o encontrar
PROCESS* getProcessById(ELEM **iniLista, long id);

int removeElem(ELEM **iniLista, PROCESS info);

int removeLast(ELEM **iniLista);

void orderBy(ELEM **iniLista, int (*func)(ELEM*, ELEM*));

ELEM* searchBy(ELEM **iniLista, int (*func)(ELEM*, PROCESS), PROCESS elem);

#endif // DOUBLY_LINKED_LITS_INCLUDE
