var express = require('express');
var router = express.Router();
var MongoClient = require('mongodb').MongoClient;
var url = "mongodb://localhost:27017/";

/* GET home page. */
router.get('/', function(req, res, next) {
  MongoClient.connect(url, function(err, db) {
    if (err) throw err;
    var dbo = db.db("SmartDB");
    var tb = req.query.title;
    dbo.listCollections().toArray(function(err, collInfos) {  
    //console.log(collInfos);
    if (tb == undefined) {
      //console.log("nothing to retrieve");
      res.render('index-notable',{title : 'No student chosen', list_tb: collInfos});
    }
    else {
        dbo.collection(tb).find().sort({_id:-1}).limit(10).toArray(function(err, result) {
          if (err) throw err;
          //console.log(collInfos);
          res.render('index', { title: tb , list_tb: collInfos, data: result});
          //db.close();
        });
    }
    }); 
  
  });

});

module.exports = router;
