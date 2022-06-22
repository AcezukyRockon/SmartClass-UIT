var MongoClient = require('mongodb').MongoClient;
var url = "mongodb://localhost:27017/";

MongoClient.connect(url, function(err, db) {
  if (err) throw err;
  var adminDb = db.db("SmartDB").admin();
  adminDb.listDatabases(function(err, result) {
    console.log(result.databases);
    db.close();
  });

  /*var dbo = db.db("SmartDB");
  if (tb == undefined) {
    console.log("nothing to retrieve");
    dbo.listCollections().toArray(function(err, collInfos) {
    // collInfos is an array of collection info objects that look like:
    // { name: 'test', options: {} }
    // console.log(collInfos);
    res.render('index-notable',{title : 'No table chosen', list_tb: collInfos});
  });}
  else { 
    dbo.collection("Trang").find({}).toArray(function(err, result) {
    if (err) throw err;
    console.log(result);
    db.close();
  }
  });*/
});
