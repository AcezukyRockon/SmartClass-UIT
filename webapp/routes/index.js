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
	    var in_lecturer_id = req.query.lecturer_id;
	    dbo.listCollections().toArray(function(err, collInfos) {  
	    //console.log(collInfos);
	    if (in_student_name == undefined || in_student_name == "") {
	      res.render('index-notable',{list_col: collInfos});
	    }
	    else { 
		// class and lecturer are null		
		if (in_class_id == "" && in_lecturer_id == "") {
		dbo.collection(in_student_name).find().sort({_id:-1}).limit(50).toArray(function(err, result) {
			  if (err) throw err;
			  //console.log(result);
			  res.render('index', { title_student_name: in_student_name , title_class_id: in_class_id , title_lecturer_id: in_lecturer_id, list_col: collInfos, data: result});
			  //db.close();

			});
                 }
		// class null only
		else if (in_class_id == "") {
		dbo.collection(in_student_name).find({lecturer_id: in_lecturer_id}).sort({_id:-1}).limit(50).toArray(function(err, result) {
			  if (err) throw err;
			  //console.log(result);
			  res.render('index', { title_student_name: in_student_name , title_class_id: in_class_id , title_lecturer_id: in_lecturer_id, list_col: collInfos, data: result});
			  //db.close();
			});
                 }

		// lecturer null only
		else if (in_lecturer_id == "") {
		dbo.collection(in_student_name).find({class_id:  in_class_id}).sort({_id:-1}).limit(50).toArray(function(err, result) {
			  if (err) throw err;
			  //console.log(result);
			  res.render('index', { title_student_name: in_student_name , title_class_id: in_class_id , title_lecturer_id: in_lecturer_id, list_col: collInfos, data: result});
			  //db.close();
			});
                 }
		else { // both are not null
			dbo.collection(in_student_name).find({ class_id:  in_class_id, lecturer_id: in_lecturer_id }).sort({_id:-1}).limit(50).toArray(function(err, result) {
			  if (err) throw err;
			  //console.log(result);
			  res.render('index', { title_student_name: in_student_name , title_class_id: in_class_id , title_lecturer_id: in_lecturer_id, list_col: collInfos, data: result});
			  //db.close();
			});
		}
	    }
	    }); 
	//db.close();
	  });	
});

module.exports = router;
