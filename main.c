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

void findSectionKey(struct section **sections, int n, char *sectionTarget, char *keyTarget, int *section, int *key);

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Too little arguments");
        return 1;
    }

    char *path = argv[1];

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
                    return 1;
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
                    return 1;
                }

                currentKey->name = (char *)malloc(sizeof(char) * strlen(token));
                strcpy(currentKey->name, token);

                token = strtok(NULL, " =\n");

                if (token == NULL || !isValidIdentifier(token, strlen(token)))
                {
                    printf("Invalid keyValue: %s", line);
                    return 1;
                }

                currentKey->value = (char *)malloc(sizeof(char) * strlen(token));
                strcpy(currentKey->value, token);
            }

            line[0] = '\0';
        }
    }
    free(line);

    fclose(fptr);

    if (strcmp(argv[2], "expression"))
    {
        char *sectionTarget = strtok(argv[2], ".");
        char *keyTarget = strtok(NULL, ".");

        int targetSection = -1, targetKey = -1;

        findSectionKey(sections, sectionCount, sectionTarget, keyTarget, &targetSection, &targetKey);

        printf("Found value %s\nIn section: %s, with key: %s\n", sections[targetSection]->keys[targetKey]->value, sections[targetSection]->name, sections[targetSection]->keys[targetKey]->name);
    }
    else
    {
        char *exp1 = strtok(argv[3], " ");
        char *operator= strtok(NULL, " ");
        char *exp2 = strtok(NULL, " ");

        char *sec1 = strtok(exp1, ".");
        char *key1 = strtok(NULL, ".");

        char *sec2 = strtok(exp2, ".");
        char *key2 = strtok(NULL, ".");

        int targetSection = -1, targetKey = -1;
        findSectionKey(sections, sectionCount, sec1, key1, &targetSection, &targetKey);

        char *val1 = sections[targetSection]->keys[targetKey]->value;

        findSectionKey(sections, sectionCount, sec2, key2, &targetSection, &targetKey);

        char *val2 = sections[targetSection]->keys[targetKey]->value;

        enum valueType type1 = checkValueType(val1), type2 = checkValueType(val2);

        if (type1 != type2)
        {
            printf("Diffrent value types!");
            return 1;
        }

        if (type1 == Text)
        {
            if (operator[0] == '+')
            {
                printf("The result of expresstion: %s is\n", argv[3]);
                printf("%s %s", val1, val2);
            }
            else
            {
                printf("Wrong expression operator");
                return 1;
            }
        }

        if (type1 == Number)
        {
            int num1 = praseInt(val1, strlen(val1)), num2 = praseInt(val2, strlen(val2));

            switch (operator[0])
            {
            case '*':
                num1 *= num2;
                break;
            case '+':
                num1 += num2;
                break;
            case '-':
                num1 -= num2;
                break;
            case '/':
                num1 /= num2;
                break;

            default:
                printf("Wrong expression operator");
                return 1;
                break;
            }
                printf("The result of expresstion: %s is\n", argv[3]);
                printf("%s %c %s = %d",val1, operator[0], val2, num1);
        }
    }

    for (int i = 0; i < sectionCount; i++)
    {
        for (int j = 0; j < sections[i]->keyCount; j++)
        {
            free(sections[i]->keys[j]->name);
            free(sections[i]->keys[j]->value);
            free(sections[i]->keys[j]);
        }
        free(sections[i]->name);
        free(sections[i]->keys);
        free(sections[i]);
    }

    free(sections);

    return 0;
}

void findSectionKey(struct section **sections, int n, char *sectionTarget, char *keyTarget, int *section, int *key)
{
    for (int i = 0; i < n; i++)
    {
        if (!strcmp(sections[i]->name, sectionTarget))
        {
            *section = i;

            for (int j = 0; j < sections[i]->keyCount; j++)
            {
                if (!strcmp(sections[i]->keys[j]->name, keyTarget))
                {
                    *key = j;
                    return;
                }
            }
        }
    }
}