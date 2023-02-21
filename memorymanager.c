#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define SIZE 64
int process_id;
int virtual_address;
int value;
char input[20];

unsigned char memory[SIZE];

int main(int argc, char **argv)
{
    while (1)
    {
        printf("Instruction? ");
        fgets(input, sizeof(input), stdin);

        char *token = strtok(input, ",");
        process_id = atoi(token);
        token = strtok(NULL, ",");
        char *instruction_type = token;
        token = strtok(NULL, ",");
        virtual_address = atoi(token);
        token = strtok(NULL, ",");
        value = atoi(token);
        
        if (!strcmp(instruction_type, "map"))
        {
            map();
        }
        else if (!strcmp(instruction_type, "store"))
        {
            store();
        }
        else if (!strcmp(instruction_type, "load"))
        {
            load();
        }
    }
}

// load instructs the memory manager to return the byte stored at the memory location specified by
// virtual address. Like the store instruction, it is the memory manager’s responsibility to translate
// and swap pages as needed. Note, the value parameter is not used for this instruction, but a dummy
// value (e.g., 0) should always be provided.

void map()
{
    printf("map");
}

void store()
{
    printf("store");
}

void load()
{
    printf("load");
}