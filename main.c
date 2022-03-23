#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

struct key
{
    char *name;
    char *value;
};

struct section
{
    char *name;
    struct key **keys;
    int keyCount;
};

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Too little arguments");
        return 1;
    }

    char *path = argv[1];

    char *sectionTarget = strtok(argv[2], ".");
    char *keyTarget = strtok(NULL, ".");

    int sectionCount = 0;
    struct section **sections;

    FILE *fptr;

    fptr = fopen(path, "r");

    if (fptr == NULL)
    {
        printf("No such .ini file");
        return 1;
    }

    char chunk[30];
    size_t len = sizeof(chunk);
    char *line = malloc(len);

    line[0] = '\0';

    struct section *currentSection;

    while (fgets(chunk, sizeof(chunk), fptr) != NULL)
    {

        size_t line_used = strlen(line);
        size_t chunk_used = strlen(chunk);

        if (len - line_used < chunk_used)
        {
            len *= 2;
            if ((line = realloc(line, len)) == NULL)
            {
                printf("Unable to reallocate memroy for line buffer");
                free(line);
                exit(1);
            }
        }

        strncpy(line + line_used, chunk, len - line_used);
        line_used += chunk_used;

        if (line[line_used - 1] == '\n')
        {
            int start = 0;
            enum lineType type = checkLineType(line, &start);
            int currentLen = strlen(line + start);
            currentLen -= 1;

            if (type == Section && (line + start)[currentLen - 1] == ']')
            {
                if (!isValidIdentifier(line + start + 1, currentLen - 2))
                {
                    printf("Invalid section name: %s", (line + start));
                }

                (line + start)[currentLen - 1] = '\0';

                sectionCount++;
                if (sectionCount == 1)
                {
                    sections = malloc(sizeof(struct section *));
                }
                else
                {
                    sections = realloc(sections, sizeof(struct section *) * sectionCount);
                }

                sections[sectionCount - 1] = (struct section *)malloc(sizeof(struct section));

                currentSection = sections[sectionCount - 1];
                currentSection->name = (char *)malloc(sizeof(char) * currentLen);
                currentSection->keyCount = 0;

                strcpy(currentSection->name, line + start + 1);
            }
            else if (type == Value && sectionCount > 0)
            {
                currentSection->keyCount++;

                if (currentSection->keyCount == 1)
                {
                    currentSection->keys = malloc(sizeof(struct key *));
                }
                else
                {
                    currentSection->keys = realloc(currentSection->keys, sizeof(struct key *) * currentSection->keyCount);
                }

                currentSection->keys[currentSection->keyCount - 1] = malloc(sizeof(struct key));

                struct key *currentKey = currentSection->keys[currentSection->keyCount - 1];

                char *token;

                token = strtok(line, " =\n");

                if (!isValidIdentifier(token, strlen(token)))
                {
                    printf("Invalid keyValue: %s", line);
                }

                currentKey->name = (char *)malloc(sizeof(char) * strlen(token));
                strcpy(currentKey->name, token);

                token = strtok(NULL, " =\n");

                if (token == NULL || !isValidIdentifier(token, strlen(token)))
                {
                    printf("Invalid keyValue: %s", line);
                }

                currentKey->value = (char *)malloc(sizeof(char) * strlen(token));
                strcpy(currentKey->value, token);
            }

            line[0] = '\0';
        }
    }

    free(line);

    fclose(fptr);

    for (int i = 0; i < sectionCount; i++)
    {
        if (!strcmp(sections[i]->name, sectionTarget))
        {
            printf("\nSection %s:\n", sections[i]->name);

            for (int j = 0; j < sections[i]->keyCount; j++)
            {
                if (!strcmp(sections[i]->keys[j]->name, keyTarget))
                {
                    printf("Key: %s, Value: %s\n", sections[i]->keys[j]->name, sections[i]->keys[j]->value);
                }
            }
        }
    }

    return 0;
}