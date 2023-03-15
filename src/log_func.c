#include "log_func.h"

bool check()
{
    struct stat sd_check;

    if (stat(FOLDER_NAME, &sd_check)) return false;
    else return true; 
}

int registarProcesso(PROCESS *process_Reg, void (*func)(FILE*, PROCESS*))
{
    FILE *fp = NULL;

    if(!check()) _mkdir(FOLDER_NAME);

    fp = fopen(FILE_NAME, "a");

    if (fp == NULL) return -1;
    
    func(fp, process_Reg);
    
    fclose(fp);       
}

int registerProcessData(TYPEPROCESS *process)
{

    FILE* fp = NULL;

    if(!check()) _mkdir(FOLDER_NAME);

    fp = fopen(FILE_NAME_B, "ab");

    if (fp == NULL) return -1;

    fwrite(process, sizeof(TYPEPROCESS), 1, fp);

    fclose(fp);

    return 0;

}

// Fazer write to data all
int readFromBinary(ELEM **iniLista, char type)
{

    FILE* fp = NULL;
    TYPEPROCESS process = {0};
    int res = 0;

    fp = fopen(FILE_NAME_B, "rb");

    if (fp == NULL) return -1;

    while (!feof(fp))
    {
        res = fread(&process, sizeof(TYPEPROCESS), 1, fp);

        if(res > 0 && tolower(type) == tolower(process.type)) {

            addListEnd(iniLista, process.info);

        }
    }

    fclose(fp);

    return 0;
}

int registarProcessos(TYPEPROCESS *process_array, int size, void (*func)(FILE*, PROCESS*))
{
    int i = 0;
    FILE *fp = NULL;

    if(!check()) _mkdir(FOLDER_NAME);

    fp = fopen(FILE_SORTEDNAME, "w");

    if (fp == NULL) return -1;
    
    for (i = 0; i < size; i++)
    {
        func(fp, &process_array[i].info);
    }
    
    fclose(fp);

    return 0;
}

int registerProcessDataOnce(TYPEPROCESS *array, int size)
{

    FILE* fp = NULL;

    if(!check()) _mkdir(FOLDER_NAME);

    fp = fopen(FILE_NAME_B, "wb");

    if (fp == NULL) return -1;

    fwrite(array, sizeof(TYPEPROCESS), size, fp);

    fclose(fp);

    return 0;   

}