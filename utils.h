#ifndef INI_UTILS
#define INI_UTILS

#include <ctype.h>

enum lineType
{
    Section,
    Value,
    Comment,
    Empty
};

int isValidIdentifier(char *str, int n);

enum lineType checkLineType(char *str, int *startingChar);

enum lineType checkLineType(char *str, int *startingChar)
{
    int i = 0;

    do
    {

        if (str[i] == '[')
        {
            *startingChar = i;
            return Section;
        }

        if (str[i] == ';')
        {
            *startingChar = i;
            return Comment;
        }

        if (isalnum(str[i]))
        {
            *startingChar = i;
            return Value;
        }

    } while (str[++i] != '\0');

    return Empty;
}

int isValidIdentifier(char *str, int n)
{
    int i;

    for (i = 0; i < n; i++)
        if (str[i] != '-' && !isalnum(str[i]))
            return 0;

    return 1;
}



#endif