#if !defined(LOG_FUNC_INCLUDED)
#define LOG_FUNC_INCLUDED

#include "linkedlists.h"
#include "default_func.h"

#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <direct.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>

#define FOLDER_NAME "log"
#define FILE_NAME_B "log/log_registery.dat"
#define FILE_NAME "log/log_registery.log"
#define FILE_SORTEDNAME "log/log_registery_sortedname.log"

// Lista os registos dos processos
int listarRegisto();

// Colocar informações do processo (acabado) no registo
int registarProcesso(PROCESS *process_Reg, void (*func)(FILE*, PROCESS*));

// Colocar informações do processo (acabado) em binario
int registerProcessData(TYPEPROCESS *process);

int readFromBinary(ELEM **iniLista, char type);

int registerProcessDataOnce(TYPEPROCESS *array, int size);

int registarProcessos(TYPEPROCESS *process_array, int size, void (*func)(FILE*, PROCESS*));

#endif // LOG_FUNC_INCLUDED
