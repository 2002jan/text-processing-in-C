#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

    char chunk[10];
    size_t len = sizeof(chunk);
    char *line = malloc(len);

    line[0] = "\0";

    while(fgets(chunk, sizeof(chunk), fptr) != NULL){
        
        size_t line_used = strlen(line);
        size_t chunk_used = strlen(chunk);

        if (len - line_used < chunk_used)
        {
            len *= 2;
            if ((line = realloc(line, len)) == NULL)
            {
                perror("Unable to reallocate memroy for line buffer");
                free(line);
                exit(1);
            }
            
        }

        strncpy(line + line_used, chunk, len - line_used);
        line_used += chunk_used;

        if (line[line_used - 1] == '\n')
        {
            printf("%s which has a length of %d out of %d \n", line, line_used, len);
            line[0] = '\0';
        }
    }

    free(line);

    fclose(fptr);

    return 0;

}