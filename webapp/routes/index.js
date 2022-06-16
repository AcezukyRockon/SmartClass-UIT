var express = require('express');
var router = express.Router();
var MongoClient = require('mongodb').MongoClient;
var url = "mongodb://localhost:27017/";

/* GET home page. */
router.get('/', function(req, res, next) {
  MongoClient.connect(url, function(err, db) {
    if (err) throw err;
    var dbo = db.db("SmartDB");
    dbo.collection("Trang").find({}).toArray(function(err, result) {
      if (err) throw err;
      console.log(result);
      db.close();
      res.render('index', { title: 'Express' , data: result});
    });
  });
  
});

module.exports = router;
