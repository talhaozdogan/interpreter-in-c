#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "header_file.h"

// util function for splitting the string
char **split(char *string, char *delimiter, int length)
{
  // allocate memory for the array of strings
  char **result = (char **)malloc((length + 1) * sizeof(char *));
  result[length] = NULL; // set the last element to NULL
  // split the string
  char *token = strtok(string, delimiter);
  // iterate over the tokens
  int i = 0;
  while (token != NULL)
  {
    // allocate memory for the token
    result[i] = (char *)malloc(length * sizeof(char));
    // copy the token to the result array
    strcpy(result[i], token);
    // get the next token
    token = strtok(NULL, delimiter);
    i++;
  }
  // set the last element to NULL
  result[i] = NULL;
  // return the result
  return result;
}

// util function for getting string array length
int get_string_array_length(char **array)
{
  int i = 0;
  while (array[i] != NULL)
  {
    i++;
  }
  return i;
}

// util function for checking if a string is in string array
// returns 1 if the string is in the array, 0 otherwise
int contains(char *string, char **array)
{
  int i = 0;
  while (array[i] != NULL && strcmp(array[i], "\0") != 0)
  {
    if (strcmp(string, array[i]) == 0)
    {
      return 1;
    }
    i++;
  }
  return 0;
}

// util function for checking if a string is a digit
// returns 1 if the string is a digit, 0 otherwise
int is_digit(char *string)
{
  int i = 0;
  while (string[i] != '\0')
  {
    if (string[i] < '0' || string[i] > '9')
    {
      return 0;
    }
    i++;
  }
  return 1;
}


// util function to trim the spaces and \n at the beginning and end of the string
// modifies the string, input must be a char array, mutability is required
// also trims the whitespaces before \n
void trim(char string[]){
  int start = 0;
  int end = strlen(string) - 1;
  while (string[start] == ' ' || string[start] == '\n')
  {
    start++;
  }
  while (string[end] == ' ' || string[end] == '\n')
  {
    end--;
  }
  int i = 0;
  for (int j = start; j <= end; j++)
  {
    string[i] = string[j];
    i++;
  }
  string[i] = '\0';
}
// util function for finding index of a string in a string array
int find_index(char *string, char **array)
{
  int i = 0;
  while (array[i] != NULL)
  {
    if (strcmp(string, array[i]) == 0)
    {
      return i;
    }
    i++;
  }
  return -1;
}

