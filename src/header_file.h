#ifndef HEADER_FILE_H
#define HEADER_FILE_H

#define MAX_ITEM_NAME_LENGTH 1040
#define MAX_NAME_LENGTH 1040
#define MAX_LOCATION_LENGTH 1040

// Item struct definition
struct Item
{
  char itemName[MAX_ITEM_NAME_LENGTH];
  int quantity;
  // Pointer to the next item
  struct Item *nextItemPtr;
};

// Person struct definition
struct Person
{
  char name[MAX_NAME_LENGTH];
  char location[MAX_LOCATION_LENGTH];
  // First item in the inventory
  struct Item dummyItem;
  // Pointer to the next person
  struct Person *nextPersonPtr;
};

// from parser.c
struct sentence
{
  // array of subjects
  char **subjects;
  // action of sentence, single
  char *action;
  // array of objects
  char **objects;
  // single to_or_from string
  char *to_or_from;
};

struct condition
{
  // array of subjects
  char **subjects;
  // action of sentence, single
  char *action;
  // array of objects
  char **objects;
};

// icnludes sentence array and condition array of a single statement part
struct sentences_conditions_pair
{
  // sentence array
  struct sentence **sentences;
  // condition array
  struct condition **conditions;
};

// functions
// from inventory_handler.c
void addItem(struct Person *person, char *itemName, int quantity);
void removeItem(struct Person *person, char *itemName, int quantity);
int getQuantity(struct Person *person, char *itemName);
int hasEnoughItem(struct Person *person, char *itemName, int quantity);
int hasExactItem(struct Person *person, char *itemName, int quantity);
int hasMoreThanItem(struct Person *person, char *itemName, int quantity);
int hasLessThanItem(struct Person *person, char *itemName, int quantity);
void changeLocation(struct Person *person, char *location);
int isAtLocation(struct Person *person, char *location);
char *getLocation(struct Person *person);
void printInventory(struct Person *person);
struct Person *getPersonByName(struct Person *firstPerson, char *name);

// from utils.c
int get_string_array_length(char **array);
void trim(char string[]);
int contains(char *string, char **array);
char **split(char *string, char *delimiter, int length);

// from execute.c
void executor(struct sentences_conditions_pair **allPairs, struct Person *firstPerson);

// form parser.c
struct sentences_conditions_pair **parser(char *statement, char ***res, char ****statement_parts, char ****final_sentences);
int isValidSubjectOrItem(char *word);
char **and_parser(char **sentence, int start_idx, int end_idx, int is_subjects, int check_for_numbers);
void free_memory(struct sentences_conditions_pair **pairs, char ***res, char ****statement_parts, char ****final_sentences);

// from question_handler.c
int question_handler(char *inputText, struct Person *firstPerson);

#endif