# Get the database using the method we defined in pymongo_test_insert file 
from pymongo_test_insert import get_database
import time

# Create a new collection
dbname = get_database()
collection_name = dbname["Trang"]
item_details = collection_name.find()
#category_index = collection_name.create_index("category")
#item_details = collection_name.find({"category" : "food"})

from pandas import DataFrame
items_df = DataFrame(item_details)
print(items_df)	

# print each document
# for item in item_details:
#     # see the magic
#     print(item)