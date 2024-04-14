#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "header_file.h"

// Function to execute a single sentence
void singleSentenceExecutor(struct sentence *inputSentence, struct Person *firstPerson){
    //printf("Single sentence executor is called\n");

    // Parsing the input sentence
    char **subjects = inputSentence->subjects;
    char *action = inputSentence->action;
    char **objects = inputSentence->objects;
    char *to_or_from = inputSentence->to_or_from;

    //printf("Input sentence is succesfully parsed\n");

    // If the action is buy
    if( strcmp(action, "buy") == 0 ){
        //printf("Action is buy\n");
        // If to-or-from is NULL
        if (to_or_from == NULL){
            //printf("to_or_from is NULL\n");
            // Traverse all subjects and add the items to their inventory
            for(int i = 0; subjects[i] != NULL ; i++){
                struct Person *currentPerson = getPersonByName(firstPerson, subjects[i]);
                //printf("Current subject is %s\n", currentPerson->name);
                for(int j = 0; objects[j] != NULL ; j++){
                    // Get the object name and quantity
                    //printf("Object is %s\n", objects[j]);
                    char *itemName = (char *)malloc( strlen(objects[j]) * sizeof(char) );
                    int quantity;
                    sscanf(objects[j], "%d %s", &quantity, itemName);

                    //printf("Object parsed: %s with the quantity %d is going to be added\n", itemName, quantity);

                    // Add the item to the current person
                    addItem(currentPerson, itemName, quantity);

                    //printf("%s bought %d %s\n", currentPerson->name, quantity, itemName);
                    free(itemName);
                }
            }

        } else {
        // If to-or-from is not NULL
            //printf("to_or_from is not NULL\n");
            // Get number of buyers
            int numberOfBuyers = get_string_array_length(subjects);
            //printf("Number of buyers: %d\n", numberOfBuyers);
            struct Person *fromPerson = getPersonByName(firstPerson, to_or_from);

            // Traverse all objects and check if from person has enough for each of them
            //printf("Checking if the from person has enough of all items\n");
            int hasEnough = 1;
            for(int i = 0; objects[i] != NULL; i++){
                // Get the object name and quantity
                char *itemName = (char *)malloc( strlen(objects[i]) * sizeof(char) );
                int quantity;
                sscanf(objects[i], "%d %s", &quantity, itemName);

                // Check if the from person has enough of the item
                //printf("    Checking if %s has enough of %s (%d)\n", fromPerson->name, itemName, quantity*numberOfBuyers);
                hasEnough = hasEnoughItem(fromPerson, itemName, quantity * numberOfBuyers);
                free(itemName);
                //printf("        Has enough: %d\n", hasEnough);
                if(hasEnough == 0){
                    break;
                }
            }

            // If the from person has enough of all items
            if(hasEnough == 1){
                //printf("From person has enough of all items\n");
                // Traverse all subjects and add the items to their inventory, remove from fromPerson
                for(int i = 0; subjects[i] != NULL; i++){
                    struct Person *currentPerson = getPersonByName(firstPerson, subjects[i]);
                    for(int j = 0; objects[j] != NULL; j++){
                        // Get the object name and quantity
                        char *itemName = (char *)malloc( strlen(objects[j]) * sizeof(char) );
                        int quantity;
                        sscanf(objects[j], "%d %s", &quantity, itemName);

                        //printf("    Buying %d %s from %s\n", quantity, itemName, fromPerson->name);
                        // Add the item to the current person
                        addItem(currentPerson, itemName, quantity);
                        // Remove the item from the from person
                        removeItem(fromPerson, itemName, quantity);
                        free(itemName);
                        //printf("%s bought %d %s from %s\n", currentPerson->name, quantity, itemName, fromPerson->name);
                    }
                }
            }
            // If the from person does not have enough of all items
            else{
                //printf("%s does not have enough of all items\n", fromPerson->name);
            }
        }
    }

    // If the action is sell
    else if( strcmp(action, "sell") == 0){
        // For each seller, check if they have enough items to sell
        int hasEnough = 1;
        for(int i = 0; subjects[i] != NULL; i++){
            struct Person *currentPerson = getPersonByName(firstPerson, subjects[i]);
            for(int j = 0; objects[j] != NULL; j++){
                // Get the object name and quantity
                char *itemName = (char *)malloc( strlen(objects[j]) * sizeof(char) );
                int quantity;
                sscanf(objects[j], "%d %s", &quantity, itemName);

                // Check if the current person has enough of the item
                //printf("Checking if %s has enough of %s (%d)\n", currentPerson->name, itemName, quantity);
                hasEnough = hasEnoughItem(currentPerson, itemName, quantity);
                free(itemName);
                //printf("Has enough: %d\n", hasEnough);
                // Even if one of the items is not enough, break
                if (hasEnough == 0){
                    //printf("%s does not have enough of %s\n", currentPerson->name, itemName);
                    break;
                }
            }
            // Even if one seller doesnt have enough, break
            if(hasEnough == 0){
                break;
            }
        }

        // If all sellers have enough of all items
        if(hasEnough == 1){
            //printf("All sellers have enough of all items\n");
            // Sell the items from all sellers to the buyer (if exists)
            for(int i = 0; subjects[i] != NULL; i++){
                struct Person *currentPerson = getPersonByName(firstPerson, subjects[i]);
                for(int j = 0; objects[j] != NULL; j++){
                    // Get the object name and quantity
                    char *itemName = (char *)malloc( strlen(objects[j]) * sizeof(char) );
                    int quantity;
                    sscanf(objects[j], "%d %s", &quantity, itemName);

                    // Remove the item from the current person
                    removeItem(currentPerson, itemName, quantity);

                    // Add the item to the to person if exists
                    if (to_or_from != NULL){
                        struct Person *toPerson = getPersonByName(firstPerson, to_or_from);
                        addItem(toPerson, itemName, quantity);
                        //printf("%s sold %d %s to %s\n", currentPerson->name, quantity, itemName, toPerson->name);
                    } else {
                        //printf("%s sold %d %s\n", currentPerson->name, quantity, itemName);
                    }
                    free(itemName);
                }
            }
        } else {
            //printf("Not all sellers have enough of all items\n");
        } 
    }

    // If the action is go to
    else if (strcmp(action, "go to") == 0){
        // Get the location
        char *location = objects[0];

        // Change the location for all subjects
        for(int i = 0; subjects[i] != NULL; i++){
            struct Person *currentPerson = getPersonByName(firstPerson, subjects[i]);
            strcpy(currentPerson->location, location);
            //printf("%s went to %s\n", currentPerson->name, location);
        }
    }
}

// Function to check if a single condition is true or false, returns 1 if true, 0 if false
int singleConditionChecker(struct condition *inputCondition, struct Person *firstPerson){
    char **subjects = inputCondition->subjects;
    char *action = inputCondition->action;
    char **objects = inputCondition->objects;

    // If action is at
    if(strcmp(action, "at") == 0){
        // Get the location
        char *location = objects[0];

        // Check if all subjects are at the location
        for(int i = 0; subjects[i] != NULL; i++){
            struct Person *currentPerson = getPersonByName(firstPerson, subjects[i]);
            if(strcmp(currentPerson->location, location) != 0){
                return 0;
            }
        }
        return 1;
    }

    // If condition is has or has more than or has less than
    else if( strcmp(action, "has")==0 || strcmp(action, "has more than")==0 || strcmp(action, "has less than")==0 ){
        // For each subject, check if they satisfy the condition
        for( int i = 0; subjects[i] != NULL; i++ ){
            struct Person *currentPerson = getPersonByName(firstPerson, subjects[i]);
            for( int j = 0; objects[j] != NULL; j++ ){
                // Get the object name and quantity
                char *itemName = (char *)malloc( strlen(objects[j]) * sizeof(char) );
                int quantity;
                sscanf(objects[j], "%d %s", &quantity, itemName);

                // Check if the condition is satisfied
                if( strcmp(action, "has") == 0 && hasEnoughItem(currentPerson, itemName, quantity) == 0){
                    return 0;
                } else if( strcmp(action, "has more than") == 0 && hasMoreThanItem(currentPerson, itemName, quantity) == 0){
                    return 0;
                } else if( strcmp(action, "has less than") == 0 && hasLessThanItem(currentPerson, itemName, quantity) == 0){
                    return 0;
                }
                free(itemName);
            }
        }
        return 1;
    }
    return 0;
}

// Function to execute all sentences and conditions
void executor(struct sentences_conditions_pair **allPairs, struct Person *firstPerson){
    // For all pairs, check if the conditions are satisfied and execute the sentences
    for (int i = 0; allPairs[i]!=NULL; i++){
        struct sentences_conditions_pair *currentPair = allPairs[i];

        struct sentence **currentSentences = currentPair->sentences;
        struct condition **currentConditions = currentPair->conditions;

        // Check if the condition is satisfied
        //printf("Checking conditions\n");
        int conditionSatisfied = 1;
        if(currentConditions != NULL){
            for(int j = 0; currentConditions[j] != NULL; j++){
                //printf("Checking condition %d\n", j);
                struct condition *currentConditionStruct = currentConditions[j];
                if(singleConditionChecker(currentConditionStruct, firstPerson) == 0){
                    conditionSatisfied = 0;
                    break;
                }
            }
        } else {
            //printf("Conditions is NULL\n");
        }
        //printf("Condition satisfied: %d\n", conditionSatisfied);
        
        // If all conditons are satisfied, execute all sentences
        if(conditionSatisfied == 1){
            // Execute all sentences
            for(int j = 0; currentSentences[j] != NULL; j++){
                //printf("Executing sentence number %d\n", j);
                struct sentence *currentSentenceStruct = currentSentences[j];
                singleSentenceExecutor(currentSentenceStruct, firstPerson);
            }
            //printf("All sentences executed\n");
        } else {
            //printf("Conditions are not satisfied\n");
        }
    }
}