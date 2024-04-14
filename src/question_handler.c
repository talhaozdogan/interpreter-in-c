#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header_file.h"

// frees the memory of split
void freeSplit(char **split){
    // if the split is NULL
    if( split == NULL ){
        return;
    }

    // Free the elements of the split
    for(int i=0; split[i]!=NULL; i++){
        free(split[i]);
    }

    // Free the split
    free(split);
}

//handles the question, if valid returns 0, if not returns -1
int question_handler(char *inputText, struct Person *firstPerson){
    // Split the text
    int charLength = strlen(inputText);
    char **splittedText = split(inputText, " ", charLength);
    int splittedLength = get_string_array_length(splittedText);

    //Find the type of the question
    //if the quesiton is a subjects total item question
    if( contains("total", splittedText) == 1 && splittedLength > 3 ){
        //printf("subjects total item question\n");
        // Check ? and total for the format
        if( strcmp(splittedText[splittedLength-1], "?")==0 && strcmp(splittedText[splittedLength-3], "total")==0 ){
            // Check if the item is valid
            if( isValidSubjectOrItem(splittedText[splittedLength-2])==0 ){
                freeSplit(splittedText);
                return -1;
            }

            // Extract subjects and check if they are valid
            char **subjects = and_parser(splittedText, 0, splittedLength-3, 1, 1);

            if( subjects == NULL ){
                //printf("Subjects are invalid (parser)\n");
                freeSplit(splittedText);
                return -1;
            }

            for(int i=0; subjects[i]!=NULL; i++){
                //printf("Name: %s\n", subjects[i]);
                if( isValidSubjectOrItem(subjects[i])==0 ){
                    //printf("Subjects are invalid (keyword etc.)\n");
                    freeSplit(splittedText);
                    free(subjects);
                    return -1;
                }
            }

            // Find the total of the item for the subjects
            int total = 0;
            for(int i=0; subjects[i]!=NULL; i++){
                struct Person *person = getPersonByName(firstPerson, subjects[i]);
                total += getQuantity(person, splittedText[splittedLength-2]);
            }

            printf("%d\n", total);
            freeSplit(splittedText);
            free(subjects);
            return 0;
        }        
    }

    //if the question is where question
    else if( contains("where", splittedText) && splittedLength == 3 ){
        //printf("where question\n");
        if( strcmp(splittedText[1], "where")==0 && strcmp(splittedText[2], "?")==0 ){
            char *subject = splittedText[0];

            // Check if the subject is a valid subject
            if( isValidSubjectOrItem(subject)==0 ){
                freeSplit(splittedText);
                return -1;
            }

            struct Person *person = getPersonByName(firstPerson, subject);
            printf("%s\n", getLocation(person));
            freeSplit(splittedText);
            return 0;
        }
        else {
            freeSplit(splittedText);
            return -1;
        }
    }
    
    //if the question is who at question
    else if( contains("who", splittedText) && contains("at", splittedText) && splittedLength == 4 ){
        //printf("who at question\n");
        if( strcmp(splittedText[0], "who")==0 && strcmp(splittedText[1], "at")==0 && strcmp(splittedText[3], "?")==0 ){
            //Check if the location is valid
            if( isValidSubjectOrItem(splittedText[2])==0 ){
                freeSplit(splittedText);
                return -1;
            }

            // Find the index of the last person at the location
            char *location = splittedText[2];
            struct Person *person = firstPerson->nextPersonPtr;

            int lastIdx = -1;
            for(int i=0; person!=NULL; i++){
                if( isAtLocation(person, location)==1 ){
                    lastIdx = i;
                }
                person = person->nextPersonPtr;
            }

            // If there is no person at the location
            if( lastIdx == -1 ){
                printf("NOBODY\n");
                freeSplit(splittedText);
                return 0;
            } 

            // print all people at the location
            person = firstPerson->nextPersonPtr;
            for(int i=0; i<=lastIdx; i++){
                if( isAtLocation(person, location)==1 ){
                    printf("%s", person->name);
                    if(i != lastIdx){
                        printf(" and ");
                    } else {
                        printf("\n");
                        freeSplit(splittedText);
                        return 0;
                    }
                }
                person = person->nextPersonPtr;
            }
        } else {
            freeSplit(splittedText);
            return -1;
        }
    }

    //if the question is subject total question
    else if( contains("total", splittedText) && splittedLength == 3 ){
        //printf("subject total question\n");
        if( strcmp(splittedText[1], "total")==0 && strcmp(splittedText[2], "?")==0 ){
            char *subject = splittedText[0];

            // Check if the subject is a valid subject
            if( isValidSubjectOrItem(subject)==0 ){
                freeSplit(splittedText);
                return -1;
            }

            struct Person *person = getPersonByName(firstPerson, subject);
            printInventory(person);
            freeSplit(splittedText);
            return 0;
        } else {
            freeSplit(splittedText);
            return -1;
        }
    }

    else{
        freeSplit(splittedText);
        return -1;
    }
    freeSplit(splittedText);
    return -1;
}