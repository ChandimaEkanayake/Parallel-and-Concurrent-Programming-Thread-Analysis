#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>  // For sqrt() and pow()

#define INITIAL_RUNS 100  // Initial number of times to run performRandomOperations

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
int determineSampleSize(double mean, double stdDev, double r, double confidenceLevel);

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

int determineSampleSize(double mean, double stdDev, double r, double confidenceLevel) {
    double z = confidenceLevel == 0.95 ? 1.96 : 1.645;  // 95% confidence: z=1.96, 90% confidence: z=1.645
    return (int)pow((z * stdDev) / (r * mean), 2);
}

void runExperiment(double mMember, double mInsert, double mDelete, int n, int m) {
    LinkedList list = {NULL};
    populateLinkedList(&list, n);

    // First, run 100 times to get initial mean and std dev
    double times[INITIAL_RUNS];
    struct timeval start, end;

    for (int i = 0; i < INITIAL_RUNS; i++) {
        gettimeofday(&start, NULL);
        performRandomOperations(&list, m, mMember, mInsert, mDelete);
        gettimeofday(&end, NULL);

        double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        times[i] = elapsed;
    }

    // Calculate initial mean and standard deviation
    double initialMean = calculateAverage(times, INITIAL_RUNS);
    double initialStdDev = calculateStandardDeviation(times, INITIAL_RUNS, initialMean);

    // Output initial results
    printf("Initial Mean: %.6f seconds\n", initialMean);
    printf("Initial Std Dev: %.6f seconds\n", initialStdDev);

    // Determine the number of samples needed based on the relative error and confidence level
    double relativeError = 0.05;  // 5% relative error
    double confidenceLevel = 0.95;  // 95% confidence level
    int sampleSize = determineSampleSize(initialMean, initialStdDev, relativeError, confidenceLevel);

    printf("Calculated Sample Size: %d\n", sampleSize);

    // Now run the function using the calculated number of turns
    double* newTimes = (double*)malloc(sampleSize * sizeof(double));

    for (int i = 0; i < sampleSize; i++) {
        gettimeofday(&start, NULL);
        performRandomOperations(&list, m, mMember, mInsert, mDelete);
        gettimeofday(&end, NULL);

        double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
        newTimes[i] = elapsed;
    }

    // Calculate the final mean and standard deviation
    double finalMean = calculateAverage(newTimes, sampleSize);
    double finalStdDev = calculateStandardDeviation(newTimes, sampleSize, finalMean);

    // Output final results
    printf("Final Mean: %.6f seconds\n", finalMean);
    printf("Final Std Dev: %.6f seconds\n", finalStdDev);

    free(newTimes);  // Free dynamically allocated memory
}

int main() {
    srand(time(NULL));  // Set a different random seed for each execution

    int n = 1000;  // Number of unique values to populate
    int m = 10000; // Number of operations to perform

    // Run the experiment 3 times with different operation fractions

    printf("=== Experiment 1: mMember = 0.99, mInsert = 0.005, mDelete = 0.005 ===\n");
    runExperiment(0.99, 0.005, 0.005, n, m);

    printf("\n=== Experiment 2: mMember = 0.90, mInsert = 0.05, mDelete = 0.05 ===\n");
    runExperiment(0.90, 0.05, 0.05, n, m);

    printf("\n=== Experiment 3: mMember = 0.50, mInsert = 0.25, mDelete = 0.25 ===\n");
    runExperiment(0.50, 0.25, 0.25, n, m);

    return 0;
}
