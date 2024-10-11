#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>  // For sqrt()

#define NUM_RUNS 100  // Number of times to run performRandomOperations

typedef struct Node {
    int value;
    struct Node* next;
} Node;

typedef struct LinkedList {
    Node* head;
} LinkedList;

void Insert(LinkedList* list, int value);
bool Member(LinkedList* list, int value);
void Delete(LinkedList* list, int value);
void populateLinkedList(LinkedList* list, int n);
void performRandomOperations(LinkedList* list, int m, double mMember, double mInsert, double mDelete);
double calculateAverage(double times[], int numRuns);
double calculateStandardDeviation(double times[], int numRuns, double average);

void Insert(LinkedList* list, int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->value = value;
    newNode->next = list->head;
    list->head = newNode;
}

bool Member(LinkedList* list, int value) {
    Node* current = list->head;
    while (current != NULL) {
        if (current->value == value) {
            return true;  // Value found
        }
        current = current->next;
    }
    return false;  // Value not found
}

void Delete(LinkedList* list, int value) {
    Node** current = &(list->head);
    while (*current != NULL) {
        if ((*current)->value == value) {
            Node* toDelete = *current;
            *current = (*current)->next;
            free(toDelete);
            return;
        }
        current = &((*current)->next);
    }
}

void populateLinkedList(LinkedList* list, int n) {
    int* values = (int*)malloc(n * sizeof(int));
    int count = 0;

    while (count < n) {
        int value = rand() % (1 << 16);  // Random value between 0 and 65535
        bool isUnique = true;
        
        for (int i = 0; i < count; i++) {
            if (values[i] == value) {
                isUnique = false;
                break;
            }
        }
        
        if (isUnique) {
            values[count] = value;
            Insert(list, value);
            count++;
        }
    }
    free(values);
}

void performRandomOperations(LinkedList* list, int m, double mMember, double mInsert, double mDelete) {
    for (int i = 0; i < m; i++) {
        int operationType = rand() % 100;
        int value = rand() % (1 << 16);

        if (operationType < mMember * 100) {
            Member(list, value);
        } else if (operationType < (mMember + mInsert) * 100) {
            if (!Member(list, value)) {
                Insert(list, value);
            }
        } else {
            if (Member(list, value)) {
                Delete(list, value);
            }
        }
    }
}

double calculateAverage(double times[], int numRuns) {
    double sum = 0.0;
    for (int i = 0; i < numRuns; i++) {
        sum += times[i];
    }
    return sum / numRuns;
}

double calculateStandardDeviation(double times[], int numRuns, double average) {
    double sum = 0.0;
    for (int i = 0; i < numRuns; i++) {
        sum += pow(times[i] - average, 2);
    }
    return sqrt(sum / numRuns);
}

int main() {
    srand(time(NULL));  // Set a different random seed for each execution

    LinkedList list = {NULL};
    int n = 1000;
    int m = 10000;
    double mMember = 0.99, mInsert = 0.005, mDelete = 0.005;

    populateLinkedList(&list, n);

    double times[NUM_RUNS];
    struct timeval start, end;

    for (int i = 0; i < NUM_RUNS; i++) {
        gettimeofday(&start, NULL);
        performRandomOperations(&list, m, mMember, mInsert, mDelete);
        gettimeofday(&end, NULL);

        double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        times[i] = elapsed;
    }

    // Calculate average time
    double average = calculateAverage(times, NUM_RUNS);
    printf("Average time: %.6f seconds\n", average);

    // Calculate standard deviation
    double stddev = calculateStandardDeviation(times, NUM_RUNS, average);
    printf("Standard deviation: %.6f seconds\n", stddev);

    // 95% Confidence Interval
    double margin_of_error = 1.96 * (stddev / sqrt(NUM_RUNS));
    printf("Margin of error: %.6f seconds\n", margin_of_error);

    double lower_bound = average - margin_of_error;
    double upper_bound = average + margin_of_error;
    printf("95%% Confidence Interval: [%.6f, %.6f]\n", lower_bound, upper_bound);

    return 0;
}
