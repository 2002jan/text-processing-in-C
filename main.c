#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
    if (argc < 3){
        printf("Too little arguments");
        return 1;
    }

    char *path = argv[1];
    char location[61];

    strcpy(location, argv[2]);

    FILE* fptr;

    fptr = fopen(path, "r");

    if (fptr == NULL){
        printf("No such ,ini file");
        return 1;
    }

    char p[100];
    while(fscanf(fptr, "%s ", p) == 1){
        printf("%s\n", p);
    }

    fclose(fptr);

    return 0;

}