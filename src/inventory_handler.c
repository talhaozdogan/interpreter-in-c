#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "header_file.h"


// Function to add an item to a person's inventory
void addItem(struct Person *person, char *itemName, int quantity) {
    // Check if the item is in the inventory. if exists, update the quantity
    struct Item *currentItem = person->dummyItem.nextItemPtr;
    while (currentItem != NULL) {
        if (strcmp(currentItem->itemName, itemName) == 0) {
            currentItem->quantity += quantity;
            //printf("Updated %d %s to %s's inventory\n", currentItem->quantity, itemName, person->name);
            return;
        }
        currentItem = currentItem->nextItemPtr;
    }

    // If the item is not in the inventory, create a new item and add it to the inventory
    struct Item *newItem = (struct Item *)malloc(sizeof(struct Item));
    strcpy(newItem->itemName, itemName);
    newItem->quantity = quantity;
    newItem->nextItemPtr = NULL;
    //printf("New item: Added %d %s to %s's inventory\n", newItem->quantity, itemName, person->name);

    // Add the new item to the inventory as the next of the last item in the inventory
    struct Item *currentLastItem = &person->dummyItem;
    while (currentLastItem->nextItemPtr != NULL) {
        currentLastItem = currentLastItem->nextItemPtr;
    }
    currentLastItem->nextItemPtr = newItem;
}

// Function to remove an item from a person's inventory
void removeItem(struct Person *person, char*itemName, int quantity){
    // Check if the item is in the inventory
    struct Item *currentItem = person->dummyItem.nextItemPtr;
    while(currentItem != NULL){
        if(strcmp(currentItem->itemName, itemName) == 0){
            // If there are enough items to remove, remove; else, do not update the quantity
            if(quantity > currentItem->quantity){
                // Do not update the quantity
                //printf("Not enough %s in %s's inventory\n", itemName, person->name);
            } else {
                // If the quantity to remove is less than the quantity in the inventory, update the quantity
                currentItem->quantity -= quantity;
                //printf("Updated %d %s to %s's inventory\n", currentItem->quantity, itemName, person->name);
            }
            return;
        }
        currentItem = currentItem->nextItemPtr;
    }
    // If the item is not in the inventory, do not do anything
    //printf("%s is not in %s's inventory\n", itemName, person->name);
    return;
}

// Function to get the quantity of an item in a person's inventory, returns intenger
int getQuantity(struct Person *person, char *itemName){
    // Check if the item is in the inventory
    struct Item *currentItem = person->dummyItem.nextItemPtr;
    while(currentItem != NULL){
        if(strcmp(currentItem->itemName, itemName) == 0){
            //printf("%s has %d %s\n", person->name, currentItem->quantity, itemName);
            return currentItem->quantity;
        }
        currentItem = currentItem->nextItemPtr;
    }
    // If the item is not in the inventory
    //printf("%s does not have %s\n", person->name, itemName);
    return 0;
}

// Function to check if a person has enough items in the inventory, returns 1 if enough, 0 otherwise
int hasEnoughItem(struct Person *person, char *itemName, int quantity){
    int inventoryQuantity = getQuantity(person, itemName);
    if(inventoryQuantity >= quantity){
        //printf("%s has enough %s\n", person->name, itemName);
        return 1;
    }
    //printf("%s does not have enough %s\n", person->name, itemName);
    return 0;
}

// Function to check if a person has exactly "quantity" items, returns 1 if exact, 0 otherwise
int hasExactItem(struct Person *person, char *itemName, int quantity){
    int inventoryQuantity = getQuantity(person, itemName);
    if(inventoryQuantity == quantity){
        //printf("%s has exactly %d %s\n", person->name, quantity, itemName);
        return 1;
    }
    //printf("%s does not have exactly %d %s\n", person->name, quantity, itemName);
    return 0;
}

// Function to check if a person has more than "quantity" items, returns 1 if more, 0 otherwise
int hasMoreThanItem(struct Person *person, char *itemName, int quantity){
    int inventoryQuantity = getQuantity(person, itemName);
    if(inventoryQuantity > quantity){
        //printf("%s has more than %d %s\n", person->name, quantity, itemName);
        return 1;
    }
    //printf("%s does not have more than %d %s\n", person->name, quantity, itemName);
    return 0;
}

// Function to check if a person has less than "quantity" items, returns 1 if less, 0 otherwise
int hasLessThanItem(struct Person *person, char *itemName, int quantity){
    int inventoryQuantity = getQuantity(person, itemName);
    if(inventoryQuantity < quantity){
        //printf("%s has less than %d %s\n", person->name, quantity, itemName);
        return 1;
    }
    //printf("%s does not have less than %d %s\n", person->name, quantity, itemName);
    return 0;
}

// Function to change the location of a person
void changeLocation(struct Person *person, char *location){
    strcpy(person->location, location);
    //printf("%s is now at %s\n", person->name, person->location);
}

// Function to get the location of a person, returns the location, if not set, returns "NOWHERE"
char *getLocation(struct Person *person){
    //TODO: default location is "NOWHERE"
    if( strcmp(person->location, "NOWHERE") == 0 ){
        //printf("%s's location is not set\n", person->name);
        return "NOWHERE";
    }
    //printf("%s is at %s\n", person->name, person->location);
    return person->location;
}

// Function to check if a person is at a location, returns 1 if at the location, 0 otherwise; uses getLocation function
int isAtLocation(struct Person *person, char *location){
    if(strcmp( getLocation(person) , location) == 0){
        //printf("%s is at %s\n", person->name, location);
        return 1;
    }
    //printf("%s is not at %s\n", person->name, location);
    return 0;
}

// Function to print the inventory of a person
void printInventory(struct Person *person){
    struct Item *currentItem = person->dummyItem.nextItemPtr;
    
    // First traverse to find the last non-zero item
    int lastIdx = -1;
    for(int i=0; currentItem != NULL; i++){
        if(currentItem->quantity > 0){
            lastIdx = i;
        }
        currentItem = currentItem->nextItemPtr;
    }

    if(lastIdx == -1){
        printf("NOTHING\n");
        return;
    }

    // Print the items
    currentItem = person->dummyItem.nextItemPtr;
    for(int i=0; currentItem != NULL; i++){
        if( currentItem->quantity > 0 ){
            printf("%d %s", currentItem->quantity, currentItem->itemName);
            if(i != lastIdx){
                printf(" and ");
            } else {
                printf("\n");
            }
        }
        
        currentItem = currentItem->nextItemPtr;
    }
}

// Function to get a person by name, returns pointer to the person; if not found, creates a new person
struct Person *getPersonByName(struct Person *firstPerson, char *name){
    struct Person *currentPerson = firstPerson->nextPersonPtr;
    while(currentPerson != NULL){
        //printf("Checking %s\n", currentPerson->name);
        if(strcmp(currentPerson->name, name) == 0){
            //printf("Found %s\n", name);
            return currentPerson;
        }
        currentPerson = currentPerson->nextPersonPtr;
    }
    //printf("Could not find %s, new one created\n", name);
    
    // If the person is not found, create a new person
    struct Person *newPerson = (struct Person *)malloc(sizeof(struct Person));
    strcpy(newPerson->name, name);
    strcpy(newPerson->location, "NOWHERE");
    newPerson->dummyItem.nextItemPtr = NULL;
    newPerson->nextPersonPtr = NULL;

    // Connect the new person to the last person
    struct Person *currentLastPerson = firstPerson;
    while(currentLastPerson->nextPersonPtr != NULL){
        currentLastPerson = currentLastPerson->nextPersonPtr;
    }
    currentLastPerson->nextPersonPtr = newPerson;
    return newPerson;

}