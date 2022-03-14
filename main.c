#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
    if (argc < 3){
        printf("Too little arguments");
        return 0;
    }

    char *path = argv[1];

    printf("size of %s is %lld", path , strlen(path));

    return 0;

}