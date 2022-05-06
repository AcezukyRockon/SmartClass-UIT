#include <iostream>

#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

int main(){
	mongocxx::instance inst{}; // This should be done only once.
	mongocxx::client conn{
	   mongocxx::uri{
	      "mongodb+srv://nhom1:nhom1@smartpodium.ra3hh.mongodb.net/SmartDB?retryWrites=true&w=majority"
	   }
	};
    auto db = conn["SmartDB"];

    // Query for all the documents in a collection.
    {
        auto cursor = db["Trang"].find({});
        for (auto&& doc : cursor) {
            std::cout << bsoncxx::to_json(doc) << std::endl;
        }
    }
}
