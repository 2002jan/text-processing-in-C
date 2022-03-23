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

enum valueType
{
    Text,
    Number
};

int isValidIdentifier(char *str, int n);

enum lineType checkLineType(char *str, int *startingChar);

enum valueType checkValueType(char *value);

int praseInt(char *str, int n);

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

enum valueType checkValueType(char *value)
{
    int i = 0;

    do{
        if (isalpha(value[i]))
        {
            return Text;
        }
    } while(value[++i] != '\0');

    return Number;
}

int praseInt(char *str, int n)
{

    int prasedString = 0;

    for (int i = 0; i < n; i++)
    {
        int temp = (str[i] - 48);

        for (int j = 0; j < n - i - 1; j ++){
            temp *= 10;
        }

        prasedString += temp;
    }

    return prasedString;
    

}

#endif