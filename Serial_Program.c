#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct Node
{
    int value;
    struct Node *next;
} Node;

typedef struct LinkedList
{
    Node *head;
} LinkedList;

// Function prototypes
void Insert(LinkedList *list, int value);
bool Member(LinkedList *list, int value);
void Delete(LinkedList *list, int value);
void populateLinkedList(LinkedList *list, int n);
void performRandomOperations(LinkedList *list, int m, double mMember, double mInsert, double mDelete);

void Insert(LinkedList *list, int value)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->value = value;
    newNode->next = list->head;
    list->head = newNode;
}

bool Member(LinkedList *list, int value)
{
    Node *current = list->head;
    while (current != NULL)
    {
        if (current->value == value)
        {
            return true; // Value found
        }
        current = current->next;
    }
    return false; // Value not found
}

void Delete(LinkedList *list, int value)
{
    Node **current = &(list->head);
    while (*current != NULL)
    {
        if ((*current)->value == value)
        {
            Node *toDelete = *current;
            *current = (*current)->next;
            free(toDelete);
            return;
        }
        current = &((*current)->next);
    }
}

void populateLinkedList(LinkedList *list, int n)
{
    int *values = (int *)malloc(n * sizeof(int));
    int count = 0;

    // Fill the array with unique random values
    while (count < n)
    {
        int value = rand() % (1 << 16); // Random value between 0 and 65535
        bool isUnique = true;

        // Check if the value is unique
        for (int i = 0; i < count; i++)
        {
            if (values[i] == value)
            {
                isUnique = false;
                break;
            }
        }

        // If unique, add to the array and linked list
        if (isUnique)
        {
            values[count] = value;
            Insert(list, value);
            count++;
        }
    }
    free(values);

    // Print the Linked List
    for (Node *current = list->head; current != NULL; current = current->next)
    {
        printf("%p:%d:%p ", current, current->value, current->next);
    }
}

void performRandomOperations(LinkedList *list, int m, double mMember, double mInsert, double mDelete)
{
    for (int i = 0; i < m; i++)
    {
        int operationType = rand() % 100; // Random number between 0 and 99
        int value = rand() % (1 << 16);   // Random value between 0 and 65535

        if (operationType < mMember * 100)
        {
            // Member operation
            if (Member(list, value))
            {
                printf("Value %d is a member of the list.\n", value);
            } else {
                printf("Value %d is not a member of the list.\n", value);
            }
        }
        else if (operationType < (mMember + mInsert) * 100)
        {
            // Insert operation (check uniqueness before inserting)
            if (!Member(list, value))
            {
                Insert(list, value);
                printf("Inserted value: %d\n", value);
            }
        }
        else
        {
            // Delete operation
            if (Member(list, value))
            {
                Delete(list, value);
                printf("Deleted value: %d\n", value);
            }
        }
    }
}

// To free the Memory
void freeLinkedList(LinkedList *list)
{
    Node *current = list->head;
    Node *nextNode;
    while (current != NULL)
    {
        nextNode = current->next;
        free(current);
        current = nextNode;
    }
    list->head = NULL; // Set the head to NULL after freeing
}

int main()
{
    srand(time(NULL)); // Set a different random seed for each execution

    LinkedList list = {NULL};
    int n = 1000;                                            // Number of unique values to populate
    int m = 10000;                                           // Total operations to perform
    double mMember = 0.99, mInsert = 0.005, mDelete = 0.005; // Operation fractions

    populateLinkedList(&list, n);
    performRandomOperations(&list, m, mMember, mInsert, mDelete);

    // (Free all nodes in the linked list)
    freeLinkedList(&list);
    return 0;
}
