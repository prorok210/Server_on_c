#ifndef COLLECTIONS_H
#define COLLECTIONS_H

// Определение структур для коллекций
struct Users {
    char col_name[255];
    char name[255];
    unsigned short age;
};

struct product {
    char col_name[255];
    char name[255];
    unsigned int price;
};

const char* collections_names[] = {
    "users",
    "products"
};

unsigned int num_collections = sizeof(collections_names) / sizeof(collections_names[0]);

#endif // COLLECTIONS_H