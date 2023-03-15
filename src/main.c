#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "default_func.h"
#include "linkedlists.h"
#include "log_func.h"
#include "queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#include <ctype.h>

// Variavel que armazena o maximo de elementos possíveis na lista
#define MAX_LIMITE_TO_ANY_LIST 4
#define SEND_LIST_WAIT -2
#define EMPTY_WAIT_LIST -3
#define NO_PROCESS_FOUND 1

typedef struct data {
    PROCESS process;
    int currentTime;
    int nextProcessId;
    int isFinish;
    int isBegin;
}DATA;

typedef struct startProcess {
    ELEMQ** queueUrgIni;
    ELEMQ** queueUrgFim;
    ELEMQ** queueNorIni;
    ELEMQ** queueNorFim;
    ELEM** processFinIni;
    HANDLE* thread;
}STARTPROCESS;

DATA currentThreadData = { 0 };

double compareTime(PROCESS* processTemp)
{
    return difftime(mktime(&processTemp->tmfim), mktime(&processTemp->tmini));
}

// Case 1
// Insere a lista, mas se esta ultrapassar do seu limite insere na lista de espera
int insertInList(ELEM **iniListaEsp, ELEM **iniLista, PROCESS *process) {

    int position = 0;

    if (getSize(*iniLista) == MAX_LIMITE_TO_ANY_LIST)
    {
        position = addListEnd(iniListaEsp, *process);
        if(position == EXIT_FAILURE) return position; // Ocorrer qualquer erro ao adicionar lista

        return SEND_LIST_WAIT; // Se ultrapassar o tamanho restrito da lista
    }

    position = addListEnd(iniLista, *process); // Adicionou normalmente á lista

    return position;
}

// Case 3
DWORD WINAPI doProcess(PROCESS *data)
{
    int i = 0;

    currentThreadData.currentTime = 0;
    currentThreadData.process = *data;
    for (i = 0; i < data->tempoEspera; i++)
    {
        currentThreadData.currentTime++;
        Sleep(1000);
    }

    // printf("\n\t !! O processo foi processado !!\n");
    
    return 0;

}

DWORD WINAPI startProcessing(STARTPROCESS *data)
{
    HANDLE mutex = NULL;
    HANDLE thread = NULL;
    DWORD thread_id;
    int result = 0;
    PROCESS processTemp;

    currentThreadData.isFinish = 0;

     // Algoritmos para processamento dos dados
    while (sizeQ(*data->queueUrgIni) != 0 || sizeQ(*data->queueNorIni) != 0)
    {
        // Short if statement, verifies if yet exist process to process in urgent list, they process first if not normal process go for it.
        processTemp = sizeQ(*data->queueUrgIni) > 0 ? dequeue(data->queueUrgIni, data->queueUrgFim) : dequeue(data->queueNorIni, data->queueNorFim);
        
        if(*data->queueUrgFim != NULL)
        {    
            currentThreadData.nextProcessId = sizeQ(*data->queueUrgIni) > 0 ? front(*data->queueUrgFim)->info.id : front(*data->queueNorFim)->info.id;
        }

        mutex = CreateMutex(NULL, FALSE, NULL);

        if (mutex == NULL) 
        {
            printf("Erro ao criar o mutex %d\n", GetLastError());
            return EXIT_FAILURE;
        }

        // Verifica se thread possuir o valor NULL ou se o codigo de saída do anterior for 258
        if(!thread || result == 0) 
        {
            // Cria uma nova thread, passando como argumento o respectivo process
            // thread = CreateThread(NULL, 0, doProcess, &processTemp. 0, &thread_id);
            thread = CreateThread(NULL, 0, doProcess, &processTemp, 0, &thread_id);
        
            if(!thread) 
            {
                printf("\n\t Erro ao abrir uma nova thread!, exit code %d", GetLastError());
                waitForEnter();
                break;
            }
        }

        currentThreadData.isBegin = 1;
        
        // Obtêm o codigo de saída da thread 258 bem sucedida
        result = WaitForSingleObject(thread, INFINITE);

        // Inserir tempo final do processo (quando finalizou)
        processTemp.tmfim = *tempoReal();
        processTemp.tempoExe = compareTime(&processTemp);

        // Registar processos terminados no relatório final
        registarProcesso(&processTemp, printFile);

        addListEnd(data->processFinIni, processTemp);
        registerProcessData(&(TYPEPROCESS){ processTemp, 'R' });

        CloseHandle(thread);

        *data->thread = NULL;
    }

    CloseHandle(mutex);

    if (*data->queueUrgIni == NULL || *data->queueNorFim)
    {
        currentThreadData.isFinish = 1;
    }
    
    return 0;

}

int insertQueue(ELEM **iniLista, ELEMQ **queueUrgIni, ELEMQ **queueUrgFim)
{

    ELEM *aux = NULL;

    for (aux = *iniLista; aux != NULL; aux = aux->seguinte)
        enqueue(queueUrgIni, queueUrgFim, aux->info);

    limpalista(iniLista);

    return EXIT_SUCCESS;

}

void storeNewData(ELEM *iniListaU, ELEM *iniListaN, ELEM *iniListaE, ELEM *iniListaR) {

    ELEM *aux = NULL;
    TYPEPROCESS *array = NULL;
    int size = getSize(iniListaU) + getSize(iniListaN) + getSize(iniListaE) + getSize(iniListaR);
    int count = 0;

    array = (TYPEPROCESS*) calloc(size, sizeof(TYPEPROCESS));

    for ( aux = iniListaU; aux != NULL; aux = aux->seguinte )
    {
        array[count] = (TYPEPROCESS){ aux->info, 'U' };
        count++;
    }

    for ( aux = iniListaN; aux != NULL; aux = aux->seguinte )
    {
        array[count] = (TYPEPROCESS){ aux->info, 'N' };
        count++;
    }

    for ( aux = iniListaE; aux != NULL; aux = aux->seguinte )
    {
        array[count] = (TYPEPROCESS){ aux->info, 'E' };
        count++;
    }

    for ( aux = iniListaR; aux != NULL; aux = aux->seguinte )
    {
        array[count] = (TYPEPROCESS){ aux->info, 'R' };
        count++;
    }

    registerProcessDataOnce(array, size);

    free(array);
    
}

// Case 4

int orderByName(ELEM *a, ELEM* b)
{
    return strcmp(a->info.name, b->info.name);
}

int searchByID(ELEM *a, PROCESS b)
{
    return a->info.id != b.id;
}

ELEM* searchInAllLists(ELEM** processIniListaUrgente, ELEM** processIniListaNormal, ELEM** processIniListaEspera, PROCESS processTemp)
{
    ELEM* elem;

    orderBy(processIniListaUrgente, orderByName);
    elem = searchBy(processIniListaUrgente, searchByID, processTemp);
    if (elem) return elem;

    orderBy(processIniListaNormal, orderByName);
    elem = searchBy(processIniListaNormal, searchByID, processTemp);
    if (elem) return elem;

    orderBy(processIniListaEspera, orderByName);
    elem = searchBy(processIniListaEspera, searchByID, processTemp);
    if (elem) return elem;
    
    return NULL;
}

// Case 5
int generateRanking(ELEM* iniListaU, ELEM* iniListaN, ELEM* iniListaE)
{
    ELEM* aux = NULL;
    TYPEPROCESS *array = NULL;
    int size = getSize(iniListaU) + getSize(iniListaN) + getSize(iniListaE);
    int count = 0, i = 0;

    if(size == 0) return NO_PROCESS_FOUND;
    
    array = (TYPEPROCESS*) calloc(size, sizeof(TYPEPROCESS));

    if (array == NULL) return EXIT_FAILURE;

    for ( aux = iniListaU; aux != NULL; aux = aux->seguinte )
    {          
        array[count] = (TYPEPROCESS){ aux->info, 'U' };
        count++;
    }

    for ( aux = iniListaN; aux != NULL; aux = aux->seguinte )
    {
        array[count] = (TYPEPROCESS){ aux->info, 'N' };
        count++;
    }

    for ( aux = iniListaE; aux != NULL; aux = aux->seguinte )
    {
        array[count] = (TYPEPROCESS){ aux->info, 'E' };
        count++;
    }

    ordenarPorTempoEspera(array, size);

    for (i = 0; i < size; i++)
    {
        printProcess(&array[i].info);
        printf("\t Tempo de execução: %i\n", array[i].info.tempoEspera);
    }

    free(array);

    return EXIT_SUCCESS;
}

// Case 6
void generateReport(ELEM *iniListaRep)
{
    ELEM *aux = NULL;
    TYPEPROCESS *array = NULL;
    int size = getSize(iniListaRep);
    int count = 0;

    array = (TYPEPROCESS*) calloc(size, sizeof(TYPEPROCESS));

    for ( aux = iniListaRep; aux != NULL; aux = aux->seguinte )
    {
        array[count] = (TYPEPROCESS){ aux->info, 'R' };
        count++;
    }

    ordenarPorName(array, size);

    registarProcessos(array, size, printFile);

    printf("\n\t\t>> Ficheiro Criado <<");

    free(array);

}

// Case 7
int compareTimeDown(PROCESS* processTemp1, PROCESS* processTemp2)
{
    return compareTime(processTemp1) > compareTime(processTemp2);
}

int compareTimeUp(PROCESS* processTemp1, PROCESS* processTemp2)
{
   return compareTime(processTemp1) < compareTime(processTemp2);
}

void orderByTimeExe(ELEM** processExe, int (*func)(PROCESS*, PROCESS*))
{
    ELEM * i = NULL, * j = NULL;
    PROCESS temp;

    if(*processExe == NULL)
        return;

    for ( i = *processExe; i != NULL; i = i->seguinte )
    {
        for ( j = i->seguinte; j != NULL; j = j->seguinte )
        {
            if (func(&i->info, &j->info))
            {
                temp = i->info;
                i->info = j->info;
                j->info = temp;
            }
        }
    }
}

double computeAvarage(ELEM* inilista, char type)
{
    ELEM* aux = NULL;
    double sum = 0;
    int counter = 0;
    
    for (aux = inilista; aux != NULL; aux = aux->seguinte)
    {
        if (toupper(aux->info.type) == toupper(type))
        {
            sum += aux->info.tempoExe;
            counter++;
        }  
    }

    if(counter == 0) return 0;

    return (double) sum / (double) counter; 
}

void renderList(ELEMQ *queueUrgFim, ELEMQ *queueNorFim, int currentIndex, int sizePrint)
{

    ELEMQ* aux = NULL;

    if (sizePrint == 0) printf("\n\t\t > Não existe mais processos!");

    for (aux = queueUrgFim; aux != NULL; aux = aux->anterior)
    {
        if (aux->info.id == currentIndex)
        {
            printf("\t\t -> %s", aux->info.name);
        } else {
            printf("\t\t %s", aux->info.name);
        }
    }
    
    for (aux = queueNorFim; aux != NULL; aux = aux->anterior)
    {
        if (aux->info.id == currentIndex)
        {
            printf("\t\t -> %s", aux->info.name);
        } else {
            printf("\t\t %s", aux->info.name);
        }
    }
}

int getProcesseSizeByTipe(ELEM* iniListaR, char type)
{

    ELEM* aux = NULL;
    int couter = 0;

    for ( aux = iniListaR; aux != NULL; aux = aux->seguinte )
    {
        if (toupper(aux->info.type) == toupper(type)) couter++;
    }

    return couter;
}

void printEstatics(ELEM* processFinIni, ELEM* processIniListaEspera, ELEMQ* queueUrgFim, ELEMQ* queueUrgIni, ELEMQ* queueNorFim, ELEMQ* queueNorIni)
{
    printf("\r\t < < Estatisticas de Processos > > \n");
    printf("     *(Carregue em qualquer tecla para sair)*\n");

    printf("\n\t > Processo Atual em processamento <");
    printf("\n\t\tNome: %s\n\r", currentThreadData.process.name);
    printf("\t\tType: %s\n\r", convertTypeToString(currentThreadData.process.type));
    printf("\t\tTempo de Espera: %i\r", currentThreadData.process.tempoEspera - currentThreadData.currentTime);

    printf("\n\n\t > Processos Já processados <");
    printf("\n\t\t Lista de Urgente: %i", getProcesseSizeByTipe(processFinIni, 'U'));
    printf("\n\t\t Lista de Normal: %i", getProcesseSizeByTipe(processFinIni, 'N'));
    printf("\n\t\t Lista de Espera (Processos Rejeitados): %i", getSize(processIniListaEspera));

    printf("\n\n\t > Processo mais demorado <");
    orderByTimeExe(&processFinIni, compareTimeUp);
    if (processFinIni != NULL) printProcessLine(processFinIni->info);
    else printf("\n\t\t > Não existe");

    printf("\n\n\t > Processo menos demorado <");
    orderByTimeExe(&processFinIni, compareTimeDown);
    if (processFinIni != NULL) printProcessLine(processFinIni->info);
    else printf("\n\t\t > Não existe");

    printf("\n\n\t > Tempo médio de espera das listas <");
    printf("\n\t\t Tempo médio de espera da lista Urgente: %.2lf", computeAvarage(processFinIni, 'U'));
    printf("\n\t\t Tempo médio de espera da lista Normal: %.2lf", computeAvarage(processFinIni, 'N'));

    printf("\n\n\t > Lista de Execução <");
    renderList(queueUrgFim, queueNorFim, currentThreadData.nextProcessId, sizeQ(queueUrgIni) + sizeQ(queueNorIni));

}

// Case 8
int ingectProcess(ELEM **processIniListaUrgente, ELEM **processIniListaNormal, ELEM **processIniListaEspera, ELEM **processFinIni)
{

    ELEM *aux = NULL;

    if (*processIniListaEspera == NULL)
        return EMPTY_WAIT_LIST;

    for ( aux = *processIniListaEspera; aux != NULL; aux = aux->seguinte )
    {
        if(toupper(aux->info.type) == 'U') {
            if(getSize(*processIniListaUrgente) < MAX_LIMITE_TO_ANY_LIST)
            {
                addListEnd(processIniListaUrgente, aux->info);
            } else return -1;
        } else {
            if(getSize(*processIniListaNormal) < MAX_LIMITE_TO_ANY_LIST)
            {
                addListEnd(processIniListaNormal, aux->info); 
            } else return -1;         
        }
    }

    storeNewData(*processIniListaUrgente, *processIniListaNormal, *processIniListaEspera, *processFinIni);

    limpalista(processIniListaEspera);

    return EXIT_SUCCESS;
}

// Case 9
int getAllUsers(ELEM* iniListaU, ELEM* iniListaN, ELEM* iniListaE, PROCESS processTemp)
{
    ELEM* aux = NULL;
    TYPEPROCESS *array = NULL;
    int size = getSize(iniListaU) + getSize(iniListaN) + getSize(iniListaE);
    int count = 0;

    if(size == 0) return NO_PROCESS_FOUND;
    
    array = (TYPEPROCESS*) calloc(size, sizeof(TYPEPROCESS));

    if (array == NULL) return EXIT_FAILURE;

    for ( aux = iniListaU; aux != NULL; aux = aux->seguinte )
    {          
        array[count] = (TYPEPROCESS){ aux->info, 'U' };
        if (strcmp(processTemp.name, array[count].info.name) == 0)
        {
            printf("\n\t* Processo Urgente *");
            printProcess(&array[count].info); 
        }
        
        count++;
    }

    for ( aux = iniListaN; aux != NULL; aux = aux->seguinte )
    {
        array[count] = (TYPEPROCESS){ aux->info, 'N' };
        if (strcmp(processTemp.name, array[count].info.name) == 0)
        {
            printf("\n\t* Processo Normal *");
            printProcess(&array[count].info); 
        }

        count++;
    }

    for ( aux = iniListaE; aux != NULL; aux = aux->seguinte )
    {
        array[count] = (TYPEPROCESS){ aux->info, 'E' };
        if (strcmp(processTemp.name, array[count].info.name) == 0)
        {
            printf("\n\t* Processo em Espera *");
            printProcess(&array[count].info); 
        }

        count++;
    }

    free(array);

    return 0;
}

int main()
{
    // Possibilita o uso de carcteres latinos
    SetConsoleOutputCP( CP_UTF8 );

    currentThreadData.isFinish = 1;

    // Variables
    int index = 0, position = 0;
    PROCESS processTemp = {0};

    // Create's th e pointer to the thread
    HANDLE thread = NULL;
    HANDLE mutex = NULL;
    DWORD thread_id;

    // progress urgente lista
    ELEM *processIniListaUrgente = NULL;
    
    // progress normal lista
    ELEM *processIniListaNormal = NULL;
   
    // progress espera lista
    ELEM *processIniListaEspera = NULL;

    // process that finish processing
    ELEM *processFinIni = NULL;
    
    // queue
    ELEMQ *queueUrgIni = NULL;
    ELEMQ *queueUrgFim = NULL;
    
    ELEMQ *queueNorIni = NULL;
    ELEMQ *queueNorFim = NULL;

    readFromBinary(&processIniListaUrgente, 'U');
    readFromBinary(&processIniListaNormal, 'N');
    readFromBinary(&processIniListaEspera, 'E');
    readFromBinary(&processFinIni, 'R');

    do
    {
        clearScreen();

        index = menu("Banco de Processos", (char*[]){"Inserir Processo", "Listar Processos","Iniciar Processos", "Listar Processo por id", "Ranking de processos", "Relatório dos processos ordenado por nome", "Painel de Análise", "Injetar processos em espera para processamento", "Procurar processos de um dono"}, 9);

        switch (index)
        {
            case 1:
            
                clearScreen();

                printf("\n\t < < < Inserir Processo > > > \n");
            
                // Perguntar nome da dono, tempo de execução, se urgente ao normal
                printf("\nInsira o nome do dono do processo: ");
                scanf(" %100[^\n]s", processTemp.name);

                // textBox("Insira o nome do dono do processo", processTemp.name);

                //Cria ID
                processTemp.id = geradorID();

                printf("\n");

                printf("\rInsira o tempo de execução em segundos: "); 
                scanf("%d", &processTemp.tempoEspera);      
                
                do
                {
                    printf("\r\nQue tipo de processo é: \tU - Urgente \t N - Normal \t.: ");
                    scanf(" %c", &processTemp.type);

                    switch (processTemp.type)
                    {
                    case 'U':
                    case 'u':
                        // Tempo de insersão no programa
                        processTemp.tmini = *tempoReal();

                        position = insertInList(&processIniListaEspera,
                        &processIniListaUrgente, &processTemp);
                        if (position == -1) 
                        {
                            printf("\n\t Out of Memory!");

                            return EXIT_FAILURE;
                        } else if (position == -2) {
                            
                            registerProcessData(&(TYPEPROCESS){ processTemp, 'E' });
                            
                            printf("\n\t Processo %li colocado na lista de espera", processTemp.id);
                            break;

                        }

                        registerProcessData(&(TYPEPROCESS){ processTemp, 'U' });

                        break;
                    
                    case 'N':
                    case 'n':
                        // Tempo de insersão no programa
                        processTemp.tmini = *tempoReal();

                        position = insertInList(&processIniListaEspera,
                        &processIniListaNormal, &processTemp);
                        if (position == -1) 
                        {
                            printf("\n\t Out of Memory!");
                            
                            return EXIT_FAILURE;
                        } else if (position == -2) 
                        {
                            registerProcessData(&(TYPEPROCESS){ processTemp, 'E' });

                            printf("\n\t Processo %li colocado na lista de espera", processTemp.id);
                            break;
                        }

                        registerProcessData(&(TYPEPROCESS){ processTemp, 'N' });

                        break;
                    }
                } while (processTemp.type!='U' && processTemp.type!='u' && processTemp.type!='N' && processTemp.type!='n');

                printf("\n\t Processo registado com sucesso!");

                waitForEnter();

                break;
            
            case 2:

                clearScreen();

                printf("\n\t < < < Listar Processos > > >\n");

                do
                {
                    clearScreen();

                    index = menu("Listas",(char*[]){"Urgente", "Normal","Espera"}, 3);

                    switch (index)
                    {
                    case 1:

                        clearScreen();

                        position = printBeginList(processIniListaUrgente, printProcess);
                        if(position == -1) printf("\n\t Nenhum processo encontrado!");

                        waitForEnter();

                        break;
                    
                    case 2:

                        clearScreen();

                        position = printBeginList(processIniListaNormal, printProcess);
                        if(position == -1) printf("\n\t Nenhum processo encontrado!");

                        waitForEnter();
                        
                        break;
                    
                    case 3:

                        clearScreen();

                        position = printBeginList(processIniListaEspera, printProcess);
                        if(position == -1) printf("\n\t Nenhum processo encontrado!");

                        waitForEnter();

                        break;
                    }
                } while (index != -1);

                index = 0;

                break;

            case 3:

                clearScreen();

                printf("\n\t < < < Iniciar Processos > > >\n");
                // Algoritmos de SO
                
                // Tratamento dos dados, ou seja, são: Urgentes, Normais?
                // Insere elementos da Lista Urgente se existirem!
                if(getSize(processIniListaUrgente) > 0)
                {
                    position = insertQueue(&processIniListaUrgente, &queueUrgIni, &queueUrgFim);
                    if (position == -1)
                    {
                        printf("\n\t Out of Memory!");
                        waitForEnter();
                        return EXIT_FAILURE;
                        break;
                    }
                    if (position == -2)
                    {
                        printf("\n\t Processo não encontrado!");
                        waitForEnter();
                        break;
                    }
                    
                }

                // Insere elementos da Lista Normal se existirem!
                if(getSize(processIniListaNormal) > 0)
                {
                    position = insertQueue(&processIniListaNormal, &queueNorIni, &queueNorFim);
                    if (position == -1)
                    {
                        printf("\n\t Out of Memory!");
                        waitForEnter();
                        return EXIT_FAILURE;
                        break;
                    }
                    if (position == -2)
                    {
                        printf("\n\t Processo não encontrado!");
                        waitForEnter();
                        break;
                    }    
                    
                }

                // Se a queue estiver vazia
                if(sizeQ(queueUrgIni) == 0 && sizeQ(queueNorIni) == 0) 
                {
                    printf("\n\t Não existe processos!");
                    waitForEnter();
                    break;
                }

                mutex = CreateMutex( 
                                NULL,              // default security attributes
                                FALSE,             // initially not owned
                                NULL);             // unnamed mutex

                if (mutex == NULL) 
                {
                    printf("CreateMutex error: %d\n", GetLastError());
                    return 1;
                }

                if(!thread) {
                // thread = CreateThread(NULL, 0, doProcess, &processTemp. 0, &thread_id);

                    if (currentThreadData.isFinish == 1)
                        thread = CreateThread(NULL, 0, startProcessing, &(STARTPROCESS){ &queueUrgIni, &queueUrgFim, &queueNorIni, &queueNorFim, &processFinIni, &thread }, 0, &thread_id);

                }

                if(!thread) 
                {
                    printf("\n\t Junção de processos bem sucedida");
                    waitForEnter();
                    break;
                }

                printf("\n\t Deu-se início ao processamento");

                storeNewData(processIniListaUrgente, processIniListaNormal, processIniListaEspera, processFinIni);

                CloseHandle(thread);
                CloseHandle(mutex);

                waitForEnter();
                break;

            case 4:
             
                clearScreen();

                printf("\nInsira o id do processo: ");
                scanf("%li", &processTemp.id);

                ELEM* elem = searchInAllLists(&processIniListaUrgente, &processIniListaNormal, &processIniListaEspera, processTemp);

                if (elem == NULL)
                {
                    printf("\n O elemento com o Id: %i não existe", processTemp.id);

                    waitForEnter();
                    break;                  
                }

                printf("\n\n ------");
                printProcess(&elem->info);
                printf("\n Lista: %s", convertTypeToString(elem->info.type));
                printf("\n\n ------");

                waitForEnter();

                break;

            case 5:
             
                clearScreen();

                position = generateRanking(processIniListaUrgente, processIniListaNormal, processIniListaEspera);

                if (position == 1)
                {
                    printf("\n\t Não existe qualquer processo!");

                    waitForEnter();
                    break;
                } else if (position == -1)
                {
                    printf("\n\t Out of Memory!");

                    waitForEnter();
                    return EXIT_FAILURE;
                }
                
                waitForEnter();
                
                break;

            case 6:
                // Tirar arraw e fazer com listas ligadas
                clearScreen();

                generateReport(processFinIni);
                
                waitForEnter();

                break;

            case 7:

                clearScreen();

                printf("\n\t\t >> Painel de Análise <<");

                if(currentThreadData.isBegin == 0)
                {
                    printf("\n\n\t\t < Não existe processos! >");
                    waitForEnter();
                    break;
                } 

                while (currentThreadData.isFinish != 1)
                {
                    clearScreen();

                    printEstatics(processFinIni, processIniListaEspera, queueUrgFim,
                    queueUrgIni, queueNorFim, queueNorIni);

                    if (kbhit()) 
                    {
                        break;
                    }

                    Sleep(950);
                    
                }

                clearScreen();

                printEstatics(processFinIni, processIniListaEspera, queueUrgFim,
                queueUrgIni, queueNorFim, queueNorIni);

                waitForEnter();

                break;
            
            case 8:

                clearScreen();

                printf("\n\t < < Injetar os processos da lista de espera para as outras listas > >");

                position = ingectProcess(&processIniListaUrgente, &processIniListaNormal, &processIniListaEspera, &processFinIni);
                if (position == -3) {
                    printf("\n\n\t\t Não existem processos para inserir nas listas...");
                }
                if(position == -1) {
                    printf("\n\n\t Lista cheia! Tente depois de processar os processos da lista.");
                }
                if (position == 0) {
                    printf("\n\n\t\t Processos inseridos nas listas com sucesso!");
                }

                waitForEnter();

                break;

            case 9:

                clearScreen();

                printf("\n\t < < Procurar processos de um dono > >");

                printf("\n\n\tInsira o nome do dono: ");
                scanf(" %100[^\n]s", processTemp.name);

                getAllUsers(processIniListaUrgente, processIniListaNormal, processIniListaEspera, processTemp);

                waitForEnter();

                break;
        }

    } while (index != -1);

    storeNewData(processIniListaUrgente, processIniListaNormal, processIniListaEspera, processFinIni);

    limpaQueue(&queueUrgIni, &queueUrgFim);

    limpalista(&processIniListaNormal);
    limpalista(&processIniListaUrgente);
    limpalista(&processIniListaEspera);
    limpalista(&processFinIni);
    
    printf("\n");

    return 0;

}