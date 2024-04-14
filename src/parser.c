#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

const int MAX_LENGTH = 1025;
char *all_actions[11] = {"buy", "sell", "buy from", "sell to", "go to", "has", "at", "has more than", "has less than", "go", "\0"};
char *keywords[20] = {"sell", "buy", "go", "to", "from", "and", "at", "has", "if", "less", "more", "than", "exit", "where", "total", "who",
                      "NOBODY", "NOTHING", "NOWHERE", "\0"};
char *if_actions[5] = {"has", "has more than", "has less than", "at", "\0"};

char *seperator = ";";

// import split from utils
char **split(char *string, char *delimiter, int length);
// import get_string_array_length from utils
int get_string_array_length(char **array);
// import contains from utils
int contains(char *string, char **array);
// import is_digit from utils
int is_digit(char *string);
// import find_index from utils
int find_index(char *string, char **array);

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
  // flag for invalid syntax check
  // 0 for invalid, 1 for valid
  int is_valid;
  // flag for avoiding double free
  int can_free_objects;
};

struct condition
{
  // array of subjects
  char **subjects;
  // action of sentence, single
  char *action;
  // array of objects
  char **objects;
  // flag for invalid syntax check
  // 0 for invalid, 1 for valid
  int is_valid;
  // flag for avoiding double free
  int can_free_objects;
};

// includes sentence array and condition array of a single statement part
struct sentences_conditions_pair
{
  // array of sentence Struct
  struct sentence **sentences;
  // condition array
  struct condition **conditions;
  // flag for invalid syntax check
  // 0 for invalid, 1 for valid
  int is_valid;
};

int isValidSubjectOrItem(char *word)
{
  // Check if the word is a keyword
  if (contains(word, keywords) == 1)
  {
    //                printf("aa_isValidSubjectOrItem 1\n");
    return 0;
  }
  int word_len = strlen(word);
  for (int i = 0; i < word_len; i++)
  {
    if (!(isalpha(word[i]) != 0 || word[i] == '_'))
    {
      //                      printf("Problematic word is : %s\n", word);
      //                      printf("aa_isValidSubjectOrItem 2\n");
      return 0;
    }
  }
  return 1;
}

// function for removing and from a word array and validating the array that could be a subject or item array
char **and_parser(char **sentence, int start_idx, int end_idx, int is_subjects, int check_for_numbers)
{
  int sentence_len = end_idx - start_idx;
  // Assert sentence length is non-zero
  if (sentence_len == 0)
  {
    //                printf("and_parser error 0\n");
    return NULL;
  }
  // Find number of ands in a sentence
  int no_of_ands = 0;
  for (int j = start_idx; j < end_idx; j++)
  {
    if (strcmp(sentence[j], "and") == 0)
    {
      no_of_ands += 1;
    }
  }
  // The sentence length after ands are removed
  int editted_len = end_idx - start_idx - no_of_ands;
  // Allocate memory for new array without ands
  char **new_words = (char **)malloc((editted_len + 1) * sizeof(char *));
  new_words[editted_len] = NULL;
  int new_words_idx = 0;
  // ASSERT format is correct: A and B and C
  if (is_subjects == 1 && sentence_len > 1 && no_of_ands == 0)
  {
    //                printf("aa1\n");
    free(new_words);
    return NULL;
  }
  int last_and_idx = -1;
  for (int i = start_idx; i < end_idx; i++)
  {
    char *word = sentence[i];
    if (strcmp(word, "and") == 0)
    {
      // Assert and is not at the and of word_array
      if (i == end_idx - 1)
      {
        //                   printf("and_parser error 0\n");
        free(new_words);
        return NULL;
      }
      int cur_and_idx = i - start_idx;
      if (cur_and_idx - last_and_idx > 3 || cur_and_idx - last_and_idx == 1)
      {
        //                            printf("and_parser error 1\n");
        free(new_words);
        return NULL;
      }
      else if (cur_and_idx - last_and_idx == 2)
      {
        // assert the format is: and WORD and
        if (isValidSubjectOrItem(sentence[start_idx + cur_and_idx - 1]) == 0)
        {
          //                                  printf("and_parser error 2.1\n");
          free(new_words);
          return NULL;
        }
      }
      else if (cur_and_idx - last_and_idx == 3)
      {
        // assert the format is: and NUMBER WORD and
        if (is_digit(sentence[start_idx + cur_and_idx - 2]) == 0 || isValidSubjectOrItem(sentence[start_idx + cur_and_idx - 1]) == 0)
        {
          //                                  printf("and_parser error 2.2\n");
          free(new_words);
          return NULL;
        }
      }
      last_and_idx = cur_and_idx;
    }
    else if (contains(word, keywords) == 1)
    {
      //                      printf("the word in keywords is: %s\n", word);
      //                      printf("and_parser error 3\n");
      free(new_words);
      return NULL;
    }
    else
    {
      if (check_for_numbers == 1 && !(isValidSubjectOrItem(word) == 1))
      {
        //                            printf("and_parser error 4\n");
        free(new_words);
        return NULL;
      }
      new_words[new_words_idx] = word;
      new_words_idx++;
    }
  }
  return new_words;
}

// function for grouping numbered items
char **group_numbered_items(char **items)
{
  // Assert there is even number of words
  int items_len = get_string_array_length(items);
  if (items_len % 2 != 0)
  {
    //                printf("gorup_numbered_items error 1\n");
    return NULL;
  }

  // Assert there is no repeated objects
  int k = 0;
  while (items[k] != NULL)
  {
    char *word = items[k];
    if (is_digit(word) == 0 && items[k + 1] != NULL)
    {
      if (contains(word, items + k + 1) == 1)
      {
        //                      printf("group_numbered_items error 0.99\n");
        free(items);
        return NULL;
      }
    }
    k++;
  }

  char **grouped_items = (char **)malloc(((get_string_array_length(items) / 2) + 1) * sizeof(char *));
  grouped_items[get_string_array_length(items) / 2] = NULL;
  for (int i = 0; i < items_len; i += 2)
  {
    // Assert the first word is a number
    // Assert the second word is a valid subject or item
    if (is_digit(items[i]) == 0 || isValidSubjectOrItem(items[i + 1]) == 0)
    {
      //                      printf("group_numbered_items error 2\n");
      //                      printf("group_numbered_items error 3\n");
      // Free all allocated memory in this function
      int k = 0;
      while (i > 0 && grouped_items[k] != NULL)
      {
        free(grouped_items[k]);
        k++;
      }
      free(grouped_items);
      free(items);
      return NULL;
    }
    int number_len = strlen(items[i]);
    int word_len = strlen(items[i + 1]);
    int grouped_len = number_len + 1 + word_len;
    //                printf("strelen is : %d\n", grouped_len);
    char *res = (char *)malloc((grouped_len + 1) * sizeof(char));
    res[grouped_len] = '\0';
    for (int k = 0; k < number_len; k++)
    {
      res[k] = items[i][k];
    }
    res[number_len] = ' ';
    for (int k = number_len + 1; k < grouped_len; k++)
    {
      res[k] = items[i + 1][k - number_len - 1];
    }
    grouped_items[i / 2] = res;
    grouped_items[(i / 2) + 1] = NULL;
  }
  free(items);
  return grouped_items;
}

// parses the statement in to little parts by returning the indices of and which
// connects the struct sentences_conditions_pair
// for example talha buy 2 apple and 3 bread and hikmet go to antalya returns {7} but not {4, 7}
// since the first and is a part of little statement part and a part of sentences_conditions_pair struct
int *statement_parser(char **all_words, int word_length, int *no_of_big_ands)
{
  // big and refers to ands which connects the struct sentences_conditions_pair
  int *big_and_indices = (int *)malloc((word_length + 1) * sizeof(int));
  big_and_indices[word_length] = -1;
  *no_of_big_ands = 0;
  int next_big_and = 0;        // looking for a big and after action
  int in_if = 0;               // if we are in a if statement
  int last_big_and_index = -1; // helps to append big and after if statement
  int action_after_and = 0;

  for (int i = 0; i < word_length; i++)
  {
    char *word = all_words[i];
    if (strcmp(word, "if") == 0)
    {
      in_if = 1;
      next_big_and = 0;
      action_after_and = 0;
    }
    // The word is an action and next we are looking for a big and
    else if (contains(word, all_actions) == 1)
    {
      next_big_and = 1;
      if (in_if == 1)
      {
        action_after_and = 1;
        if (contains(word, if_actions) == 0 && last_big_and_index != -1)
        {
          in_if = 0;
          big_and_indices[*no_of_big_ands] = last_big_and_index;
          *no_of_big_ands += 1;
        }
      }
    }
    // Check if the and connects two items
    else if (strcmp(word, "and") == 0 && (next_big_and == 1))
    {
      if (!(i + 1 < word_length && is_digit(all_words[i + 1]) == 1))
      {
        if (in_if == 1)
        {
          if (action_after_and == 1)
          {
            last_big_and_index = i;
            action_after_and = 0;
          }
        }
        else
        {

          big_and_indices[*no_of_big_ands] = i;
          *no_of_big_ands += 1;
          next_big_and = 0;
        }
      }
    }
  }
  return big_and_indices;
}

// Returns a sentence struct containing a single action
struct sentence *little_sentence_parser(char **sentence, int start_idx, int end_idx)
{
  //          printf("Starting the sentence with word: %s\n", sentence[start_idx]);
  //          printf("Ending the sentence with word: %s\n", sentence[end_idx - 1]);
  struct sentence *parsed_sentence = (struct sentence *)malloc(sizeof(struct sentence));
  int action_index = -1;
  for (int i = start_idx; i < end_idx; i++)
  {
    char *word = sentence[i];
    if (contains(word, all_actions) == 1 && contains(word, if_actions) == 0)
    {
      parsed_sentence->to_or_from = NULL;
      if (strcmp(word, "go") == 0)
      {
        // there must be a 'to' afterwards
        // ASSERT next word is 'to'
        if ((i == end_idx - 1) || strcmp(sentence[i + 1], "to") != 0)
        {
          //                                  printf("aa4\n");
          free(parsed_sentence);
          return NULL;
        }
        parsed_sentence->action = "go to";
        // Assert only one word after go to
        if (i + 2 != end_idx - 1)
        {
          //                                  printf("i + 2 is %d and end_idx - start_idx - 1 is %d\n", i + 2, end_idx - 1);
          //                                  printf("aa5\n");
          free(parsed_sentence);
          return NULL;
        }
        // Assert word after to is valid
        int isValid = isValidSubjectOrItem(sentence[i + 2]);
        if (isValid == 0)
        {
          //                                  printf("aa6\n");
          free(parsed_sentence);
          return NULL;
        }
        int objects_word_count = end_idx - (i + 2);
        char **objects = (char **)malloc((objects_word_count + 1) * sizeof(char *));
        objects[objects_word_count] = NULL;
        for (int j = 0; j < objects_word_count; j++)
        {
          objects[j] = sentence[i + 2 + j];
        }
        parsed_sentence->objects = objects;
        parsed_sentence->can_free_objects = 0;
      }

      if (strcmp(word, "buy") == 0)
      {
        parsed_sentence->action = "buy";
        int from_index = -1;
        for (int j = i + 1; j < end_idx; j++)
        {
          if (strcmp(sentence[j], "from") == 0)
          {
            from_index = j;
            break;
          }
        }
        if (from_index != -1)
        {
          // Assert there is only one word after from
          if (from_index + 2 != end_idx)
          {
            //                                        printf("aa9\n");
            free(parsed_sentence);
            return NULL;
          }
          // Assert the word after from is valid
          if (isValidSubjectOrItem(sentence[from_index + 1]) == 0)
          {
            //                                        printf("aa10\n");
            free(parsed_sentence);
            return NULL;
          }
          parsed_sentence->to_or_from = sentence[from_index + 1];
        }
        int parser_end_idx = (from_index == -1) ? end_idx : from_index;
        char **objects_without_ands = and_parser(sentence, i + 1, parser_end_idx, 0, 0);
        if (objects_without_ands == NULL)
        {
          //                                  printf("aa7\n");
          free(parsed_sentence);
          return NULL;
        }
        // Checked and corrected for numbers
        char **final_objects = group_numbered_items(objects_without_ands);
        if (final_objects == NULL)
        {
          //                                  printf("aa8\n");
          free(parsed_sentence);
          return NULL;
        }
        parsed_sentence->objects = final_objects;
        parsed_sentence->can_free_objects = 1;
      }

      if (strcmp(word, "sell") == 0)
      {
        parsed_sentence->action = "sell";
        int to_index = -1;
        for (int j = i + 1; j < end_idx; j++)
        {
          if (strcmp(sentence[j], "to") == 0)
          {
            to_index = j;
            break;
          }
        }
        if (to_index != -1)
        {
          // Assert there is only one word after to
          if (to_index + 2 != end_idx)
          {
            //                                        printf("aa13\n");
            free(parsed_sentence);
            return NULL;
          }
          // Assert the word after to is valid
          if (isValidSubjectOrItem(sentence[to_index + 1]) == 0)
          {
            //                                        printf("aa14\n");
            free(parsed_sentence);
            return NULL;
          }
          parsed_sentence->to_or_from = sentence[to_index + 1];
        }
        int parser_end_idx = (to_index == -1) ? end_idx : to_index;
        char **objects_without_ands = and_parser(sentence, i + 1, parser_end_idx, 0, 0);
        if (objects_without_ands == NULL)
        {
          //                                  printf("aa11\n");
          free(parsed_sentence);
          return NULL;
        }
        // Checked and corrected for numbers
        char **final_objects = group_numbered_items(objects_without_ands);
        if (final_objects == NULL)
        {
          //                                  printf("aa12\n");
          free(parsed_sentence);
          return NULL;
        }
        parsed_sentence->objects = final_objects;
        parsed_sentence->can_free_objects = 1;
      }
      action_index = i;
      break;
    }
  }
  // Assert there is an action in the sentence
  if (action_index == -1)
  {
    //                printf("aa15\n");
    free(parsed_sentence);
    return NULL;
  }
  char **subjects = and_parser(sentence, start_idx, action_index, 1, 1);
  if (subjects == NULL)
  {
    //                printf("aa16\n");
    // Free objects
    if (parsed_sentence->can_free_objects == 1)
    {
      int k = 0;
      while (parsed_sentence->objects[k] != NULL)
      {
        free(parsed_sentence->objects[k]);
        k++;
      }
    }
    free(parsed_sentence->objects);
    free(parsed_sentence);
    return NULL;
  }
  parsed_sentence->subjects = subjects;
  // Assert to_or_from is not in subjects
  if (parsed_sentence->to_or_from != NULL && contains(parsed_sentence->to_or_from, parsed_sentence->subjects) == 1)
  {
    //                printf("aa17\n");
    // Free objects
    if (parsed_sentence->can_free_objects == 1)
    {
      int k = 0;
      while (parsed_sentence->objects[k] != NULL)
      {
        free(parsed_sentence->objects[k]);
        k++;
      }
    }
    free(parsed_sentence->objects);
    // Free subjects
    free(subjects);
    free(parsed_sentence);
    return NULL;
  }

  int k = 0;
  while (subjects[k] != NULL)
  {
    char *subject = subjects[k];
    if (subjects[k + 1] != NULL && contains(subject, (subjects + k + 1)) == 1)
    {
      // Free objects
      if (parsed_sentence->can_free_objects == 1)
      {
        int k = 0;
        while (parsed_sentence->objects[k] != NULL)
        {
          free(parsed_sentence->objects[k]);
          k++;
        }
      }
      free(parsed_sentence->objects);
      // Free subjects
      free(subjects);
      free(parsed_sentence);
      return NULL;
    }
    k++;
  }

  return parsed_sentence;
}

struct condition *if_sentence_parser(char **sentence, int start_idx, int end_idx)
{
  //          printf("Starting the condition with word: %s\n", sentence[start_idx]);
  //          printf("Ending the condition with word: %s\n", sentence[end_idx - 1]);
  struct condition *cond = (struct condition *)malloc(sizeof(struct condition));
  int action_index = -1;
  for (int i = start_idx; i < end_idx; i++)
  {
    char *word = sentence[i];
    if (contains(word, if_actions) == 1)
    {
      action_index = i;
      if (strcmp(word, "at") == 0)
      {
        cond->action = "at";
        // Assert there is only one word after at
        if (i + 2 != end_idx)
        {
          //                                  printf("aa18\n");
          free(cond);
          return NULL;
        }
        // Assert the word after at is valid
        if (isValidSubjectOrItem(sentence[i + 1]) == 0)
        {
          //                                  printf("aa19\n");
          free(cond);
          return NULL;
        }
        char **null_terminated_objects = (char **)malloc(2 * sizeof(char *));
        null_terminated_objects[1] = NULL;
        null_terminated_objects[0] = sentence[i + 1];
        cond->objects = null_terminated_objects;
        cond->can_free_objects = 0;
      }
      else if (strcmp(word, "has") == 0)
      {
        int obj_start_index = i + 1;
        cond->action = "has";
        if (strcmp(sentence[i + 1], "more") == 0 || strcmp(sentence[i + 1], "less") == 0)
        {
          obj_start_index += 2;
          // Assert there is than after "more" or "less"
          if (strcmp(sentence[i + 2], "than") != 0)
          {
            //                                        printf("aa20\n");
            free(cond);
            return NULL;
          }
          if (strcmp(sentence[i + 1], "more") == 0)
          {
            cond->action = "has more than";
          }
          else
          {
            cond->action = "has less than";
          }
        }
        char **objects_without_ands = and_parser(sentence, obj_start_index, end_idx, 0, 0);
        if (objects_without_ands == NULL)
        {
          //                                  printf("aa21\n");
          free(cond);
          return NULL;
        }
        char **grouped_objects = group_numbered_items(objects_without_ands);
        if (grouped_objects == NULL)
        {
          //                                  printf("aa22\n");
          free(cond);
          return NULL;
        }
        cond->objects = grouped_objects;
        cond->can_free_objects = 1;
      }
      break;
    }
  }
  if (action_index == -1)
  {
    //                printf("aa23\n");
    free(cond);
    return NULL;
  }
  char **subjects = and_parser(sentence, start_idx, action_index, 1, 1);
  if (subjects == NULL)
  {
    //                printf("aa24\n");
    // Free objects if they can be freed
    if (cond->can_free_objects == 1)
    {
      int k = 0;
      while (cond->objects[k] != NULL)
      {
        free(cond->objects[k]);
        k++;
      }
    }
    free(cond->objects);
    free(cond);
    return NULL;
  }

  int k = 0;
  while (subjects[k] != NULL)
  {
    char *subject = subjects[k];
    if (subjects[k + 1] != NULL && contains(subject, (subjects + k + 1)) == 1)
    {
      // Free objects
      if (cond->can_free_objects == 1)
      {
        int k = 0;
        while (cond->objects[k] != NULL)
        {
          free(cond->objects[k]);
          k++;
        }
      }
      free(cond->objects);
      // Free subjects
      free(subjects);
      free(cond);
      return NULL;
    }
    k++;
  }
  cond->subjects = subjects;
  return cond;
}

// function that takes a big sentence that consists of many actions and multiple conditions
// and directs them to corresponding parser functions
struct sentences_conditions_pair *sentence_parser(char **sentence, int sentence_length)
{
  struct sentences_conditions_pair *pair = (struct sentences_conditions_pair *)malloc(sizeof(struct sentences_conditions_pair));
  pair->conditions = NULL;

  // Check if contains any conditions
  int if_index = -1;
  if (contains("if", sentence) == 1)
  {
    if_index = find_index("if", sentence);
  }
  //          printf("if_index: %d\n", if_index);

  // Check how many little sentences that contains a single action exists
  // by checking the number of seperators and if_index
  int no_of_little_sentences = 1;
  for (int i = 0; i < sentence_length; i++)
  {
    if (sentence[i] == NULL)
      break;
    else if (strcmp(sentence[i], seperator) == 0)
      no_of_little_sentences++;
    else if (i == if_index)
      break;
  }
  //          printf("no_of_little_sentences: %d\n", no_of_little_sentences);

  // Allocate memory for the little sentences
  struct sentence **little_sentences = (struct sentence **)malloc((no_of_little_sentences + 1) * sizeof(struct sentence *));
  little_sentences[no_of_little_sentences] = NULL;

  int start_idx = 0;
  int end_idx = 0;
  for (int i = 0; i < no_of_little_sentences; i++)
  {
    while (sentence[end_idx] != NULL && strcmp(sentence[end_idx], seperator) != 0 && end_idx != if_index)
    {
      end_idx++;
    }
    struct sentence *little_sentence = little_sentence_parser(sentence, start_idx, end_idx);
    if (little_sentence == NULL)
    {
      //                      printf("Error: little sentence is null \n");
      // free all memmory allocated for little_sentences
      int k = 0;
      while (i > 0 && little_sentences[k] != NULL)
      {
        free(little_sentences[k]);
        k++;
      }
      free(little_sentences);
      return NULL; /* TESTING dont forget to remove return value*/
    }
    else
    {
      // TESTING
      //                      printf("Printing little sentence no: %d\n", i);
      //                      printf("The action is: %s\n", little_sentence->action);
      //                      printf("The to_or_from is: %s\n", little_sentence->to_or_from);
      //                      printf("The subjects are: \n");
      int j = 0;
      while (little_sentence->subjects[j] != NULL)
      {
        //                            printf("subject[%d]: %s\n", j, little_sentence->subjects[j]);
        j++;
      }
      //                      printf("The objects are: \n");
      j = 0;
      while (little_sentence->objects[j] != NULL)
      {
        //                            printf("object[%d]: %s\n", j, little_sentence->objects[j]);
        j++;
      }
      // END OF TESTING
    }
    little_sentences[i] = little_sentence;
    little_sentences[i + 1] = NULL;
    start_idx = end_idx + 1;
    end_idx += 1;
  }
  pair->sentences = little_sentences;

  // Handle if part
  if (if_index != -1)
  {
    // Split the part after if
    // Find all the indices that seperates actions and number of conditions
    int big_and_counter = 0;
    int next_big_and = 0;
    int big_and_indices[sentence_length - if_index - 1];
    //            printf("sentence_length: %d\n", sentence_length);
    for (int i = if_index + 1; i < sentence_length; i++)
    {
      //                printf("sentence[%d] is %s \n", i, sentence[i]);
      if (contains(sentence[i], if_actions) == 1)
        next_big_and = 1;
      else if (strcmp(sentence[i], "and") == 0 && next_big_and == 1)
      {
        if (!((i + 1 < sentence_length) && is_digit(sentence[i + 1]) == 1))
        {
          big_and_indices[big_and_counter++] = i;
          next_big_and = 0;
        }
      }
    }
    int condition_count = big_and_counter + 1;

    // Allocate memory for the conditions
    struct condition **conditions = (struct condition **)malloc((condition_count + 1) * sizeof(struct condition *));
    conditions[condition_count] = NULL;
    int condition_start_idx = if_index + 1;
    for (int i = 0; i < condition_count; i++)
    {
      int condition_end_idx = (i == condition_count - 1) ? (sentence_length - 1) : big_and_indices[i];
      struct condition *condition = if_sentence_parser(sentence, condition_start_idx, condition_end_idx);
      if (condition == NULL)
      {
        //                            printf("Error: condition is null \n");
        // Free all memmory allocated for conditions
        int k = 0;
        while (i > 0 && conditions[k] != NULL)
        {
          free(conditions[k]);
          k++;
        }
        free(conditions);
        return NULL; /* TESTING dont forget to remove return value */
      }
      /*TESTING*/
      //                      printf("Printing condition no: %d\n", i);
      //                      printf("The action is: %s\n", condition->action);
      //                      printf("The subjects are: \n");
      int j = 0;
      while (condition->subjects[j] != NULL)
      {
        //                            printf("subject[%d]: %s\n", j, condition->subjects[j]);
        j++;
      }
      //                      printf("The objects are: \n");
      j = 0;
      while (condition->objects[j] != NULL)
      {
        //         printf("here\n");
        //                            printf("object[%d]: %s\n", j, condition->objects[j]);
        j++;
      }

      /*END OF TESTING*/
      //                      printf("condition_counter: %d\n", condition_count);
      conditions[i] = condition;
      conditions[i + 1] = NULL;
      condition_start_idx = condition_end_idx + 1;
    }
    pair->conditions = conditions;
  }

  return pair;
}

// return struct sentences_conditions_pair
struct sentences_conditions_pair **parser(char *statement, char ***res, char ****statement_parts, char ****final_sentences)
{
  struct sentences_conditions_pair *returned;
  // get the length of statement
  int char_length = strlen(statement);
  //          printf("char_length: %d\n", char_length);
  // split the statememt using strtok
  *res = split(statement, " ", char_length);
  // print the content of res, until reaching NULL
  int i = 0;
  while ((*res)[i] != NULL)
  {
    //            printf("res[%d]: %s\n", i, (*res)[i]);
    i++;
  }
  // get the length of the words
  int no_of_words = get_string_array_length(*res);
  //          printf("no_of_words: %d\n", no_of_words);
  // find big ands which connect sentences_conditions_pair
  int no_of_big_ands = 0;
  int *big_and_indices = statement_parser(*res, no_of_words, &no_of_big_ands);
  // print indices in int array
  for (int i = 0; i < no_of_big_ands; i++)
  {
    if (big_and_indices[i] == 0)
      break;
    //            printf("big_and_indices[%d]: %d\n", i, big_and_indices[i]);
  }

  //          printf("no_of_big_ands: %d\n", no_of_big_ands);

  int start_index = 0;
  *statement_parts = (char ***)malloc((no_of_big_ands + 2) * sizeof(char **));
  (*statement_parts)[no_of_big_ands + 1] = NULL;
  for (int i = 0; i < no_of_big_ands; i++)
  {
    int end_index = big_and_indices[i];
    (*statement_parts)[i] = (char **)malloc((end_index - start_index + 1) * sizeof(char *));
    (*statement_parts)[i][end_index - start_index] = NULL;
    for (int j = 0; j < end_index - start_index; j++)
    {
      (*statement_parts)[i][j] = (*res)[start_index + j];
    }
    start_index = end_index + 1;
  }
  free(big_and_indices);
  (*statement_parts)[no_of_big_ands] = (char **)malloc((no_of_words - start_index + 1) * sizeof(char *));
  (*statement_parts)[no_of_big_ands][no_of_words - start_index] = NULL;
  for (int j = 0; j < no_of_words - start_index; j++)
  {
    (*statement_parts)[no_of_big_ands][j] = (*res)[start_index + j];
  }

  // find and store all the indices of sentences which contain if
  int if_indices[no_of_words + 1];
  if_indices[no_of_words] = -1;
  int if_counter = 0; // number of if statements
  for (int i = 0; i < no_of_big_ands + 1; i++)
  {
    if (contains("if", (*statement_parts)[i]) == 1)
    {
      if_indices[if_counter] = i;
      if_counter++;
    }
  }

  /* TESTING */
  //        printf("if counter: %d\n", if_counter);
  for (int i = 0; i < if_counter; i++)
  {
    //                printf("if_indices[%d]: %d\n", i, if_indices[i]);
  }

  // final sentences is the part where if statements and sentences are grouped
  int sentences_after_last_if = 0;
  if (if_counter != 0)
  {
    // Calculate additional sentences after last if statement
    sentences_after_last_if += no_of_big_ands - if_indices[if_counter - 1];
  }
  else
  {
    sentences_after_last_if += no_of_big_ands + 1;
  }
  //        printf("sentences_after_last_if: %d\n", sentences_after_last_if);
  *final_sentences = (char ***)malloc((if_counter + sentences_after_last_if + 1) * sizeof(char **));
  (*final_sentences)[if_counter + sentences_after_last_if] = NULL;

  start_index = 0;
  for (int i = 0; i < if_counter; i++)
  {
    int if_index = if_indices[i];
    // find how many words exist till the if statement
    int word_count = 0;
    for (int j = start_index; j <= if_index; j++)
    {
      char **current_sentece = (*statement_parts)[j];
      int current_word_index = 0;
      while (current_sentece[current_word_index] != NULL)
      {
        // print the word
        //                    printf("current_sentece[%d]: %s\n", current_word_index, current_sentece[current_word_index]);
        word_count++;
        current_word_index++;
      }
      word_count++;
      // since we will add seperator between each sentence we need to increase word count by sentence count
    }
    // using the number of words, we can allocate memory for the words including the
    // if statement up to if_index
    //                printf("word_count: %d\n", word_count);
    char **concatted = (char **)malloc((word_count + 1) * sizeof(char *));
    concatted[word_count] = NULL;

    // concat the words including if statement
    int concatted_word_count = 0;
    for (int j = start_index; j <= if_index; j++)
    {
      char **current_sentence = (*statement_parts)[j];
      int current_word_index = 0;
      while (current_sentence[current_word_index] != NULL)
      {
        concatted[concatted_word_count] = current_sentence[current_word_index];
        current_word_index++;
        concatted_word_count++;
      }
      concatted[concatted_word_count] = seperator;
      concatted_word_count++;
    }

    (*final_sentences)[i] = concatted;
    start_index = if_index + 1;
  }
  if (if_counter == 0 || if_indices[if_counter - 1] != no_of_big_ands)
  {
    for (int k = 0; k < sentences_after_last_if; k++)
    {
      // first find word counter
      int word_count = 0;
      char **current_sentence = (*statement_parts)[start_index + k];
      int current_word_index = 0;
      while (current_sentence[current_word_index] != NULL)
      {
        word_count++;
        current_word_index++;
      }

      char **sentence = (char **)malloc((word_count + 1) * sizeof(char *));
      sentence[word_count] = NULL;

      // assign the words to the counter
      for (int h = 0; h < word_count; h++)
      {
        sentence[h] = (*statement_parts)[start_index + k][h];
      }
      (*final_sentences)[if_counter + k] = sentence;
    }
  }
  int final_sentences_length = if_counter + sentences_after_last_if;
  //          printf("final_sentences_length: %d\n", final_sentences_length);
  // create an array for holding the length of sentences
  int final_sentences_lengths[final_sentences_length];
  /* TESTING */
  //          printf("printing final sentences\n");
  for (int a = 0; a < final_sentences_length; a++)
  {
    int b = 0;
    while ((*final_sentences)[a][b] != NULL)
    {
      //                printf("(*final_sentences)[%d][%d]: %s\n", a, b, (*final_sentences)[a][b]);
      b++;
    }
    final_sentences_lengths[a] = b;
    //            printf("final_sentences_lengths[%d]: %d\n", a, final_sentences_lengths[a]);
  }
  struct sentences_conditions_pair **parser_return_arr;
  parser_return_arr = (struct sentences_conditions_pair **)malloc((final_sentences_length + 1) * sizeof(struct sentences_conditions_pair *));
  parser_return_arr[final_sentences_length] = NULL;
  for (int i = 0; i < final_sentences_length; i++)
  {
    char **current_sentence = (*final_sentences)[i];
    int current_sentence_length = final_sentences_lengths[i];
    struct sentences_conditions_pair *pair = sentence_parser(current_sentence, current_sentence_length);
    if (pair != NULL)
    {
      parser_return_arr[i] = pair;
      parser_return_arr[i + 1] = NULL;
    }
    else
    {
      //                      printf("Error: parser return object is null \n");
      // free all memmory allocated for parser_return_arr
      int k = 0;
      while (i > 0 && parser_return_arr[k] != NULL)
      {
        free(parser_return_arr[k]);
        k++;
      }
      free(parser_return_arr);
      return NULL; /* TESTING dont forget to remove return value */
    }
  }

  // END OF MEMORY FREEING
  return parser_return_arr;
}

// RUNS AFTER EVERY PROMPT entered and processed
// takes a sentences_conditions_pair and
// frees all memory allocated for inner references
// WARNING: Don't try to free action of any struct as they are not dynamically allocated
// WARNING: assign NULL to every pointer after freeing
// in order to avoid double freeing
void free_memory(struct sentences_conditions_pair **pairs, char ***res, char ****statement_parts, char ****final_sentences)
{
  int i = 0;
  // First free statement_parts
  while (*statement_parts != NULL && (*statement_parts)[i] != NULL)
  {
    free((*statement_parts)[i]);
    (*statement_parts)[i] = NULL;
    i++;
  }
  free(*statement_parts);
  *statement_parts = NULL;

  // Then free final_sentences
  i = 0;
  while (*final_sentences != NULL && (*final_sentences)[i] != NULL)
  {
    free((*final_sentences)[i]);
    (*final_sentences)[i] = NULL;
    i++;
  }
  free(*final_sentences);
  *final_sentences = NULL;

  // Free res
  i = 0;
  while (*res != NULL && (*res)[i] != NULL)
  {
    free((*res)[i]);
    (*res)[i] = NULL;
    i++;
  }
  free(*res);
  *res = NULL;

  // Free pairs

  i = 0;
  while (pairs != NULL && pairs[i] != NULL)
  {
    struct sentences_conditions_pair *pair = pairs[i];
    struct sentence **sentences = pair->sentences;
    int j = 0;
    while (sentences != NULL && sentences[j] != NULL)
    {
      struct sentence *sentence = sentences[j];
      char **subjects = sentence->subjects;
      int k = 0;
      while (subjects != NULL && subjects[k] != NULL)
      {
        // free(subjects[k]);
        // subjects[k] = NULL;
        k++;
      }
      free(subjects);
      subjects = NULL;
      char **objects = sentence->objects;
      k = 0;
      if (sentence->can_free_objects == 1)
      {
        while (objects != NULL && objects[k] != NULL)
        {
          free(objects[k]);
          objects[k] = NULL;
          k++;
        }
      }
      free(objects);
      objects = NULL;
      if (sentence->to_or_from != NULL)
      {
        //                         printf("here\n");
        //                           printf("sentence->to_or_from: %s\n", sentence->to_or_from);
        //                           printf("address of sentence->to_or_from: %p\n", sentence->to_or_from);
        // free(sentence->to_or_from);
        sentence->to_or_from = NULL;
      }
      free(sentence);
      sentence = NULL;
      j++;
    }
    free(sentences);
    sentences = NULL;
    struct condition **conditions = pair->conditions;
    j = 0;
    while (conditions != NULL && conditions[j] != NULL)
    {
      struct condition *condition = conditions[j];
      char **subjects = condition->subjects;
      int k = 0;
      while (subjects != NULL && subjects[k] != NULL)
      {
        // free(subjects[k]);
        subjects[k] = NULL;
        k++;
      }
      free(subjects);
      subjects = NULL;
      char **objects = condition->objects;
      k = 0;
      if (condition->can_free_objects == 1)
      {
        while (objects != NULL && objects[k] != NULL)
        {
          free(objects[k]);
          objects[k] = NULL;
          k++;
        }
      }
      free(objects);
      objects = NULL;
      free(condition);
      condition = NULL;
      j++;
    }
    free(conditions);
    conditions = NULL;
    free(pair);
    pair = NULL;
    i++;
  }
  free(pairs);
  pairs = NULL;
}

/*
int main()
{
  // take intput from user by scanf
  char input_text[1024];
  scanf("%[^\n]", input_text);
  // call the parser function
  struct sentences_conditions_pair **pairs = parser(input_text);
  if (pairs == NULL)
  {
//        printf("Invalid syntax!\n");
  }
  free_memory(pairs);
  return 0;
} */
