var express = require('express');
var router = express.Router();
var MongoClient = require('mongodb').MongoClient;
var url = "mongodb://localhost:27017/";

/* GET home page. */
router.get('/', function(req, res, next) {
  MongoClient.connect(url, function(err, db) {
	if (err) throw err;
	    var dbo = db.db("SmartDB");
	    var in_student_name = req.query.student_name;
	    var in_class_id = req.query.class_id;
	    dbo.listCollections().toArray(function(err, collInfos) {  
	    //console.log(collInfos);
	    if (in_student_name == undefined) {
	      res.render('index-notable',{list_col: collInfos});
	    }
	    else {
                console.log(in_class_id);
		dbo.collection(in_student_name).find({ class_id: { $gt: in_class_id } }).sort({_id:-1}).limit(10).toArray(function(err, result) {
		  if (err) throw err;
		  //console.log(collInfos);
		  res.render('index', { title: in_student_name , list_col: collInfos, data: result});
		  //db.close();
		});
	    }
	    }); 
	  });

});

module.exports = router;
