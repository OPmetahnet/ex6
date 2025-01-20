#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char *str)
{
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0)
    {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r'))
    {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src)
{
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (!dest)
    {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt)
{
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success)
    {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0')
        {
            printf("Invalid input.\n");
        }
        else
        {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type)
{
    switch (type)
    {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput()
{
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *)malloc(capacity);
    if (!input)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (size + 1 >= capacity)
        {
            capacity *= 2;
            char *temp = (char *)realloc(input, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node)
{
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

// Function to add a given owner to the existing list of owners
void linkOwnerInCircularList(OwnerNode *owner) {
    // 1) if there are no owners - make the head point to the new owner
    if(ownerHead == NULL) {
        ownerHead = owner;
        return;
    }

    OwnerNode* temp = ownerHead;

    // 2) if the list contains more than the head node - search for the node which points to head
    if(ownerHead->next != NULL) {
        while(temp->next != ownerHead) {
            temp = temp->next;
        }
    }

    // 3) set its next pointer to owner and vice versa
    temp->next = owner;
    owner->prev = temp;

    // 4) set head's prev node pointer to owner and vice versa
    ownerHead->prev = owner;
    owner->next = ownerHead;
}

//Function to create new Pokemon data matching the Pokemon we want to create
PokemonData *createPokemonData(const PokemonData pokedexEntry) {
    // 1) allocate data for the data
    PokemonData* newPokemon = (PokemonData *)malloc(sizeof(PokemonData));
    if(newPokemon == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // 2) copy the Pokedex entry details into the new Pokemon's data we've created
    newPokemon->id = pokedexEntry.id;
    newPokemon->name = myStrdup(pokedexEntry.name);
    newPokemon->TYPE = pokedexEntry.TYPE;
    newPokemon->hp = pokedexEntry.hp;
    newPokemon->attack = pokedexEntry.attack;
    newPokemon->CAN_EVOLVE = pokedexEntry.CAN_EVOLVE;

    return newPokemon;
}

// Function to create a new Pokemon node based on the given Pokemon's data
PokemonNode *createPokemonNode(PokemonData *data) {
    // 1) allocate new memory
    PokemonNode* newPokemon = (PokemonNode*)malloc(sizeof(PokemonNode));
    if (newPokemon == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // 2) init data
    newPokemon->data = data;
    newPokemon->left = NULL;
    newPokemon->right = NULL;

    return newPokemon;
}

// Function to insert a given PokemonNode to the Owner's Pokedex tree
PokemonNode *insertPokemonNode(PokemonNode *root, PokemonNode *newNode) {
    // 1) If the Pokedex is empty make the root the new Pokemon's node
    if(root == NULL) {
        root = newNode;
        return root;
    }

    // 2) Check for the right placement for the Pokemon to be put in(by ID)
    if(newNode->data->id < root->data->id) {
        root->left = insertPokemonNode(root->left, newNode);
    }
    if(newNode->data->id > root->data->id) {
       root->right = insertPokemonNode(root->right, newNode);
    }

    return root;
}

// Function to search for a Pokemon in a given owner's Pokedex.
PokemonNode *searchPokemonBFS(PokemonNode *root, int id) {
    // 1) If the pokemon wasn't found - return NULL
    if(root == NULL) {
        return root;
    }

    // 2) if the root's ID matches - return its node
    if(root->data->id == id) {
        return root;
    }

    // 3) Compare the left/right node ID values with the given ID and progress accordingly
    if(id < root->data->id) {
        return searchPokemonBFS(root->left, id);
    }
    if(id > root->data->id) {
        return searchPokemonBFS(root->right, id);
    }

    return root;
}

// --------------------------------------------------------------
// Pokemon Addition
// --------------------------------------------------------------
void addPokemon(OwnerNode *owner) {
    // 1) Get the pokemon's ID from the user
    int pokemonId = readIntSafe("Enter ID to add:");

    // 2) Validate the ID (if it's in range of 1-151 - Pokedex entries)
    if(pokemonId < 1 || pokemonId > 151) {
        printf("Invalid ID.\n");
        return;
    }

    // 2) Check if the pokemon already exists in the pokedex
    if(searchPokemonBFS(owner->pokedexRoot, pokemonId) != NULL) {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", pokemonId);
        return;
    }

    // 4) create the new Pokemon node to be entered
    PokemonData* newPokemonData = createPokemonData(pokedex[pokemonId - 1]);
    PokemonNode* newPokemonNode = createPokemonNode(newPokemonData);

    //3) If it doesn't - insert it to the pokedex
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, newPokemonNode);
    printf("Pokemon %s (ID %d) added.\n", newPokemonData->name, newPokemonData->id);
}

// Function to create a new owner and add it to the linked-list of owners
OwnerNode *createOwner(char *ownerName, PokemonNode *starter) {
    // 1) allocate memory for the new owner
    OwnerNode *newOwner = (OwnerNode *)malloc(sizeof(OwnerNode));
    if (newOwner == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    // 2) copy properties into the owner
    newOwner->ownerName = ownerName;
    newOwner->pokedexRoot = starter;
    newOwner->next = NULL;
    newOwner->prev = NULL;

    return newOwner;
}

// --------------------------------------------------------------
// Open a new Pokedex
// --------------------------------------------------------------
void openPokedexMenu() {
    // 1) get the name and starter choice form the user
    printf("Your name:");
    char* trainerName = getDynamicInput();
    int starterChoice = readIntSafe("Choose Starter:\n"
                 "1. Bulbasaur\n"
                 "2. Charmander\n"
                 "3. Squirtle\n"
                 "Your Choice:");

    PokemonData* data = NULL;

    // 2) set the new Pokemon Data according to the starter that was picked
    switch(starterChoice) {
        //set data to Bulbasaur's data
        case 1: {
            data = createPokemonData(pokedex[0]);
            break;
        }
        //set data to Charmander's data
        case 2: {
            data = createPokemonData(pokedex[3]);
            break;
        }
        //set data to Squirtle's data
        case 3: {
            data = createPokemonData(pokedex[6]);
            break;
        }
        //invalid choice case: free trainerName and go back to main menu
        default: {
            printf("Invalid choice.\n");
            free(trainerName);
            return;
        }
    }

    // 3) create the starter's Pokemon Node based on the data we made
    PokemonNode* starter = createPokemonNode(data);

    // 4) create the new owner
    OwnerNode* newOwner = createOwner(trainerName, starter);

    // 5) add the owner to the list of owners
    linkOwnerInCircularList(newOwner);

    printf("New Pokedex created for %s with starter %s.\n", trainerName, starter->data->name);
}

// Function to print all owners once in a numbered menu
void printAllOwners() {
    // 1) If the list is empty print nothing and return
    if(ownerHead == NULL) {
        return;
    }

    printf("1. %s\n", ownerHead->ownerName);
    // 2) If the list is comprised only of one node print it and return
    if(ownerHead->next == NULL) {
        return;
    }

    int i = 2;
    OwnerNode* printNode = ownerHead->next;

    // 3) print all owners in a menu list
    while(printNode != ownerHead) {
        printf("%d. %s\n", i, printNode->ownerName);
        i++;
        printNode = printNode->next;
    }
}

// Function to find the owner by order number starting from head
OwnerNode *findOwnerByPosition(int orderPosition) {
    // 1) If there are no owners - return
    if(ownerHead == NULL) {
        return ownerHead;
    }

    OwnerNode* searchNode = ownerHead;

    // 2) keep going until the number of movements needed is met
    while(orderPosition > 0 && searchNode->next != NULL) {
        searchNode = searchNode->next;
        orderPosition--;
    }

    return searchNode;
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
void displayMenu(OwnerNode *owner)
{
    if (!owner->pokedexRoot)
    {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    switch (choice)
    {
    case 1:
        displayBFS(owner->pokedexRoot);
        break;
    case 2:
        preOrderTraversal(owner->pokedexRoot);
        break;
    case 3:
        inOrderTraversal(owner->pokedexRoot);
        break;
    case 4:
        postOrderTraversal(owner->pokedexRoot);
        break;
    case 5:
        displayAlphabetical(owner->pokedexRoot);
        break;
    default:
        printf("Invalid choice.\n");
    }
}

// Function to add a Pokemon node to a given queue
void enqueue(Queue* queue, PokemonNode *node) {
    QueueNode* newNode = (QueueNode *)malloc(sizeof(QueueNode));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    //point the QueueNode's PokemonNode to the given node we wish to enqueue
    newNode->treeNode = node;
    newNode->next = NULL;

    //if the queue is empty - make both first and last nodes the single node we enqueue
    if(queue->rear == NULL) {
        queue->rear = newNode;
        queue->front = newNode;
    }
    //otherwise, make the new node the next element of the queue
    else {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }
}

// Function to pop the first-out PokemonNode of a given queue an return it
PokemonNode *dequeue(Queue* queue) {
    if(queue->front == NULL) {
        return NULL;
    }

    //store the last QueueNode and its PokemonNode
    QueueNode* temp = queue->front;
    PokemonNode* treeTemp = temp->treeNode;
    //advance the queue forward
    queue->front = queue->front->next;

    //if the queue has been emptied - make both rear and front NULL
    if(queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp);
    return treeTemp;
}

// Function to display all Pokemon using BFS method
void displayBFS(PokemonNode *root) {
    VisitNodeFunc nodePrintPtr = printPokemonNode;

    //create a queue to handle the order of Pokemon tree nodes
    Queue* bfsQueue = malloc(sizeof(Queue));
    if(bfsQueue == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    bfsQueue->front = NULL;
    bfsQueue->rear = NULL;

    //enqueue the root node
    enqueue(bfsQueue, root);

    //go over the queue's nodes and print them as long as the queue still has nodes in it
    while(bfsQueue->front != NULL) {
        //take the first node to come out of the queue and print it
        PokemonNode* current = dequeue(bfsQueue);
        nodePrintPtr(current);

        //check both sides for child nodes and enqueue them
        if(current->left != NULL) {
            enqueue(bfsQueue, current->left);
        }
        if(current->right != NULL) {
            enqueue(bfsQueue, current->right);
        }
    }

    free(bfsQueue);
}

// Function to traverse the tree in pre-order method
void preOrderTraversal(PokemonNode *root) {
    VisitNodeFunc nodePrintPtr = printPokemonNode;
    preOrderGeneric(root, nodePrintPtr);
}

// Function to print nodes based on given fPtr and Pokedex tree using pre-order
void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if(root == NULL) {
        return;
    }
    //print root first
    visit(root);
    //print left side second
    preOrderGeneric(root->left, visit);
    //print right side last
    preOrderGeneric(root->right, visit);
}

// Function to traverse nodes by in-order method
void inOrderTraversal(PokemonNode *root) {
    VisitNodeFunc nodePrintPtr = printPokemonNode;
    inOrderGeneric(root, nodePrintPtr);
}

// Function to print nodes based on given fPtr and Pokedex tree using in-order
void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if(root == NULL) {
        return;
    }
    //print left side first
    inOrderGeneric(root->left, visit);
    //print root second
    visit(root);
    //print right side last
    inOrderGeneric(root->right, visit);
}

// Function to traverse nodes by post-order method
void postOrderTraversal(PokemonNode *root) {
    VisitNodeFunc nodePrintPtr = printPokemonNode;
    postOrderGeneric(root, nodePrintPtr);
}

// Function to print nodes based on given fPtr and Pokedex tree using post-order
void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if(root == NULL) {
        return;
    }
    //print left side first
    postOrderGeneric(root->left, visit);
    //print right side second
    postOrderGeneric(root->right, visit);
    //print root last
    visit(root);
}

// Function to alphabetically all nodes alphabetically
void displayAlphabetical(PokemonNode *root) {
    VisitNodeFunc nodePrintPtr = printPokemonNode;

    //init node array and copy all nodes into it
    NodeArray* nodeArray = malloc(sizeof(NodeArray));
    if(nodeArray == NULL) {
        printf("Memory allocation error.\n");
        exit(1);
    }
    initNodeArray(nodeArray, 1);
    collectAll(root, nodeArray);

    //sort the array using a generic Pokemon node name comparison function
    qsort(nodeArray->nodes, nodeArray->size, sizeof(PokemonNode*), compareByNameNode);

    //print the sorted array by its order
    for(int i = 0; i < nodeArray->size; i++) {
        nodePrintPtr(nodeArray->nodes[i]);
    }

    //free the node array
    free(nodeArray->nodes);
    free(nodeArray);
}

// Function to init the node array with a given capacity
void initNodeArray(NodeArray *na, int cap) {
    na->capacity = cap;
    na->nodes = malloc(cap * sizeof(PokemonNode*));
    if(na->nodes == NULL) {
        printf("Memory allocation error.\n");
        exit(1);
    }
    na->size = 0;
}

// Function to copy all given Pokedex tree nodes into a NodeArray
void collectAll(PokemonNode *root, NodeArray *na) {
    if(root == NULL || na == NULL) {
        return;
    }
    addNode(na, root);
    collectAll(root->left, na);
    collectAll(root->right, na);
}

// Function to add a Pokemon node into a given node array
void addNode(NodeArray *na, PokemonNode *node) {
    if(node == NULL || na == NULL) {
        return;
    }

    //if the capacity was reached - double it
    if(na->size == na->capacity) {
        na->capacity *= 2;
    }

    //reallocate space for one more node in the node array
    PokemonNode** temp = realloc(na->nodes, na->capacity * sizeof(PokemonNode*));
    if(temp == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    na->nodes = temp;

    na->nodes[na->size++] = node;
}

// Function to compare to Pokemon names for two given Pokemon nodes
int compareByNameNode(const void *a, const void *b) {
    PokemonNode* nodeA = *(PokemonNode**)a;
    PokemonNode* nodeB = *(PokemonNode**)b;
    char* name1 = myStrdup(nodeA->data->name);
    char* name2 = myStrdup(nodeB->data->name);
    int result = strcmp(name1, name2);
    free(name1);
    free(name2);
    return result;
}

// --------------------------------------------------------------
// Pokemon Releasing
// --------------------------------------------------------------

// Function to find the min element in a BST
PokemonNode* findMinTreeElement(PokemonNode* root) {
    PokemonNode* minFinder = root->right;

    //find the leftmost member of the tree
    while(minFinder != NULL && minFinder->left != NULL) {
        minFinder = minFinder->left;
    }
    return minFinder;
}

// Function to remove the node from the tree based on its ID
PokemonNode *removeNodeBST(PokemonNode *root, int id) {
    // 1) If the id isn't found - return
    if(root == NULL) {
        printf("No Pokemon with ID %d found.\n", id);
        return root;
    }

    // 2) If ID is smaller go left
    if(id < root->data->id) {
        root->left = removePokemonByID(root->left, id);
    }
    // 3) If ID is larger go right
    else if(id > root->data->id) {
        root->right = removePokemonByID(root->right, id);
    }
    // 4) If ID has been found:
    else {
        printf("Removing Pokemon %s (ID %d).\n", pokedex[id - 1].name, id);
        // 4.1) If there is only one child from the right
        if(root->left == NULL) {
            PokemonNode* temp = root->right;
            freePokemonNode(root);
            return temp;
        }
        // 4.2) If there is only one child from the left
        if(root->right == NULL) {
            PokemonNode* temp = root->left;
            freePokemonNode(root);
            return temp;
        }
        // 4.3) If the node has two children - find the successor, copy its data and free it
        PokemonNode* temp = findMinTreeElement(root->right);
        freePokemonData(root->data);
        root->data = temp->data;
        root->right = removePokemonByID(root->right, temp->data->id);
    }

    return root;
}

// Function to search for a node to delete and get rid of it if found
PokemonNode *removePokemonByID(PokemonNode *root, int id) {
    root = removeNodeBST(root ,id);

    return root;
}

// Function to free a pokemon by its given id(prompted to user prior)
void freePokemon(OwnerNode *owner) {
    // 1) If the Pokedex is empty - print message and return
    if(owner->pokedexRoot == NULL) {
        printf("No Pokemon to release.\n");
        return;
    }
    // 2) Get the ID of the Pokemon the user wishes to release
    int idToFree = readIntSafe("Enter Pokemon ID to release: ");

    // 3) Remove the Pokemon's node from the Owner's Pokemon tree
    owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, idToFree);
}

// --------------------------------------------------------------
// Pokemon Battling
// --------------------------------------------------------------
void pokemonFight(OwnerNode *owner) {
    // 1) If the Pokedex is empty - print message and return
    if(owner->pokedexRoot == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }

    // 2) Get two ID's for the fighting Pokemon from the user
    int battleIds[2];
    battleIds[0] = readIntSafe("Enter ID of the first Pokemon: ");
    battleIds[1] = readIntSafe("Enter ID of the second Pokemon: ");

    // 3) Check for both Pokemon in the Pokedex
    PokemonNode* searchResults[2];
    for(int i = 0; i < 2; i++) {
        searchResults[i] = searchPokemonBFS(owner->pokedexRoot, battleIds[i]);
        if(searchResults[i] == NULL) {
            printf("One or both Pokemon IDs not found.\n");
            return;
        }
    }

    // 4) If both are in the Pokedex - Fight!
    float scores[2];
    //print the Pokemon and their scores
    for(int i = 0; i < 2; i++) {
        scores[i] = (float)(searchResults[i]->data->attack * 1.5) + (float)(searchResults[i]->data->hp * 1.2);
        printf("Pokemon %d: %s (Score = %.2f)\n", i + 1, searchResults[i]->data->name, scores[i]);
    }

    // 5) Handle different outcome cases
    if(scores[0] > scores[1]) {
        printf("%s wins!\n", searchResults[0]->data->name);
    }
    else if(scores[1] > scores[0]) {
        printf("%s wins!\n", searchResults[1]->data->name);
    }
    else {
        printf("It's a tie!\n");
    }
}

// --------------------------------------------------------------
// Pokemon Evolution
// --------------------------------------------------------------
void evolvePokemon(OwnerNode *owner) {
    // 1) If the owner has no Pokemon - print message and return
    if(owner->pokedexRoot == NULL) {
        printf("Cannot evolve. Pokedex empty.\n");
        return;
    }

    // 2) Get the ID of the Pokemon to evolve and try to locate it
    int idToEvolve = readIntSafe("Enter ID of Pokemon to evolve: ");
    PokemonNode* pokemonToEvolve = searchPokemonBFS(owner->pokedexRoot, idToEvolve);
    if(pokemonToEvolve == NULL) {
        printf("No Pokemon with ID %d found.\n", idToEvolve);
        return;
    }

    // 3) Check if the Pokemon can evolve
    if(pokemonToEvolve->data->CAN_EVOLVE == CANNOT_EVOLVE){
        printf("%s (ID %d) cannot evolve.\n", pokemonToEvolve->data->name, pokemonToEvolve->data->id);
        return;
    }

    //if the evolved form already exists in the Pokedex - release the unevolved form
    if(searchPokemonBFS(owner->pokedexRoot, idToEvolve + 1) != NULL) {
        printf("Evolution ID %d (%s) already in the Pokedex. Releasing %s (ID %d).\n",
                      idToEvolve + 1,
                      pokedex[idToEvolve].name,
                      pokedex[idToEvolve - 1].name,
                      idToEvolve);
        owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, idToEvolve);
        return;
    }

    //if the Pokemon can evolve - remove the old form and insert the new
    owner->pokedexRoot = removePokemonByID(owner->pokedexRoot, idToEvolve);
    PokemonData* evolutionData = createPokemonData(pokedex[idToEvolve]);
    PokemonNode* evolvedPokemon = createPokemonNode(evolutionData);
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, evolvedPokemon);
    printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n",
                  pokedex[idToEvolve - 1].name,
                  idToEvolve,
                  pokedex[idToEvolve].name,
                  idToEvolve + 1);
}

// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu()
{
    //make sure the list isn't empty
    if(ownerHead == NULL) {
        printf("No existing Pokedexes.\n");
        return;
    }

    // list owners
    printf("\nExisting Pokedexes:\n");
    printAllOwners();

    //get owner choice from the user
    int pokedexChoice = readIntSafe("Choose a Pokedex by number: ");

    //try to find the owner in the list
    OwnerNode* cur = findOwnerByPosition(pokedexChoice - 1);

    printf("\nEntering %s's Pokedex...\n", cur->ownerName);

    int subChoice;
    do
    {
        printf("\n-- %s's Pokedex Menu --\n", cur->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        subChoice = readIntSafe("Your choice: ");

        switch (subChoice)
        {
        case 1:
            addPokemon(cur);
            break;
        case 2:
            displayMenu(cur);
            break;
        case 3:
            freePokemon(cur);
            break;
        case 4:
            pokemonFight(cur);
            break;
        case 5:
            evolvePokemon(cur);
            break;
        case 6:
            printf("Back to Main Menu.\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}

// Function to free given Pokemon data in its entirety
void freePokemonData(PokemonData *data) {
    // 1) if the data is empty - go back
    if(data == NULL) {
        return;
    }

    // 2) free all data contents
    free(data->name);
    data->name = NULL;
    free(data);
    data = NULL;
}

// Function to free the entire data of a given Pokemon Node
void freePokemonNode(PokemonNode *node) {
    // 1) if the node is empty - go back
    if (node == NULL) {
        return;
    }

    // 2) free all the node's Pokemon data
    freePokemonData(node->data);

    // 3) free the node itself
    free(node);
    node = NULL;
}

// Function to free all nodes of an owner's Pokedex tree
void freePokemonTree(PokemonNode *root) {
    // 1) if the node is empty - go back
    if(root == NULL) {
        return;
    }

    // 2) go node by node recursively and free its contents
    freePokemonTree(root->left);
    freePokemonTree(root->right);

    // 3) free the node's Pokemon data
    freePokemonNode(root);
}

// Function to free the entirety of an owner's Pokedex data
void freeOwnerNode(OwnerNode *owner) {
    // 1) check if the owner has data, if no - go back
    if (owner == NULL) {
        return;
    }

    // 2) free trainer name data
    free(owner->ownerName);
    owner->ownerName = NULL;

    // 3) free the trainer's Pokedex tree
    freePokemonTree(owner->pokedexRoot);

    // 4) free the node itself
    free(owner);
    owner = NULL;
}

// Function to free all memory related to owners
void freeAllOwners() {
    // 1) if the linked list is already empty - no need to free any data
    if(ownerHead == NULL) {
        return;
    }

    // 2) go over every element of the list and free its contents - until the head pointer is reached
    OwnerNode* currentNode = ownerHead;
    OwnerNode* traversionNode = NULL;

    //simple linked list navigation using two node* pointers
    do {
        //progress traversion node by one step
        traversionNode = currentNode->next;
        //free the current node's memory
        freeOwnerNode(currentNode);
        //progress the current node to the next
        if(traversionNode != NULL) {
            currentNode = traversionNode;
        }
    } while (currentNode != ownerHead);

    //avoid dangling head pointer
    ownerHead = NULL;
}

// --------------------------------------------------------------
// Pokedex Deletion
// --------------------------------------------------------------

// Function to remove a target owner from the linked list of owners
void removeOwnerFromCircularList(OwnerNode **target) {
    //the list is circular, so checking either next or prev's existence is enough
    if((*target)->next != NULL) {
        (*target)->next->prev = (*target)->prev;
        (*target)->prev->next = (*target)->next;
    }

    //if there is only one node left
    if((*target)->prev == *target && (*target)->next == *target) {
        freeOwnerNode(*target);
        *target = NULL;
        ownerHead = NULL;
        return;
    }

    // if the node is the head node - update its pointer value
    if(ownerHead == *target) {
        ownerHead = (*target)->next;
    }

    freeOwnerNode(*target);
    *target = NULL;
}

// Function to prompt user for a Pokedex to delete
void deletePokedex() {
    // 1) If there are no Pokedex return
    if(ownerHead == NULL) {
        printf("No existing Pokedexes to delete.\n");
        return;
    }

    // 2) Get the Pokedex to delete from the user
    printf("=== Delete a Pokedex ===\n");
    printAllOwners();
    int pokedexToDelete = readIntSafe("Choose a Pokedex to delete by number: ");
    OwnerNode* nodeToDelete = findOwnerByPosition(pokedexToDelete - 1);
    OwnerNode** nodePointer = &nodeToDelete;
    printf("Deleting %s's entire Pokedex...\nPokedex deleted.\n", nodeToDelete->ownerName);
    removeOwnerFromCircularList(nodePointer);
    nodeToDelete = NULL;
}

// --------------------------------------------------------------
// Pokedex Merging
// --------------------------------------------------------------
void mergePokedexMenu() {
    //make sure that there are at least two Pokedexes
    if(ownerHead == NULL || ownerHead->next == NULL) {
        printf("Not enough owners to merge.\n");
        return;
    }

    //get the owner names from the user
    printf("\n=== Merge Pokedexes ===\nEnter name of first owner: ");
    char* name1 = getDynamicInput();
    printf("Enter name of second owner: ");
    char* name2 = getDynamicInput();

    //try to find both owners in the list(by name)
    OwnerNode* owner1 = findOwnerByName(name1);
    OwnerNode* owner2 = findOwnerByName(name2);

    //if owners were'nt found in the list
    if(owner1 == NULL || owner2 == NULL) {
        printf("One or both owners not found.\n");
        free(name1);
        free(name2);
        return;
    }

    //merge the 2nd owner into the 1st
    printf("Merging %s and %s...\n", name1, name2);
    mergePokedexes(owner1, owner2);

    //prevent the merged Pokemon from being freed accidentally
    owner2->pokedexRoot = NULL;
    //delete the 2nd owner
    removeOwnerFromCircularList(&owner2);
    printf("Merge completed.\nOwner '%s' has been removed after merging.\n", name2);

    free(name1);
    free(name2);
}

// Function to find an owner by their given name
OwnerNode *findOwnerByName(const char *name) {
    if(ownerHead == NULL) {
        return NULL;
    }

    OwnerNode* currentNode = ownerHead;

    //go over the rest of the list and compare the names to the given name
     do {
        if(strcmp(currentNode->ownerName, name) == 0) {
            return currentNode;
        }
        if(currentNode->next != NULL) {
            currentNode = currentNode->next;
        }
    } while(currentNode != ownerHead);

    return NULL;
}

// Function merge an owner into another owner(2->1) using BFS
void mergePokedexes(OwnerNode* owner1, OwnerNode* owner2) {
    if(owner1 == NULL || owner2 == NULL) {
        return;
    }

    //init node array to use for BFS order
    NodeArray* nodeArray = malloc(sizeof(NodeArray));
    if(nodeArray == NULL) {
        printf("Memory allocation error.\n");
        exit(1);
    }
    initNodeArray(nodeArray, 1);

    //insert all of owner2's Pokemon into the node array
    collectAll(owner2->pokedexRoot, nodeArray);

    //go over all of the node array's nodes and insert them one by one to owner1's Pokedex tree
    for(int i = 0; i < nodeArray->size; i++) {
        //if the pokemon is not already found in the Pokedex - insert it
        if(searchPokemonBFS(owner1->pokedexRoot, nodeArray->nodes[i]->data->id) == NULL) {
            insertPokemonNode(owner1->pokedexRoot, nodeArray->nodes[i]);
        }
    }

    free(nodeArray->nodes);
    free(nodeArray);
}

// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu()
{
    int choice;
    do
    {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice:");

        switch (choice)
        {
        case 1:
            openPokedexMenu();
            break;
        case 2:
            enterExistingPokedexMenu();
            break;
        case 3:
            deletePokedex();
            break;
        case 4:
            //mergePokedexMenu();
            break;
        case 5:
            //sortOwners();
            break;
        case 6:
            //printOwnersCircular();
            break;
        case 7:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid.\n");
        }
    } while (choice != 7);
}

int main()
{
    mainMenu();
    freeAllOwners();
    return 0;
}
