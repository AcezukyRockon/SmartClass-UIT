// install: sudo apt install libjson-c-dev
// compile: gcc -o query_local_parse query_local_parse.c $(pkg-config --cflags --libs libmongoc-1.0) -ljson-c

#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <stdio.h>
#include <json-c/json.h>

int main (int argc, char *argv[])
{
	mongoc_client_t *client;
	mongoc_collection_t *collection;
	mongoc_cursor_t *cursor;
	const bson_t *doc;
	bson_t *query;
	char *str;
	struct json_object *parsed_json;
	struct json_object *_id;
	struct json_object *class_id;
	struct json_object *timestamp;

	mongoc_init ();

	client = mongoc_client_new ("mongodb://localhost:27017/?appname=insert-example");
	collection = mongoc_client_get_collection (client, "SmartDB", "Trang");
	query = bson_new ();
	cursor = mongoc_collection_find_with_opts (collection, query, NULL, NULL);

	while (mongoc_cursor_next (cursor, &doc)) 
	{
		str = bson_as_canonical_extended_json (doc, NULL);
		printf ("%s\n", str);
		parsed_json = json_tokener_parse(str);
		json_object_object_get_ex(parsed_json, "_id", &_id);
		json_object_object_get_ex(parsed_json, "class_id", &class_id);
		json_object_object_get_ex(parsed_json, "timestamp", &timestamp);
		printf("_id: %s\n", json_object_get_string(_id));
		printf("class_id: %s\n", json_object_get_string(class_id));
		printf("timestamp: %s\n", json_object_get_string(timestamp));
		bson_free (str);
		

	}

	bson_destroy (query);
	mongoc_cursor_destroy (cursor);
	mongoc_collection_destroy (collection);
	mongoc_client_destroy (client);
	mongoc_cleanup ();
	//printf("Hello World");
	return 0;
}
