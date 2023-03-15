#include "default_func.h"

// Tempo da Atualidade
struct tm* tempoReal()
{
  time_t rawtime;
  struct tm * timeinfo;

  time ( &rawtime );
  timeinfo = localtime ( &rawtime );
  
  return timeinfo;
}

// Criar random ID
int geradorID()
{
    int r;
    
    srand(time(NULL));
    r = rand();
    return r;
}

// Lista a lista em linha
void printProcessLine(PROCESS process_look)
{
    char buffer_hour[3];
    char buffer_min[3];
    char buffer_sec[3];

    char buffer_fi_hour[3];
    char buffer_fi_min[3];
    char buffer_fi_sec[3];

    numberToString(process_look.tmini.tm_hour, buffer_hour);
    numberToString(process_look.tmini.tm_min, buffer_min); 
    numberToString(process_look.tmini.tm_sec, buffer_sec);

    numberToString(process_look.tmfim.tm_hour, buffer_fi_hour);
    numberToString(process_look.tmfim.tm_min, buffer_fi_min); 
    numberToString(process_look.tmfim.tm_sec, buffer_fi_sec);

    printf("\n\t\t ID: %li", process_look.id);
    printf("   Nome: %s", process_look.name);
    printf("   Tipo: %s", convertTypeToString(process_look.type));
    printf("\n\t\t Tempo de inserção: %s:%s:%s %i %s %i",
    buffer_hour,
    buffer_min,
    buffer_sec,
    process_look.tmini.tm_mday,
    numberToMoth(process_look.tmini.tm_mon),
    process_look.tmini.tm_year + 1900);
    printf("   Tempo de execução: %i", process_look.tempoEspera);

    printf( "   Fim: %s:%s:%s %i %s %i \n",
        buffer_fi_hour,
        buffer_fi_min,
        buffer_fi_sec,
        process_look.tmfim.tm_mday,
        numberToMoth(process_look.tmfim.tm_mon), 
        process_look.tmfim.tm_year + 1900);
}
// Lista a lista
void printProcess(PROCESS *process_look)
{
    char buffer_hour[3];
    char buffer_min[3];
    char buffer_sec[3];

    numberToString(process_look->tmini.tm_hour, buffer_hour);
    numberToString(process_look->tmini.tm_min, buffer_min); 
    numberToString(process_look->tmini.tm_sec, buffer_sec);

    printf("\n\t ID: %li", process_look->id);
    printf("\n\t Nome: %s", process_look->name);
    printf("\n\t Tipo: %s", convertTypeToString(process_look->type));
    printf("\n\t Tempo de inserção: %s:%s:%s %i %s %i\n",
    buffer_hour,
    buffer_min,
    buffer_sec,
    process_look->tmini.tm_mday,
    numberToMoth(process_look->tmini.tm_mon),
    process_look->tmini.tm_year + 1900);
}

void printFile(FILE* fp, PROCESS *process)
{
    char buffer_in_hour[3];
    char buffer_in_min[3];
    char buffer_in_sec[3];

    char buffer_fi_hour[3];
    char buffer_fi_min[3];
    char buffer_fi_sec[3];

    numberToString(process->tmini.tm_hour, buffer_in_hour);
    numberToString(process->tmini.tm_min, buffer_in_min); 
    numberToString(process->tmini.tm_sec, buffer_in_sec);

    numberToString(process->tmfim.tm_hour, buffer_fi_hour);
    numberToString(process->tmfim.tm_min, buffer_fi_min); 
    numberToString(process->tmfim.tm_sec, buffer_fi_sec);

    fprintf(fp,"ID: %li \tOWNER: %s \tTYPE: %s \tIni: %s:%s:%s %i %s %i \tTempo de execução: %i ",
    process->id,
    process->name, 
    convertTypeToString(process->type), 
    buffer_in_hour,
    buffer_in_min,
    buffer_in_sec,
    process->tmini.tm_mday,
    numberToMoth(process->tmini.tm_mon), 
    process->tmini.tm_year + 1900,
    process->tempoEspera);
    if (process->tmfim.tm_year > 0)
    {
        fprintf(fp, "\tFim: %s:%s:%s %i %s %i \n",
        buffer_fi_hour,
        buffer_fi_min,
        buffer_fi_sec,
        process->tmfim.tm_mday,
        numberToMoth(process->tmfim.tm_mon), 
        process->tmfim.tm_year + 1900);
    } else fprintf(fp, "\tFim: Ainda não foi processado\n");
    
    
}

char* convertTypeToString(char tipo) 
{
    switch (tipo)
    {
    case 'u':
    case 'U':
        return "Urgente";
        break;
    
    case 'n':
    case 'N':
        return "Normal";
        break;

    case 'e':
    case 'E':
        return "Espera";
        break;
    }
}

void waitForEnter()
{
    char ch;

    printf("\n\n");
    do
    {
        printf("\r\t\t Pressione Enter...");
    } while ((ch = getch()) != ENTER);
}

void draw_menu(int selectedIndex, char **options, int n_opt)
{

    int i = 0;

    for (i = 0; i < n_opt; i++)
    {

        if (i == selectedIndex)
        {

            printf("\n\t >> %s <<", options[i]);
            
        } else {

            printf("\n\t    %s", options[i]);

        }
    
    }

    printf("\n\nENTER - Selecionar \tSPACE - Exit");
    
}

int menu(char *title, char **options, int n_opt) 
{
    int selectedIndex = 0;
    int key = 0;

    do
    {
        clearScreen();

        printf("\n\t      < < %s > >\n", title);

        draw_menu(selectedIndex, options, n_opt);

        key = _getch();

        if (key == ARROW_UP_KEY)
        {
            selectedIndex--;

            if (selectedIndex < 0)
            {
                selectedIndex = n_opt - 1;
            }
            

        } else if (key == ARROW_DOWN_KEY)
        {
            selectedIndex++;

            if(selectedIndex > n_opt - 1) 
            {
                selectedIndex = 0;
            }
        } else if (key == SPACE) 
        {
            return -1;
        } 
        
    } while (key != ENTER);

    return selectedIndex + 1;
}

void numberToString(int number, char* buffer)
{
    char bufferTemp[3] = "0";

    buffer[0] = '\0';
   
    strcpy(buffer, itoa(number, buffer, 10));


    if (number < 10)
    {
        strcat(bufferTemp, buffer);
        strcpy(buffer, bufferTemp);
    }

}

char* numberToMoth(int moth) 
{

    switch (moth)
    {
    case 0:
        return "Janeiro";
        break;
    case 1:
        return "Fevereiro";
        break;
    case 2:
        return "Março";
        break;
    case 3:
        return "Abril";
        break;
    case 4:
        return "Maio";
        break;
    case 5:
        return "Junho";
        break;
    case 6:
        return "Julho";
        break;
    case 7:
        return "Agosto";
        break;
    case 8:
        return "Setembro";
        break;
    case 9:
        return "Outubro";
        break;
    case 10:
        return "Novembro";
        break;

    case 11:
        return "Dezembro";
        break;

    default:
        return "Undifined!";
        break;    
    }

}

void clearScreen() 
{ 
    system("cls");
}

void ordenarPorTempoEspera(TYPEPROCESS *process, int qProcessos)
{
    int i = 0, j = 0;
    TYPEPROCESS aux;

    for(i = 0;  i < qProcessos; i++)
    {        
        for(j = 0; j < qProcessos - 1 - i; j++)
        {
            if(process[j].info.tempoEspera > process[j+1].info.tempoEspera)
            {
                aux = process[j];
                process[j] = process[j+1];
                process[j+1] = aux;
            }
        }
    }
}

void ordenarPorName(TYPEPROCESS *process, int qProcessos)
{
    int i = 0, j = 0;
    TYPEPROCESS aux;

    for(i = 0;  i < qProcessos; i++)
    {        
        for(j = 0; j < qProcessos - 1 - i; j++)
        {
            if(strcmp(process[j].info.name, process[j+1].info.name) > 0)
            {
                aux = process[j];
                process[j] = process[j+1];
                process[j+1] = aux;
            }
        }
    }
}

int textBox(char* prompt, char *buffer)
{
    int i = 0;
    int key = 0;

    printf("%s (ESC - Sair): \n\t.: ", prompt);
    
    fflush(stdin);

    while ((key = _getch()) != ESC)
    {
        buffer[strlen(buffer)] = (char)key;

        if (key == 8 && strlen(buffer) > 0)
        {
            buffer[strlen(buffer) - 1] = buffer[strlen(buffer)];
            printf("\b \b");
            i--;
        } else if(!isalpha(key))
        {
            continue;
        }
        else
        {
            printf("%c", buffer[i]);
        }

        i++;   
    }

    return 0;   
}