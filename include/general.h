#include <unistd.h>

int IsFileExist(char *name){
    if(access(name, F_OK) != -1)
        return 1;
    else
        return 0;
}
