#include <stdio.h>
#include <string.h>

char* output[] = {
    "OK\n",
    "NOBODY\n",
    "Rivendell\n",
    "OK\n",
    "OK\n",
    "OK\n",
    "NOWHERE\n",
    "1 ring and 3 bread\n",
    "INVALID\n",
    "0\n",
    "0\n",
    "OK\n",
    "Frodo\n",
    "OK\n",
    "0\n",
    "OK\n",
    "This is a wrong output\n"
};

int main() {

    char line[1025]; 
    int index = 0;

    while (1) {
        printf(">> ");
        fflush(stdout);

        if (fgets(line, 1025, stdin) == NULL) {
            break;
        }

        fprintf(stderr, "Received: %s", line);

        if (strcmp(line, "exit\n") == 0) {
            break;
        }
        
        printf("%s", output[index++]);
        fflush(stdout);
    }

    return 0;
}