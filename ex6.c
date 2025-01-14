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
void addOwner(OwnerNode *owner) {
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

// Function to create a new Pokedex
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
    addOwner(newOwner);

    printf("New Pokedex created for %s with starter %s.\n", trainerName, starter->data->name);
}
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

// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu()
{
    // list owners
    printf("\nExisting Pokedexes:\n");
    // you need to implement a few things here :)

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
            mergePokedexMenu();
            break;
        case 5:
            sortOwners();
            break;
        case 6:
            printOwnersCircular();
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
