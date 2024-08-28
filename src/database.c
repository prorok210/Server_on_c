#include <libmongoc-1.0/mongoc/mongoc.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


int connect_to_db(const char* connection_details, const char* database_name, mongoc_client_t **client, mongoc_database_t **database) {
    bson_error_t error = {0};
    mongoc_server_api_t *api = NULL;
    bson_t *command = NULL;
    bson_t reply = BSON_INITIALIZER;
    int rc = 0;
    bool ok = true;

    mongoc_init();

    char url[2048];

    snprintf(url, sizeof(url), "mongodb+srv://%s@cluster0.bsas9.mongodb.net/?retryWrites=true&w=majority&appName=Cluster0", connection_details);

    *client = mongoc_client_new(url);
        if (!*client) {
            fprintf(stderr, "Failed to create a MongoDB client.\n");
            rc = 1;
        }

    api = mongoc_server_api_new(MONGOC_SERVER_API_V1);
    if (!api) {
        fprintf(stderr, "Failed to create a MongoDB server API.\n");
        rc = 1;
    }

    ok = mongoc_client_set_server_api(*client, api, &error);
    if (!ok) {
        fprintf(stderr, "error: %s\n", error.message);
        rc = 1;
    }

    *database = mongoc_client_get_database(*client, database_name);
    if (!*database) {
        fprintf(stderr, "Failed to get a MongoDB database handle.\n");
        rc = 1;;
    }

    command = BCON_NEW("ping", BCON_INT32(1));
    ok = mongoc_database_command_simple( *database, command, NULL, &reply, &error);
    if (!ok) {
        fprintf(stderr, "error: %s\n", error.message);
        rc = 1;
    }

    bson_destroy(&reply);
    if (command) bson_destroy(command);
    mongoc_server_api_destroy(api);
    if (rc == 1){
        mongoc_database_destroy(*database);
        mongoc_client_destroy(*client);
        mongoc_cleanup();

    } else printf("Successfully connected to MongoDB!\n");

    return rc;
}


void disconnect_from_db(mongoc_client_t *client, mongoc_database_t *database) {
    mongoc_database_destroy(database);
    mongoc_client_destroy(client);
    mongoc_cleanup();
}

