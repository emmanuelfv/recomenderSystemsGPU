#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define CAPACITY 50000 // Size of the Hash Table
 
unsigned long hash_function_str(char* str) {
    unsigned long i = 0;
    for (int j=0; str[j]; j++)
        i += str[j];
    return i % CAPACITY;
}
 
typedef struct Ht_item_2str Ht_item_2str;
 
// Define the Hash Table Item here
struct Ht_item_2str {
    char* key;
    char* value;
};
 
 
typedef struct LinkedList_2str LinkedList_2str;
 
// Define the LinkedList_2str here
struct LinkedList_2str {
    Ht_item_2str* item; 
    LinkedList_2str* next;
};
 
 
typedef struct HashTable_str2str HashTable_str2str;
 
// Define the Hash Table here
struct HashTable_str2str {
    // Contains an array of pointers
    // to items
    Ht_item_2str** items;
    LinkedList_2str** overflow_buckets;
    int size;
    int count;
};
 
 
static LinkedList_2str* allocate_list_str2str () {
    // Allocates memory for a LinkedList_2str pointer
    LinkedList_2str* list = (LinkedList_2str*) malloc (sizeof(LinkedList_2str));
    return list;
}
 
static LinkedList_2str* LinkedList_2str_insert(LinkedList_2str* list, Ht_item_2str* item) {
    // Inserts the item onto the Linked List
    if (!list) {
        LinkedList_2str* head = allocate_list_str2str();
        head->item = item;
        head->next = NULL;
        list = head;
        return list;
    } 
     
    else if (list->next == NULL) {
        LinkedList_2str* node = allocate_list_str2str();
        node->item = item;
        node->next = NULL;
        list->next = node;
        return list;
    }
 
    LinkedList_2str* temp = list;
    while (temp->next->next) {
        temp = temp->next;
    }
     
    LinkedList_2str* node = allocate_list_str2str();
    node->item = item;
    node->next = NULL;
    temp->next = node;
     
    return list;
}
 
static Ht_item_2str* LinkedList_2str_remove(LinkedList_2str* list) {
    // Removes the head from the linked list
    // and returns the item of the popped element
    if (!list)
        return NULL;
    if (!list->next)
        return NULL;
    LinkedList_2str* node = list->next;
    LinkedList_2str* temp = list;
    temp->next = NULL;
    list = node;
    Ht_item_2str* it = NULL;
    memcpy(temp->item, it, sizeof(Ht_item_2str));
    free(temp->item->key);
    free(temp->item->value);
    free(temp->item);
    free(temp);
    return it;
}
 
static void free_LinkedList_2str(LinkedList_2str* list) {
    LinkedList_2str* temp = list;
    while (list) {
        temp = list;
        list = list->next;
        free(temp->item->key);
        free(temp->item->value);
        free(temp->item);
        free(temp);
    }
}
 
static LinkedList_2str** create_overflow_buckets_str2str(HashTable_str2str* table) {
    // Create the overflow buckets; an array of LinkedList_2strs
    LinkedList_2str** buckets = (LinkedList_2str**) calloc (table->size, sizeof(LinkedList_2str*));
    for (int i=0; i<table->size; i++)
        buckets[i] = NULL;
    return buckets;
}
 
static void free_overflow_buckets_str2str(HashTable_str2str* table) {
    // Free all the overflow bucket lists
    LinkedList_2str** buckets = table->overflow_buckets;
    for (int i=0; i<table->size; i++)
        free_LinkedList_2str(buckets[i]);
    free(buckets);
}
 
 
Ht_item_2str* create_item_str2str(char* key, char* value) {
    // Creates a pointer to a new hash table item
    Ht_item_2str* item = (Ht_item_2str*) malloc (sizeof(Ht_item_2str));
    item->key = (char*) malloc (strlen(key) + 1);
    item->value = (char*) malloc (strlen(value) + 1);
     
    strcpy(item->key, key);
    strcpy(item->value, value);
 
    return item;
}
 
HashTable_str2str* create_table_str2str(int size) {
    // Creates a new HashTable_str2str
    HashTable_str2str* table = (HashTable_str2str*) malloc (sizeof(HashTable_str2str));
    table->size = size;
    table->count = 0;
    table->items = (Ht_item_2str**) calloc (table->size, sizeof(Ht_item_2str*));
    for (int i=0; i<table->size; i++)
        table->items[i] = NULL;
    table->overflow_buckets = create_overflow_buckets_str2str(table);
 
    return table;
}
 
void free_item_2str(Ht_item_2str* item) {
    // Frees an item
    free(item->key);
    free(item->value);
    free(item);
}
 
void free_table_str2str(HashTable_str2str* table) {
    // Frees the table
    for (int i=0; i<table->size; i++) {
        Ht_item_2str* item = table->items[i];
        if (item != NULL)
            free_item_2str(item);
    }
 
    free_overflow_buckets_str2str(table);
    free(table->items);
    free(table);
}
 
void handle_collision_str2str(HashTable_str2str* table, unsigned long index, Ht_item_2str* item) {
    LinkedList_2str* head = table->overflow_buckets[index];
 
    if (head == NULL) {
        // We need to create the list
        head = allocate_list_str2str();
        head->item = item;
        table->overflow_buckets[index] = head;
        return;
    }
    else {
        // Insert to the list
        table->overflow_buckets[index] = LinkedList_2str_insert(head, item);
        return;
    }
 }
 
void ht_insert_str2str(HashTable_str2str* table, char* key, char* value) {
    // Create the item
    Ht_item_2str* item = create_item_str2str(key, value);
 
    // Compute the index
    unsigned long index = hash_function_str(key);
 
    Ht_item_2str* current_item = table->items[index];
     
    if (current_item == NULL) {
        // Key does not exist.
        if (table->count == table->size) {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            // Remove the create item
            free_item_2str(item);
            return;
        }
         
        // Insert directly
        table->items[index] = item; 
        table->count++;
    }
 
    else {
        // Scenario 1: We only need to update value
        if (strcmp(current_item->key, key) == 0) {
            strcpy(table->items[index]->value, value);
            return;
        }
    
        else {
            // Scenario 2: Collision
            handle_collision_str2str(table, index, item);
            return;
        }
    }
}
 
char* ht_search_str2str(HashTable_str2str* table, char* key) {
    // Searches the key in the HashTable_str2str
    // and returns NULL if it doesn't exist
    int index = hash_function_str(key);
    Ht_item_2str* item = table->items[index];
    LinkedList_2str* head = table->overflow_buckets[index];
 
    // Ensure that we move to items which are not NULL
    while (item != NULL) {
        if (strcmp(item->key, key) == 0)
            return item->value;
        if (head == NULL)
            return NULL;
        item = head->item;
        head = head->next;
    }
    return NULL;
}
 
void ht_delete_str2str(HashTable_str2str* table, char* key) {
    // Deletes an item from the table
    int index = hash_function_str(key);
    Ht_item_2str* item = table->items[index];
    LinkedList_2str* head = table->overflow_buckets[index];
 
    if (item == NULL) {
        // Does not exist. Return
        return;
    }
    else {
        if (head == NULL && strcmp(item->key, key) == 0) {
            // No collision chain. Remove the item
            // and set table index to NULL
            table->items[index] = NULL;
            free_item_2str(item);
            table->count--;
            return;
        }
        else if (head != NULL) {
            // Collision Chain exists
            if (strcmp(item->key, key) == 0) {
                // Remove this item and set the head of the list
                // as the new item
                 
                free_item_2str(item);
                LinkedList_2str* node = head;
                head = head->next;
                node->next = NULL;
                table->items[index] = create_item_str2str(node->item->key, node->item->value);
                free_LinkedList_2str(node);
                table->overflow_buckets[index] = head;
                return;
            }
 
            LinkedList_2str* curr = head;
            LinkedList_2str* prev = NULL;
             
            while (curr) {
                if (strcmp(curr->item->key, key) == 0) {
                    if (prev == NULL) {
                        // First element of the chain. Remove the chain
                        free_LinkedList_2str(head);
                        table->overflow_buckets[index] = NULL;
                        return;
                    }
                    else {
                        // This is somewhere in the chain
                        prev->next = curr->next;
                        curr->next = NULL;
                        free_LinkedList_2str(curr);
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
 
void print_search_str2str(HashTable_str2str* table, char* key) {
    char* val;
    if ((val = ht_search_str2str(table, key)) == NULL) {
        printf("%s does not exist\n", key);
        return;
    }
    else {
        printf("Key:%s, Value:%s\n", key, val);
    }
}
 
void print_table_str2str(HashTable_str2str* table) {
    printf("\n-------------------\n");
    for (int i=0; i<table->size; i++) {
        if (table->items[i]) {
            printf("Index:%d, Key:%s, Value:%s", i, table->items[i]->key, table->items[i]->value);
            if (table->overflow_buckets[i]) {
                printf(" => Overflow Bucket => ");
                LinkedList_2str* head = table->overflow_buckets[i];
                while (head) {
                    printf("Key:%s, Value:%s ", head->item->key, head->item->value);
                    head = head->next;
                }
            }
            printf("\n");
        }
    }
    printf("-------------------\n");
}
 
int main() {
    HashTable_str2str* ht = create_table_str2str(CAPACITY);
    ht_insert_str2str(ht, "1", "First address");
    ht_insert_str2str(ht, "2", "Second address");
    ht_insert_str2str(ht, "Hel", "Third address");
    ht_insert_str2str(ht, "Cau", "Fourth address");
    print_search_str2str(ht, "1");
    print_search_str2str(ht, "2");
    print_search_str2str(ht, "3");
    print_search_str2str(ht, "Hel");
    print_search_str2str(ht, "Cau");  // Collision!
    print_table_str2str(ht);
    ht_delete_str2str(ht, "1");
    ht_delete_str2str(ht, "Cau");
    print_table_str2str(ht);
    free_table_str2str(ht);
    return 0;
}