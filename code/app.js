'use strict'

// C library API
const ffi = require('ffi');
const mysql = require('mysql');

let sharedLib = ffi.Library('./sharedLib', { 
  'createGEDCOMwrapper': [ 'string', ['string']],		//return type first, argument list second
									//for void input type, leave argumrnt list empty
  'simpleGEDwrapper': [ 'void', ['string','string'] ],	//int return, int argument
  'addIndiwrapper': [ 'void', ['string','string'] ],
  'dbIndiWrapper': [ 'string', ['string']],
  //'getDesc' : [ 'string', [] ],
  //'putDesc' : [ 'void', [ 'string' ] ],
});
// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    console.log(err);
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

//Sample endpoint
app.get('/someendpoint', function(req , res){
  res.send({
    foo: "bar"
  });
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);



app.get('/createGEDCOMs', function(req , res){
	/*let files = new Array();
	files = Folder
	
	let error = sharedLib.createGEDCOM('uploads/' + req.params.name, GEDCOMobjectPtrPtr );
	res.send({error,GEDCOMobject});*/
	const testFolder = path.join(__dirname+'/uploads/');
	let array = [];
	fs.readdir(testFolder, (err, files) => {
	  files.forEach(file => {
		 let error = sharedLib.createGEDCOMwrapper('uploads/' + file); 
		array.push(error);
	  });
	  res.json(array);
	}) 
});

//Respond to GET requests for files in the uploads/ directory
app.get('/createAGedcom', function(req , res){
    let array = [];
    console.log(req.query.filename);
    let error = sharedLib.createGEDCOMwrapper('uploads/' +req.query.filename);
    console.log(error);
    array.push(error);
    res.json(array);
});

app.get('/createsimple', function(req, res) {
	//console.log(req.query.json);
	sharedLib.simpleGEDwrapper(req.query.json,'uploads/' +req.query.filename);
});

app.get('/addindi', function(req, res) {
	sharedLib.addIndiwrapper(req.query.json,req.query.filename);
});

let connection = new Object;
app.get('/dbconnect', function(req, res) {
	console.log(req.query.pass);
	connection = mysql.createConnection({
		host     : 'dursley.socs.uoguelph.ca',
		user     : req.query.usrname,
		password : req.query.pass,
		database : req.query.dbase
	});
	connection.connect(function (err, rows, fields) {
		if (err) {
			console.log("Something went wrong. "+err);
			res.send(err);
		}
	});
	/*connection.query("create table FILE (file_id int auto_increment,  file_name varchar(60) not null,  source varchar(250) not null, version varchar(10) not null, encoding varchar(10) not null,sub_name varchar(62) not null, sub_addr varchar(256), num_individuals int , num_families int , primary key(file_id) )", function (err, rows, fields) {
		if (err) console.log("Something went wrong. "+err);
		res.send(err);
	});*/
	//connection.end();
	//res.send(err);
});

app.get('/dbfiletable', function(req, res) {
	
	//console.log(objects);
	connection.query("create table FILE (file_id int auto_increment,  file_name varchar(60) not null,  source varchar(250) not null, version varchar(10) not null, encoding varchar(10) not null,sub_name varchar(62) not null, sub_addr varchar(256), num_individuals int , num_families int , primary key(file_id) )", function (err, rows, fields) {
		if (err) {
			console.log("Something went wrong. "+err);
			//res.send(err);
		}
	});
	connection.query("create table INDIVIDUAL (ind_id int auto_increment,  surname varchar(256) not null,  given_name varchar(256) not null, sex varchar(1), fam_size int,source_file int, primary key(ind_id), foreign key (source_file) references FILE (file_id) ON DELETE CASCADE )", function (err, rows, fields) {
		if (err) {
			console.log("Something went wrong. "+err);
			//res.send(err);
		}
	});
	
	let objects = JSON.parse(req.query.obj);
	for(let x of objects)
	{
		connection.query("INSERT INTO FILE VALUES (null,'"+x.fileName+"','"+x.source+"','"+x.version+"','"+x.encoding+"','"+x.subName+"','"+x.subAddr+"','"+x.numInd+"','"+x.numFam+"')", function (err, rows, fields) {
		if (err) {
			console.log("Something went wrong. "+err);
			//res.send(err);
		}
	});
	}
	//connection.end();
});

app.get('/dbindividual', function(req, res) {
	const testFolder = path.join(__dirname+'/uploads/');
	let array = [];
	fs.readdir(testFolder, (err, files) => {
	  files.forEach(file => {
		let error = sharedLib.dbIndiWrapper('uploads/' + file); 
		//console.log(error);
		let obj = JSON.parse(error);
		for(let x of obj)
		{
			let newString = x.filename;
			//console.log(x);
			if(x.filename.includes("uploads/")==true)
			{
				newString = x.filename.replace('uploads/','');
			}
			connection.query("INSERT INTO INDIVIDUAL(surname,given_name,sex,fam_size,source_file) VALUES ('"+x.surname+"','"+x.givenName+"',null,'0',(SELECT file_id FROM FILE WHERE file_name ='"+newString+"'))", function (err, rows, fields) {
			if (err) {
				console.log("Something went wrong. "+err);
				//res.send(err);
			}
			});
			
		} 
		array.push(error);
	  });
	  //console.log(array);
	  //let arrJSON = JSON.stringify(array);
	  //console.log(arrJSON);
	}) 
	//console.log(array);
});


app.get('/cleardb', function(req, res) {
	connection.query("DELETE FROM FILE;", function (err, rows, fields) {
	if (err) {
		console.log("Something went wrong. "+err);
		//res.send(err);
	}
	});
});

app.get('/countdb', function(req, res) {
	let array = [];
	connection.query("SELECT * FROM FILE;", function (err, rows, fields) {
		if (err) {
			console.log("Something went wrong. "+err);
			//res.send(err);
		}
		let fc = rows.length;
		array.push(fc);
	});
	connection.query("SELECT * FROM INDIVIDUAL;", function (err, rows, fields) {
		if (err) {
			console.log("Something went wrong. "+err);
			//res.send(err);
		}
		array.push(rows.length);
		res.json(array);
	});
	
	
});

app.get('/customdb', function(req, res) {
	//console.log(req.query.query);
	connection.query("SELECT "+req.query.query, function (err, rows, fields) {
		if (err) {
			console.log("Something went wrong. "+err);
			//res.send(err);
		}
		else 
		{
			console.log(rows);
			res.json(rows);
		}
	});
});

app.get('/query1', function(req, res) {
	//console.log(req.query.query);
	connection.query("SELECT surname, given_name,sex,fam_size  FROM INDIVIDUAL ORDER BY surname", function (err, rows, fields) {
		if (err) {
			console.log("Something went wrong. "+err);
			//res.send(err);
		}
		else 
		{
			console.log(rows);
			res.json(rows);
		}
	});
});

app.get('/query2', function(req, res) {
	//console.log(req.query.query);
	connection.query("SELECT surname, given_name FROM INDIVIDUAL WHERE source_file IN (SELECT file_id FROM FILE WHERE file_name='"+req.query.query+"');", function (err, rows, fields) {
		if (err) {
			console.log("Something went wrong. "+err);
			//res.send(err);
		}
		else 
		{
			console.log(rows);
			res.json(rows);
		}
	});
});

app.get('/query3', function(req, res) {
	//console.log(req.query.query);
	connection.query("SELECT file_name FROM FILE WHERE file_id IN (SELECT source_file FROM INDIVIDUAL WHERE given_name='"+req.query.query1+"' AND surname='"+req.query.query2+"');", function (err, rows, fields) {
		if (err) {
			console.log("Something went wrong. "+err);
			//res.send(err);
		}
		else 
		{
			console.log(rows);
			res.json(rows);
		}
	});
});

app.get('/query4', function(req, res) {
	//console.log(req.query.query);
	connection.query("SELECT surname, given_name FROM INDIVIDUAL WHERE surname LIKE '%"+req.query.query+"%' OR given_name LIKE '%"+req.query.query+"%';", function (err, rows, fields) {
		if (err) {
			console.log("Something went wrong. "+err);
			//res.send(err);
		}
		else 
		{
			console.log(rows);
			res.json(rows);
		}
	});
});

app.get('/query5', function(req, res) {
	//console.log(req.query.query);
	connection.query("SELECT surname, given_name FROM INDIVIDUAL WHERE source_file IN (SELECT file_id FROM FILE WHERE sub_name='"+req.query.query+"');", function (err, rows, fields) {
		if (err) {
			console.log("Something went wrong. "+err);
			//res.send(err);
		}
		else 
		{
			console.log(rows);
			res.json(rows);
		}
	});
});


app.get('/queryhelpfile', function(req, res) {
	//console.log(req.query.query);
	connection.query("DESCRIBE FILE;", function (err, rows, fields) {
		if (err) {
			console.log("Something went wrong. "+err);
			//res.send(err);
		}
		else 
		{
			//console.log(rows);
			res.json(rows);
		}
	});
});

app.get('/queryhelpind', function(req, res) {
	//console.log(req.query.query);
	connection.query("DESCRIBE INDIVIDUAL;", function (err, rows, fields) {
		if (err) {
			console.log("Something went wrong. "+err);
			//res.send(err);
		}
		else 
		{
			//console.log(rows);
			res.json(rows);
		}
	});
});
