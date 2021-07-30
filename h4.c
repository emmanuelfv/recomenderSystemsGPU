#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h> 

#define CAPACITY 50000 // Size of the Hash Table
 
unsigned long hash_function_2int(int x1, int x2) {
    return (x1 + x2) % CAPACITY;
}
 
typedef struct Ht_item_2int_float Ht_item_2int_float;
 
// Define the Hash Table Item here
struct Ht_item_2int_float {
    int key1, key2;
    float value;
};
 
 
typedef struct LinkedList_2int_float LinkedList_2int_float;
 
// Define the LinkedList_2int_float here
struct LinkedList_2int_float {
    Ht_item_2int_float* item; 
    LinkedList_2int_float* next;
};
 
 
typedef struct HashTable_2int_float HashTable_2int_float;
 
// Define the Hash Table here
struct HashTable_2int_float {
    // Contains an array of pointers
    // to items
    Ht_item_2int_float** items;
    LinkedList_2int_float** overflow_buckets;
    int size;
    int count;
};
 
 
static LinkedList_2int_float* allocate_list_2int_float () {
    // Allocates memory for a LinkedList_2int_float pointer
    LinkedList_2int_float* list = (LinkedList_2int_float*) malloc (sizeof(LinkedList_2int_float));
    return list;
}
 
static LinkedList_2int_float* LinkedList_2int_float_insert(LinkedList_2int_float* list, Ht_item_2int_float* item) {
    // Inserts the item onto the Linked List
    if (!list) {
        LinkedList_2int_float* head = allocate_list_2int_float();
        head->item = item;
        head->next = NULL;
        list = head;
        return list;
    } 
     
    else if (list->next == NULL) {
        LinkedList_2int_float* node = allocate_list_2int_float();
        node->item = item;
        node->next = NULL;
        list->next = node;
        return list;
    }
 
    LinkedList_2int_float* temp = list;
    while (temp->next->next) {
        temp = temp->next;
    }
     
    LinkedList_2int_float* node = allocate_list_2int_float();
    node->item = item;
    node->next = NULL;
    temp->next = node;
     
    return list;
}
 
static Ht_item_2int_float* LinkedList_2int_float_remove(LinkedList_2int_float* list) {
    // Removes the head from the linked list
    // and returns the item of the popped element
    if (!list)
        return NULL;
    if (!list->next)
        return NULL;
    LinkedList_2int_float* node = list->next;
    LinkedList_2int_float* temp = list;
    temp->next = NULL;
    list = node;
    Ht_item_2int_float* it = NULL;
    memcpy(temp->item, it, sizeof(Ht_item_2int_float));
    // free(temp->item->key);
    // free(temp->item->value);
    free(temp->item);
    free(temp);
    return it;
}
 
static void free_LinkedList_2int_float(LinkedList_2int_float* list) {
    LinkedList_2int_float* temp = list;
    while (list) {
        temp = list;
        list = list->next;
        // free(temp->item->key);
        // free(temp->item->value);
        free(temp->item);
        free(temp);
    }
}
 
static LinkedList_2int_float** create_overflow_buckets_2int_float(HashTable_2int_float* table) {
    // Create the overflow buckets; an array of LinkedList_2int_floats
    LinkedList_2int_float** buckets = (LinkedList_2int_float**) calloc (table->size, sizeof(LinkedList_2int_float*));
    for (int i=0; i<table->size; i++)
        buckets[i] = NULL;
    return buckets;
}
 
static void free_overflow_buckets_2int_float(HashTable_2int_float* table) {
    // Free all the overflow bucket lists
    LinkedList_2int_float** buckets = table->overflow_buckets;
    for (int i=0; i<table->size; i++)
        free_LinkedList_2int_float(buckets[i]);
    free(buckets);
}
 
 
Ht_item_2int_float* create_item_2int_float(int key1, int key2, float value) {
    // Creates a pointer to a new hash table item
    Ht_item_2int_float* item = (Ht_item_2int_float*) malloc (sizeof(Ht_item_2int_float));
    
    item->key1 = key1;
    item->key2 = key2;
    item->value = value;
    // item->key = (char*) malloc (strlen(key) + 1);
    // item->value = (char*) malloc (strlen(value) + 1);
     
    // strcpy(item->key, key);
    // strcpy(item->value, value);
 
    return item;
}
 
HashTable_2int_float* create_table_2int_float(int size) {
    // Creates a new HashTable_2int_float
    HashTable_2int_float* table = (HashTable_2int_float*) malloc (sizeof(HashTable_2int_float));
    table->size = size;
    table->count = 0;
    table->items = (Ht_item_2int_float**) calloc (table->size, sizeof(Ht_item_2int_float*));
    for (int i=0; i<table->size; i++)
        table->items[i] = NULL;
    table->overflow_buckets = create_overflow_buckets_2int_float(table);
 
    return table;
}
 
void free_item_2int_float(Ht_item_2int_float* item) {
    // Frees an item
    // free(item->key);
    // free(item->value);
    free(item);
}
 
void free_table_2int_float(HashTable_2int_float* table) {
    // Frees the table
    for (int i=0; i<table->size; i++) {
        Ht_item_2int_float* item = table->items[i];
        if (item != NULL)
            free_item_2int_float(item);
    }
 
    free_overflow_buckets_2int_float(table);
    free(table->items);
    free(table);
}
 
void handle_collision_2int_float(HashTable_2int_float* table, unsigned long index, Ht_item_2int_float* item) {
    LinkedList_2int_float* head = table->overflow_buckets[index];
 
    if (head == NULL) {
        // We need to create the list
        head = allocate_list_2int_float();
        head->item = item;
        table->overflow_buckets[index] = head;
        return;
    }
    else {
        // Insert to the list
        table->overflow_buckets[index] = LinkedList_2int_float_insert(head, item);
        return;
    }
 }
 
void ht_insert_2int_float(HashTable_2int_float* table, int key1, int key2, float value) {
    // Create the item
    Ht_item_2int_float* item = create_item_2int_float(key1, key2, value);
 
    // Compute the index
    unsigned long index = hash_function_2int(key1, key2);
 
    Ht_item_2int_float* current_item = table->items[index];
     
    if (current_item == NULL) {
        // Key does not exist.
        if (table->count == table->size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            // Remove the create item
            free_item_2int_float(item);
            return;
        }
         
        // Insert directly
        table->items[index] = item; 
        table->count++;
    }
 
    else {
        // Scenario 1: We only need to update value
        if (current_item->key1 == key1 && current_item->key2 == key2) {
            table->items[index]->value = value;
            return;
        }
    
        else {
            // Scenario 2: Collision
            handle_collision_2int_float(table, index, item);
            return;
        }
    }
}
 
float ht_search_2int_float(HashTable_2int_float* table, int key1, int key2) {
    // Searches the key in the HashTable_2int_float
    // and returns NULL if it doesn't exist
    int index = hash_function_2int(key1, key2);
    Ht_item_2int_float* item = table->items[index];
    LinkedList_2int_float* head = table->overflow_buckets[index];
 
    // Ensure that we move to items which are not NULL
    while (item != NULL) {
        if (item->key1 == key1 && item->key2 == key2)
            return item->value;
        if (head == NULL)
            return FLT_MIN;
        item = head->item;
        head = head->next;
    }
    return FLT_MIN;
}
 
void ht_delete_2int_float(HashTable_2int_float* table, int key1, int key2) {
    // Deletes an item from the table
    int index = hash_function_2int(key1, key2);
    Ht_item_2int_float* item = table->items[index];
    LinkedList_2int_float* head = table->overflow_buckets[index];
 
    if (item == NULL) {
        // Does not exist. Return
        return;
    }
    else {
        if (head == NULL && item->key1 == key1 && item->key2 == key2) {
            // No collision chain. Remove the item
            // and set table index to NULL
            table->items[index] = NULL;
            free_item_2int_float(item);
            table->count--;
            return;
        }
        else if (head != NULL) {
            // Collision Chain exists
            if (item->key1 == key1 && item->key2 == key2) {
                // Remove this item and set the head of the list
                // as the new item
                 
                free_item_2int_float(item);
                LinkedList_2int_float* node = head;
                head = head->next;
                node->next = NULL;
                table->items[index] = create_item_2int_float(node->item->key1, node->item->key2, node->item->value);
                free_LinkedList_2int_float(node);
                table->overflow_buckets[index] = head;
                return;
            }
 
            LinkedList_2int_float* curr = head;
            LinkedList_2int_float* prev = NULL;
             
            while (curr) {
                if (curr->item->key1 == key1 && curr->item->key2 == key2) {
                    if (prev == NULL) {
                        // First element of the chain. Remove the chain
                        free_LinkedList_2int_float(head);
                        table->overflow_buckets[index] = NULL;
                        return;
                    }
                    else {
                        // This is somewhere in the chain
                        prev->next = curr->next;
                        curr->next = NULL;
                        free_LinkedList_2int_float(curr);
                        table->overflow_buckets[index] = head;
                        return;
                    }
                }
                curr = curr->next;
                prev = curr;
            }
 
        }
    }
}
 
void print_search_2int_float(HashTable_2int_float* table, int key1, int key2) {
    float val;
    if ((val = ht_search_2int_float(table, key1, key2)) == FLT_MIN) {
        printf("(%d,%d) does not exist\n", key1, key2);
        return;
    }
    else {
        printf("Key:(%d,%d), Value:%f\n", key1, key2, val);
    }
}
 
void print_table_2int_float(HashTable_2int_float* table) {
    printf("\n-------------------\n");
    for (int i=0; i<table->size; i++) {
        if (table->items[i]) {
            printf("Index:%d, Key:(%d,%d), Value:%f", i, table->items[i]->key1, table->items[i]->key2, table->items[i]->value);
            if (table->overflow_buckets[i]) {
                printf(" => Overflow Bucket => ");
                LinkedList_2int_float* head = table->overflow_buckets[i];
                while (head) {
                    printf("Key:(%d,%d), Value:%f ", head->item->key1, head->item->key2, head->item->value);
                    head = head->next;
                }
            }
            printf("\n");
        }
    }
    printf("-------------------\n");
}
 
int main() {
    HashTable_2int_float* ht = create_table_2int_float(CAPACITY);
    //buscar siempre a > b para media matriz de datos.
    ht_insert_2int_float(ht, 1, 2, 2.3); 
    ht_insert_2int_float(ht, 2, 3, .53);
    ht_insert_2int_float(ht, 3, 1, 0.3);
    ht_insert_2int_float(ht, 50001, 50002, 9.99);
    print_search_2int_float(ht, 1, 2);
    print_search_2int_float(ht, 2, 3);
    print_search_2int_float(ht, 3, 1);
    print_search_2int_float(ht, 2, 1);
    print_search_2int_float(ht, 50002, 50001);  // Collision!
    print_table_2int_float(ht);
    ht_delete_2int_float(ht, 1, 2);
    ht_delete_2int_float(ht, 50001, 50002);
    print_table_2int_float(ht);
    free_table_2int_float(ht);
    return 0;
}