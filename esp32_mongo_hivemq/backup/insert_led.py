def get_database():
    from pymongo import MongoClient
    import pymongo
    import certifi

    # Provide the mongodb atlas url to connect python to mongodb using pymongo
    ca = certifi.where()
    CONNECTION_STRING = "mongodb+srv://nhom1:nhom1@smartpodium.ra3hh.mongodb.net/SmartDB?retryWrites=true&w=majority"

    # Create a connection using MongoClient. You can import MongoClient or use pymongo.MongoClient
    from pymongo import MongoClient
    client = MongoClient(CONNECTION_STRING, tlsCAFile = ca)

    # Create the database for our example (we will use the same database throughout the tutorial
    return client['SmartDB']
    
# This is added so that many files can reuse the function get_database()
if __name__ == "__main__":    
    
    # Get the database
    dbname = get_database()

    #Creating a collection in Python
    collection_name = dbname["LED"]

    #Inserting documents in Python
    # item_1 = {
    # "_id" : "18521522",
    # "hoten" : "Ha Kieu Trang",
    # "max_discount" : "10%",
    # "batch_number" : "RR450020FRG",
    # "price" : 340,
    # "category" : "kitchen appliance"
    # }

    # item_2 = {
    # "_id" : "U1IT00002",
    # "item_name" : "Egg",
    # "category" : "food",
    # "quantity" : 12,
    # "price" : 36,
    # "item_description" : "brown country eggs"
    # }
    # collection_name.insert_many([item_1,item_2])

    from dateutil import parser
    expiry_date = '2021-07-13T00:00:00.000Z'
    expiry = parser.parse(expiry_date)
    item_3 = {
	"Status" : "ON"
    }
    collection_name.insert_one(item_3)
