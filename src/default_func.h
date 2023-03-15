#if !defined(DEFAULT_FUNC_INCLUDED)
#define DEFAULT_FUNC_INCLUDED

#include "linkedlists.h"

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define ARROW_UP_KEY 72
#define ARROW_DOWN_KEY 80
#define ENTER 13
#define SPACE 32
#define ESC 27

typedef struct typeprocess
{
    PROCESS info;
    char type;
}TYPEPROCESS;

// Tempo da Atualidade
struct tm* tempoReal();

// Criar random ID
int geradorID();

// Apresenta a informação de um processo numa linha
void printProcessLine(PROCESS process_look);

// Apresenta a informação de um processo
void printProcess(PROCESS *process_look);

// Escreve toda a informação num ficheiro de texto
void printFile(FILE* fp, PROCESS *process);

// Converte o tipo do processo de U-Urgente e de N-Normal
char* convertTypeToString(char tipo);

// Apenas espera por algum input do stdin
void waitForEnter();

// Após lhe fornecido o titulo, optções e quantidade de opções cria um menu
// Já com a capacidade de sair neste caso se a opção for 0, está retorna esse mesmo valor, que significa que saiu do menu
int menu(char *title, char **options, int n_opt);

// Converte um número qualquer até 60 para string desse número
void numberToString(int number, char* buffer);
// Coverte o número do mês para string
char* numberToMoth(int moth);

// Limpa o ecrã no Windows como no Linux
void clearScreen();

void ordenarPorTempoEspera(TYPEPROCESS *process, int qProcessos);

void ordenarPorName(TYPEPROCESS *process, int qProcessos);

int textBox(char* prompt, char *buffer);

#endif // DEFAULT_FUNC_INCLUDED
