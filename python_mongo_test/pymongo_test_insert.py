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
    collection_name = dbname["Trang"]

    #Inserting documents in Python
    import time
    from dateutil import parser
    from datetime import datetime
    malop = "CE410.M21.MTCL"
    for i in range(1,5):
        item = {
        "_id" : i,
        "malop" : malop
        }
        ts = time.time()
        item['timestamp'] = datetime.utcfromtimestamp(ts)
        collection_name.insert_one(item)
