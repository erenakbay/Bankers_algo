#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_CUSTOMERS 5 // n
#define NUMBER_OF_RESOURCES 4 // m
#define MAXIMUM_INPUT_FILE "maximum.txt" // <-

#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define RESET "\e[0m"
// Sample maximum.txt content
// 6 4 7 3
// 4 2 3 2
// 2 5 3 3
// 6 3 3 2
// 5 6 7 5

/* The available amount of each resource */ 
int available[NUMBER_OF_RESOURCES]; //
/* The maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* The amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* The remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

void printfcol(char *a, char *color) {
    printf("%s%s%s\n",color,a, RESET);
}

int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);
int is_safe();
// Parse the resource request
void parse_request(char input[]) {
    int i = 0;
    int customer_num = 0;
    int request[NUMBER_OF_RESOURCES] = {0};

    while (input[i] == 32)
        i++;
    while(input[i] != 32 && input[i])
        i++;
    while (input[i] == 32)
        i++;
    customer_num = atoi(&input[i]);
    while (input[i] >= '0' && input[i] <= '9')
        i++;

    int j = 0;
    while(input[i]) {
        while (input[i] == 32)
            i++;
        request[j++] = atoi(&input[i]);
        while (input[i] >= '0' && input[i] <= '9')
            i++;
    }
    printf("Requesting resources for Customer %d...\n", customer_num);
    request_resources(customer_num, request);
}
// Parse the resource release
void parse_release(char input[]) {
    int i = 0;
    int customer_num = 0;
    int release[NUMBER_OF_RESOURCES] = {0};

    while (input[i] == 32)
        i++;
    while(input[i] != 32 && input[i])
        i++;
    while (input[i] == 32)
        i++;
    customer_num = atoi(&input[i]);
    while (input[i] >= '0' && input[i] <= '9')
        i++;

    int j = 0;
    while(input[i]) {
        while (input[i] == 32)
            i++;
        release[j++] = atoi(&input[i]);
        while (input[i] >= '0' && input[i] <= '9')
            i++;
    }
    printf("Releasing resources for Customer %d...\n", customer_num);
    release_resources(customer_num, release);
}
// Safety algorithm
int is_safe() {
    int Work[NUMBER_OF_RESOURCES];
    int Finish[NUMBER_OF_CUSTOMERS] = {0};
    int index = 0;
    while (index < NUMBER_OF_RESOURCES) {
        Work[index] = available[index];
        index++;
    }
    // check if a customer can finish
    while (1) {
        int foundFlag = 0;
        int index = 0;
        while (index < NUMBER_OF_CUSTOMERS) {
            if (!Finish[index]) {
                int ableToFinish = 1;
                int indexTwo = 0;
                while (indexTwo < NUMBER_OF_RESOURCES) {
                    if (need[index][indexTwo] > Work[indexTwo]) {
                        ableToFinish = 0;
                        break;
                    }
                    indexTwo++;
                } 
                if (ableToFinish) {
                    indexTwo = 0;
                    while (indexTwo < NUMBER_OF_RESOURCES) {
                        Work[indexTwo] = Work[indexTwo] + allocation[index][indexTwo];
                        indexTwo++;
                    }
                    Finish[index] = 1;
                    foundFlag = 1;
                }
            }
            index++;
        }
        if (!foundFlag) {
            break; 
        }
    }

    index = 0;
    while (index < NUMBER_OF_CUSTOMERS) {
        if (!Finish[index]) 
            return 0;
        index++;
    }
    return 1;
}

int request_resources(int customer_num, int request[]) {

    int index = 0;
    // Check if the request is valid
    while (index < NUMBER_OF_RESOURCES) {

        if (request[index] > need[customer_num][index]) {
            printfcol("Error: Request exceeds the declared maximum needs.",RED);
            return -1;
        }
        if (request[index] > available[index]) {
            printfcol("Error: Request exceeds the available resources.", RED);
            return -1;
        }
        index++;
    }
    // Temporarily allocate resources
    index = 0;
    while (index < NUMBER_OF_RESOURCES) {
        available[index] = available[index] - request[index];
        allocation[customer_num][index] = allocation[customer_num][index] + request[index];
        need[customer_num][index] = need[customer_num][index] - request[index];

        index++;
    }
    
    if (!is_safe()) {
        index = 0;
        while (index < NUMBER_OF_RESOURCES) {
            available[index] = available[index] + request[index];
            allocation[customer_num][index] = allocation[customer_num][index] - request[index];
            need[customer_num][index] = need[customer_num][index] + request[index];

            index++;
        } // Rollback allocation
        printfcol("Error: Request denied. System would be left in an unsafe state.", RED);
        return -1;
    }

    printfcol("Request granted. System is in a safe state.", GRN);
    return 0;
}
// Release resources
void release_resources(int customer_num, int release[]) {

    int index = 0;
    while( index < NUMBER_OF_RESOURCES) {
        if (release[index] > allocation[customer_num][index]) {
            printfcol("Error: Cannot release more resources than allocated.", RED);
            return;
        } 
        allocation[customer_num][index] = allocation[customer_num][index] - release[index];
        available[index] = available[index] + release[index];
        need[customer_num][index] = need[customer_num][index] + release[index];
        index++;
    }

    printfcol("Resources released successfully.", GRN);
}

int fill_values(int argc, char **argv) {

    FILE *file = fopen(MAXIMUM_INPUT_FILE, "r");

    if (file == NULL) {
        printfcol("Error: cannot open file.\n", RED);
        return 1;
    }
    // Fill maximum
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            if (fscanf(file, " %d", &maximum[i][j]) != 1) {
                printf("Error reading data from file.\n");
                fclose(file);
                return 1;
            }
            need[i][j] = maximum[i][j];
        }
    }
    fclose(file);

    for (int i = 1; i < argc; i++) {
        available[i-1] = atoi(argv[i]);
    }
    return 0;
}
// Initiate the system
void initiation() {

    printfcol("Welcome to the Banker's Algorithm Simulation",BLU);
    printf("Initializing system with resources:\nAvailable: ");

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (i == 0) {
            printf("\033[0;35m[");
        }
        else 
            printf(", ");
        printf("%d", available[i]);
        if (i == NUMBER_OF_RESOURCES - 1) {
            printf("]%s\n",RESET);
        }
    }

    printf("Reading maximum request file...\n");
    printf("Maximum requests initialized:\n");

    for (int i = 0; i<NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d: ", i);
        for (int j = 0; j< NUMBER_OF_RESOURCES; j++) {
            if (j == 0) {
                printf("\033[0;35m[");

            }
            else 
                printf(", ");
            printf("%d", maximum[i][j]);
            if (j == NUMBER_OF_RESOURCES - 1) {
                printf("]%s\n",RESET);
            }
        }
    }
    // Allocate the resources
    printf("Please enter commands:\n");
    printf("- %s'RQ customer_num resource_1 resource_2 resource_3 resource_4'%s to request resources\n", YEL, RESET);
    printf("- %s'RL customer_num resource_1 resource_2 resource_3 resource_4'%s to release resources\n", YEL, RESET);
    printf("- %s'*'%s to display the current values of the data structures\n", YEL, RESET);
    printf("- %s'exit'%s to quit\n", RED, RESET);
}

int makeLower(int a) {
    if (a <= 'Z' && a >= 'A')
        return a + 32;
    return a;
}
// Check the input
int check_input(char input[]) {
    char cmp[4] = {0};
    int i = 0;
    int j = 0;
    // check front spaces
    while (input[i] == 32) {
        i++;
    }
    while (input[i+j] != 32 && input[i+j] && j < 4) {
        cmp[j] = input[i+j];
        j++;
    }

    cmp[j] = '\0';
    // make cmp smaller

    for (i = 0; i < j; i++) {
        cmp[i] = makeLower(cmp[i]);
    }
    // printf("%s\n",cmp);
    if (strcmp(cmp, "rq") == 0)
        return 1;
    if (strcmp(cmp, "rl") == 0)
        return 2;
    if (strcmp(cmp, "*") == 0)
        return 3;
    if (strcmp(cmp, "exit") == 0)
        return 4;
    return -1;
}
// Command options
void default_option() {
    printf("\033[0;31mPlease enter commands:\033[0;37m\n");
    printf("- \033[0;33m'RQ customer_num resource_1 resource_2 resource_3 resource_4'\033[0;37m to request resources\n");
    printf("- \033[0;33m'RL customer_num resource_1 resource_2 resource_3 resource_4'\033[0;37m to release resources\n");
    printf("- \033[0;33m'*'\033[0;37m to display the current values of the data structures\n");
    printf("- \033[0;31m'exit'\033[0;37m to quit\n");
}

void star_option() {
    printf("Current system state:\n");
    printf("Available:\n");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (i == 0) {
                printf("\033[0;35m[");

            }
            else 
                printf(", ");
            printf("%d", available[i]);
            if (i == NUMBER_OF_RESOURCES - 1) {
                printf("]\033[0;37m\n");
            }
    }
    printf("Maximum:\n");
    for (int i = 0; i<NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d: ", i);
        for (int j = 0; j< NUMBER_OF_RESOURCES; j++) {
            if (j == 0) {
                printf("\033[0;35m[");

            }
            else 
                printf(", ");
            printf("%d", maximum[i][j]);
            if (j == NUMBER_OF_RESOURCES - 1) {
                printf("]\033[0;37m\n");
            }
        }
    }
    printf("Allocation:\n");
    for (int i = 0; i<NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d: ", i);
        for (int j = 0; j< NUMBER_OF_RESOURCES; j++) {
            if (j == 0) {
                printf("\033[0;35m[");

            }
            else 
                printf(", ");
            printf("%d", allocation[i][j]);
            if (j == NUMBER_OF_RESOURCES - 1) {
                printf("]\033[0;37m\n");
            }
        }
    }

    printf("Need:\n");
    for (int i = 0; i<NUMBER_OF_CUSTOMERS; i++) {
        printf("Customer %d: ", i);
        for (int j = 0; j< NUMBER_OF_RESOURCES; j++) {
            if (j == 0) {
                printf("\033[0;35m[");

            }
            else 
                printf(", ");
            printf("%d", need[i][j]);
            if (j == NUMBER_OF_RESOURCES - 1) {
                printf("]\033[0;37m\n");
            }
        }
    }

}

// Main function
int main(int argc, char **argv) {

    if (argc != NUMBER_OF_RESOURCES + 1) {
        printf("Not enough resources needs to be %d\n", NUMBER_OF_RESOURCES);
        return 1;
    }

    if (fill_values(argc, argv))
        return 1;
        
    printfcol("\n---------------------------------",YEL);
    initiation();
    // loop to take input
    while (1) {
        char input[100] = {0};
        printfcol("\n---------------------------------",YEL);
        printf("Command: ");
        scanf("%[^\n]", input); 
        switch (check_input(input)) {
            case 1: // RQ
                parse_request(input);
                break;
            case 2: // RL
                parse_release(input);
                break;
            case 3: // *
                star_option();
                break;
            case 4: // exit
                printfcol("Exiting program. Goodbye!",RED);
                return 1;
                break;
            default:
                default_option();
        }
        fflush(0);
        puts("\n");
    }
    return 1;
}

