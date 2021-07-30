#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h> 

#include "h3.c"

// #define CAPACITY 50000 // Size of the Hash Table
 
// unsigned long hash_function_int(int x) {
//     return x % CAPACITY;
// }
 
typedef struct Ht_item_int2hash Ht_item_int2hash;
 
// Define the Hash Table Item here
struct Ht_item_int2hash {
    int key;
    HashTable_int2float* value;
};
 
 
typedef struct LinkedList_int2hash LinkedList_int2hash;
 
// Define the LinkedList_int2hash here
struct LinkedList_int2hash {
    Ht_item_int2hash* item; 
    LinkedList_int2hash* next;
};
 
 
typedef struct HashTable_int2hash HashTable_int2hash;
 
// Define the Hash Table here
struct HashTable_int2hash {
    // Contains an array of pointers
    // to items
    Ht_item_int2hash** items;
    LinkedList_int2hash** overflow_buckets;
    int size;
    int count;
};
 
 
static LinkedList_int2hash* allocate_list_int2hash () {
    // Allocates memory for a LinkedList_int2hash pointer
    LinkedList_int2hash* list = (LinkedList_int2hash*) malloc (sizeof(LinkedList_int2hash));
    return list;
}
 
static LinkedList_int2hash* LinkedList_int2hash_insert(LinkedList_int2hash* list, Ht_item_int2hash* item) {
    // Inserts the item onto the Linked List
    if (!list) {
        LinkedList_int2hash* head = allocate_list_int2hash();
        head->item = item;
        head->next = NULL;
        list = head;
        return list;
    } 
     
    else if (list->next == NULL) {
        LinkedList_int2hash* node = allocate_list_int2hash();
        node->item = item;
        node->next = NULL;
        list->next = node;
        return list;
    }
 
    LinkedList_int2hash* temp = list;
    while (temp->next->next) {
        temp = temp->next;
    }
     
    LinkedList_int2hash* node = allocate_list_int2hash();
    node->item = item;
    node->next = NULL;
    temp->next = node;
     
    return list;
}
 

 //
static Ht_item_int2hash* LinkedList_int2hash_remove(LinkedList_int2hash* list) {
    // Removes the head from the linked list
    // and returns the item of the popped element
    if (!list)
        return NULL;
    if (!list->next)
        return NULL;
    LinkedList_int2hash* node = list->next;
    LinkedList_int2hash* temp = list;
    temp->next = NULL;
    list = node;
    Ht_item_int2hash* it = NULL;
    memcpy(temp->item, it, sizeof(Ht_item_int2hash));
    // free(temp->item->key);
    // free(temp->item->value);
    free(temp->item);
    free(temp);
    return it;
}
 

static void free_LinkedList_int2hash(LinkedList_int2hash* list) {
    LinkedList_int2hash* temp = list;
    while (list) {
        temp = list;
        list = list->next;
        // free(temp->item->value);
        free(temp->item);
        free(temp);
    }
}
 
static LinkedList_int2hash** create_overflow_buckets_int2hash(HashTable_int2hash* table) {
    // Create the overflow buckets; an array of LinkedList_int2hashs
    LinkedList_int2hash** buckets = (LinkedList_int2hash**) calloc (table->size, sizeof(LinkedList_int2hash*));
    for (int i=0; i<table->size; i++)
        buckets[i] = NULL;
    return buckets;
}
 
static void free_overflow_buckets_int2hash(HashTable_int2hash* table) {
    // Free all the overflow bucket lists
    LinkedList_int2hash** buckets = table->overflow_buckets;
    for (int i=0; i<table->size; i++)
        free_LinkedList_int2hash(buckets[i]);
    free(buckets);
}
 
 
Ht_item_int2hash* create_item_int2hash(int key, float value) {
    // Creates a pointer to a new hash table item
    Ht_item_int2hash* item = (Ht_item_int2hash*) malloc (sizeof(Ht_item_int2hash));
    
    item->key = key;
    item->value = value;
    // item->key = (char*) malloc (strlen(key) + 1);
    // item->value = (char*) malloc (strlen(value) + 1);
     
    // strcpy(item->key, key);
    // strcpy(item->value, value);
 
    return item;
}
 
HashTable_int2hash* create_table_int2hash(int size) {
    // Creates a new HashTable_int2hash
    HashTable_int2hash* table = (HashTable_int2hash*) malloc (sizeof(HashTable_int2hash));
    table->size = size;
    table->count = 0;
    table->items = (Ht_item_int2hash**) calloc (table->size, sizeof(Ht_item_int2hash*));
    for (int i=0; i<table->size; i++)
        table->items[i] = NULL;
    table->overflow_buckets = create_overflow_buckets_int2hash(table);
 
    return table;
}
 
void free_item_int2hash(Ht_item_int2hash* item) {
    // Frees an item
    // free(item->key);
    // free(item->value);
    free(item);
}
 
void free_table_int2hash(HashTable_int2hash* table) {
    // Frees the table
    for (int i=0; i<table->size; i++) {
        Ht_item_int2hash* item = table->items[i];
        if (item != NULL)
            free_item_int2hash(item);
    }
 
    free_overflow_buckets_int2hash(table);
    free(table->items);
    free(table);
}
 
void handle_collision_int2hash(HashTable_int2hash* table, unsigned long index, Ht_item_int2hash* item) {
    LinkedList_int2hash* head = table->overflow_buckets[index];
 
    if (head == NULL) {
        // We need to create the list
        head = allocate_list_int2hash();
        head->item = item;
        table->overflow_buckets[index] = head;
        return;
    }
    else {
        // Insert to the list
        table->overflow_buckets[index] = LinkedList_int2hash_insert(head, item);
        return;
    }
 }
 
void ht_insert_int2hash(HashTable_int2hash* table, int key, float value) {
    // Create the item
    Ht_item_int2hash* item = create_item_int2hash(key, value);
 
    // Compute the index
    unsigned long index = hash_function_int(key);
 
    Ht_item_int2hash* current_item = table->items[index];
     
    if (current_item == NULL) {
        // Key does not exist.
        if (table->count == table->size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            // Remove the create item
            free_item_int2hash(item);
            return;
        }
         
        // Insert directly
        table->items[index] = item; 
        table->count++;
    }
 
    else {
        // Scenario 1: We only need to update value
        if (current_item->key == key) {
            table->items[index]->value = value;
            return;
        }
    
        else {
            // Scenario 2: Collision
            handle_collision_int2hash(table, index, item);
            return;
        }
    }
}
 
float ht_search_int2hash(HashTable_int2hash* table, int key) {
    // Searches the key in the HashTable_int2hash
    // and returns NULL if it doesn't exist
    int index = hash_function_int(key);
    Ht_item_int2hash* item = table->items[index];
    LinkedList_int2hash* head = table->overflow_buckets[index];
 
    // Ensure that we move to items which are not NULL
    while (item != NULL) {
        if (item->key == key)
            return item->value;
        if (head == NULL)
            return FLT_MIN;
        item = head->item;
        head = head->next;
    }
    return FLT_MIN;
}
 
void ht_delete_int2float(HashTable_int2hash* table, int key) {
    // Deletes an item from the table
    int index = hash_function_int(key);
    Ht_item_int2hash* item = table->items[index];
    LinkedList_int2hash* head = table->overflow_buckets[index];
 
    if (item == NULL) {
        // Does not exist. Return
        return;
    }
    else {
        if (head == NULL && item->key == key) {
            // No collision chain. Remove the item
            // and set table index to NULL
            table->items[index] = NULL;
            free_item_int2hash(item);
            table->count--;
            return;
        }
        else if (head != NULL) {
            // Collision Chain exists
            if (item->key == key) {
                // Remove this item and set the head of the list
                // as the new item
                 
                free_item_int2hash(item);
                LinkedList_int2hash* node = head;
                head = head->next;
                node->next = NULL;
                table->items[index] = create_item_int2hash(node->item->key, node->item->value);
                free_LinkedList_int2hash(node);
                table->overflow_buckets[index] = head;
                return;
            }
 
            LinkedList_int2hash* curr = head;
            LinkedList_int2hash* prev = NULL;
             
            while (curr) {
                if (curr->item->key == key) {
                    if (prev == NULL) {
                        // First element of the chain. Remove the chain
                        free_LinkedList_int2hash(head);
                        table->overflow_buckets[index] = NULL;
                        return;
                    }
                    else {
                        // This is somewhere in the chain
                        prev->next = curr->next;
                        curr->next = NULL;
                        free_LinkedList_int2hash(curr);
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
 
void print_search_int2hash(HashTable_int2hash* table, int key) {
    float val;
    if ((val = ht_search_int2hash(table, key)) == FLT_MIN) {
        printf("\"%d\" does not exist\n", key);
        return;
    }
    else {
        printf("Key:%d, Value:%f\n", key, val);
    }
}
 
void print_table_int2hash(HashTable_int2hash* table) {
    printf("\n-------------------\n");
    for (int i=0; i<table->size; i++) {
        if (table->items[i]) {
            printf("Index:%d, Key:%d, Value:%f", i, table->items[i]->key, table->items[i]->value);
            if (table->overflow_buckets[i]) {
                printf(" => Overflow Bucket => ");
                LinkedList_int2hash* head = table->overflow_buckets[i];
                while (head) {
                    printf("Key:%d, Value:%f ", head->item->key, head->item->value);
                    head = head->next;
                }
            }
            printf("\n");
        }
    }
    printf("-------------------\n");
}





//--------------------------------------------

//--------------------------------------------

//--------------------------------------------

//--------------------------------------------

//--------------------------------------------

//--------------------------------------------

//--------------------------------------------

//--------------------------------------------



//------------------------------------------
 
int main() {
    HashTable_int2hash* ht = create_table_int2hash(CAPACITY);
    ht_insert_int2hash(ht, 1, 2.3);
    ht_insert_int2hash(ht, 2, .53);
    ht_insert_int2hash(ht, 3, 0.3);
    ht_insert_int2hash(ht, 50001, 9.99);
    print_search_int2hash(ht, 1);
    print_search_int2hash(ht, 2);
    print_search_int2hash(ht, 3);
    print_search_int2hash(ht, 4);
    print_search_int2hash(ht, 50001);  // Collision!
    print_table_int2hash(ht);
    ht_delete_int2float(ht, 1);
    ht_delete_int2float(ht, 50001);
    print_table_int2hash(ht);
    free_table_int2hash(ht);
    return 0;
}