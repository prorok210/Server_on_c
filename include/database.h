#ifndef DATABASE_H
#define DATABASE_H

#include <libmongoc-1.0/mongoc/mongoc.h>

int connect_to_db(
    const char* connection_details, // "<user>:<password>"
    const char* database_name,
    mongoc_client_t **client, 
    mongoc_database_t **database          
);

void disconnect_from_db(
    mongoc_client_t *client, 
    mongoc_database_t *database
);

int check_collections(mongoc_database_t *database);

#endif //DATABASE_H