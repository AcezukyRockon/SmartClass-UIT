#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <stdio.h>
#include <time.h>

int main (int   argc,
      char *argv[])
{
    mongoc_client_t *client;
    mongoc_collection_t *collection;
    bson_error_t error;
    bson_oid_t oid;
    bson_t *doc;
    
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char *s = asctime (timeinfo);
    s[strlen(s)-1]  = '\0';
    //printf ( "Current local time and date: %s", asctime (timeinfo) );

    mongoc_init ();

    client = mongoc_client_new ("mongodb+srv://nhom1:nhom1@smartpodium.ra3hh.mongodb.net/SmartDB?retryWrites=true&w=majority");
    collection = mongoc_client_get_collection (client, "SmartDB", "test");

    doc = bson_new ();
    bson_oid_init (&oid, NULL);
    BSON_APPEND_OID (doc, "_id", &oid);
    BSON_APPEND_UTF8 (doc, "timestamp", s);

    if (!mongoc_collection_insert_one (
           collection, doc, NULL, NULL, &error)) {
        fprintf (stderr, "%s\n", error.message);
    }

    bson_destroy (doc);
    mongoc_collection_destroy (collection);
    mongoc_client_destroy (client);
    mongoc_cleanup ();

    return 0;
}
