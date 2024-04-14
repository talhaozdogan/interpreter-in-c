#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "header_file.h"

int main()
{

    char line[1025];
    // Create the first person
    struct Person *firstPerson = (struct Person *)malloc(sizeof(struct Person));
    strcpy(firstPerson->name, "dummyFirstPersonName");
    strcpy(firstPerson->location, "NOWHERE");
    firstPerson->dummyItem.nextItemPtr = NULL;
    firstPerson->nextPersonPtr = NULL;

    while (1)
    {
        // Declarations for parser
        struct sentences_conditions_pair **parserReturn = NULL;
        char **res = NULL;
        char ***statement_parts = NULL;
        char ***final_sentences = NULL;

        printf(">> ");
        fflush(stdout);

        // Read a line of input from the user
        if (fgets(line, 1025, stdin) == NULL)
        {
            break;
        }

        // Trim the spaces and \n at the beginning and end of the line
        trim(line);

        // Check if the user wants to exit
        if (strcmp(line, "exit") == 0)
        {
            break;
        }

        // if input text is a question
        if (line[strlen(line) - 1] == '?')
        {
            // printf("Question detected\n");

            int exitCode = question_handler(line, firstPerson);

            if (exitCode == -1)
            {
                printf("INVALID\n");
            }
        }
        // if input text is not a question
        else
        {
            // Send the line to parser
            parserReturn = parser(line, &res, &statement_parts, &final_sentences);
            // Send the parsed data to the executor
            if (parserReturn == NULL)
            {
                printf("INVALID\n");
            }
            else
            {
                // printf("sending to executor\n");
                executor(parserReturn, firstPerson);
                printf("OK\n");
            }
        }

        fflush(stdout);
        free_memory(parserReturn, &res, &statement_parts, &final_sentences);
    }

    return 0;
}