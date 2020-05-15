// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/createGEDCOMs',   //The server endpoint we are connecting to
        success: function (data) {
			//parse the files in the uploads file and add them to the fileview table
            let table = document.getElementById("fileLogTable");

			
			for(let x of data)
			{
				let row = table.insertRow(-1);
				let cell1 = row.insertCell(0);
				let cell2 = row.insertCell(1);
				let cell3 = row.insertCell(2);
				let cell4 = row.insertCell(3);
				let cell5 = row.insertCell(4);
				let cell6 = row.insertCell(5);
				let cell7 = row.insertCell(6);
				let cell8 = row.insertCell(7);
			
				let array = JSON.parse(x); 
				console.log(array);
				
				let link = document.createElement("a");
				link.setAttribute("href", ""+array.filename);
				link.text = array.filename.replace("uploads/","");
				
				cell1.append(link);
				cell2.innerHTML = array.source; 
				cell3.innerHTML = array.gedcversion;
				cell4.innerHTML = array.encoding;
				cell5.innerHTML = array.submitterName;
				cell6.innerHTML = array.submitterAddress;
				cell7.innerHTML = array.numInd;
				cell8.innerHTML = array.numFam;
				
				//adding things to dropdowns
				let opt1 = document.getElementById("mySelect1");
				let opt2 = document.getElementById("mySelect2");
				let opt3 = document.getElementById("mySelect3");
				let opt4 = document.getElementById("mySelect4");
				let option1 = document.createElement("option");
				option1.text = array.filename;
				option1.value = array.filename;
				let option2 = document.createElement("option");
				option2.text = array.filename;
				option2.value = array.filename;
				let option3 = document.createElement("option");
				option3.text = array.filename;
				option3.value = array.filename;
				let option4 = document.createElement("option");
				option4.text = array.filename;
				option4.value = array.filename;
				opt1.add(option1);
				opt2.add(option2);
				opt3.add(option3);
				opt4.add(option4);
			
			}
			if(document.getElementById("fileLogTable").rows.length>1)
			{
				document.getElementById("storefiles_btn").disabled = false; 
			}
			
        },
        fail: function(error) {
            // Non-200 return, do something with error 
            console.log(error); 
        },
    });
    
    
	let objDiv =document.getElementById("status_a");
	objDiv.scrollTop = objDiv.scrollHeight;
	
    // Event listener form replacement example, building a Single-Page-App, no redirects if possible
    $('#form_upload').submit(function(e){ 
		e.preventDefault();
		if(document.getElementById("upload").value.split('.').pop() == "ged")
		{
			
			
			$.ajax({
				url: '/upload',
				type: 'POST', 
				data: new FormData(document.getElementById('form_upload')),
				cache: false,
				contentType: false,
				processData: false,
				success: function (data) {
					$('#status_a').append("Uploaded " + document.getElementById("upload").value + "<br />");
					let objDiv =document.getElementById("status_a");
					objDiv.scrollTop = objDiv.scrollHeight; 
					document.getElementById("storefiles_btn").disabled = false;
				},
				fail: function(error) { 
					$('#status_a').append("Failed to upload " + document.getElementById("upload").value + "<br />");
					let objDiv =document.getElementById("status_a");
					objDiv.scrollTop = objDiv.scrollHeight;
				}
			});	
			
			$.ajax({
				url: '/createAGedcom',
				type: 'get', 
				dataType: 'json', 
				data: {filename:document.getElementById("upload").value},
				success: function (data) {
					let table = document.getElementById("fileLogTable");

					
					for(let x of data)
					{
						let row = table.insertRow(-1);
						let cell1 = row.insertCell(0);
						let cell2 = row.insertCell(1);
						let cell3 = row.insertCell(2);
						let cell4 = row.insertCell(3);
						let cell5 = row.insertCell(4);
						let cell6 = row.insertCell(5);
						let cell7 = row.insertCell(6);
						let cell8 = row.insertCell(7);
					
						let array = JSON.parse(x); 
						console.log(array);
						
						let link = document.createElement("a");
						link.setAttribute("href", ""+array.filename);
						link.text = array.filename.replace("uploads/","");
						
						cell1.append(link);
						cell2.innerHTML = array.source; 
						cell3.innerHTML = array.gedcversion;
						cell4.innerHTML = array.encoding;
						cell5.innerHTML = array.submitterName;
						cell6.innerHTML = array.submitterAddress;
						cell7.innerHTML = array.numInd;
						cell8.innerHTML = array.numFam;
						
						//adding things to dropdowns
						let opt1 = document.getElementById("mySelect1");
						let opt2 = document.getElementById("mySelect2");
						let opt3 = document.getElementById("mySelect3");
						let opt4 = document.getElementById("mySelect4");
						let option1 = document.createElement("option");
						option1.text = array.filename;
						option1.value = array.filename;
						let option2 = document.createElement("option");
						option2.text = array.filename;
						option2.value = array.filename;
						let option3 = document.createElement("option");
						option3.text = array.filename;
						option3.value = array.filename;
						let option4 = document.createElement("option");
						option4.text = array.filename;
						option4.value = array.filename;
						opt1.add(option1);
						opt2.add(option2);
						opt3.add(option3);
						opt4.add(option4);
					
					}
				},
				fail: function(error) { 
					console.log(error); 
				}
			});	
			
			
			
			
			/*//adding things to file viewer
			let table = document.getElementById("fileLogTable");
			let row = table.insertRow(-1);
			let cell1 = row.insertCell(0);
			let cell2 = row.insertCell(1);
			let cell3 = row.insertCell(2);
			let cell4 = row.insertCell(3);
			let cell5 = row.insertCell(4);
			let cell6 = row.insertCell(5);
			let cell7 = row.insertCell(6);
			let cell8 = row.insertCell(7);
			
			let link = document.createElement("a");
			link.setAttribute("href", "/uploads/"+document.getElementById("upload").value);
			link.text = document.getElementById("upload").value;
			
			cell1.append(link);
			cell2.innerHTML = "this";
			cell3.innerHTML = "would";
			cell4.innerHTML = "crash";
			cell5.innerHTML = "from";
			cell6.innerHTML = "my";
			cell7.innerHTML = "createGEDCOM";
			cell8.innerHTML = "function";*/
			
			//adding things to dropdowns
			let opt1 = document.getElementById("mySelect1");
			let opt2 = document.getElementById("mySelect2");
			let opt3 = document.getElementById("mySelect3");
			let opt4 = document.getElementById("mySelect4");
			let option1 = document.createElement("option");
			option1.text = document.getElementById("upload").value;
			option1.value = document.getElementById("upload").value;
			let option2 = document.createElement("option");
			option2.text = document.getElementById("upload").value;
			option2.value = document.getElementById("upload").value;
			let option3 = document.createElement("option");
			option3.text = document.getElementById("upload").value;
			option3.value = document.getElementById("upload").value;
			let option4 = document.createElement("option");
			option4.text = document.getElementById("upload").value;
			option4.value = document.getElementById("upload").value;
			opt1.add(option1);
			opt2.add(option2);
			opt3.add(option3);
			opt4.add(option4);
		}
    });
    
    //uploading a file
    document.getElementById("form_upload").style.display="none";
	$('#upload_btn').click(function(e){
		e.preventDefault();
		let x = document.getElementById("form_upload");
		if (x.style.display === "none") {
			x.style.display = "block";
		} else {
			x.style.display = "none";
		}
	});
    //$('#upload').css('visibility','hidden');
	
	$('#upload_btn_sub').submit(function(e) {
		e.preventDefault();
		$("#form_upload").trigger("submit");
		
	});
	
	//create simple gedcom 
	document.getElementById("create_gedcom").style.display="none";
	$('#crt_ged_btn').click(function(e){
		e.preventDefault();
		let x = document.getElementById("create_gedcom");
		if (x.style.display === "none") {
			x.style.display = "block";
		} else {
			x.style.display = "none";
		}
	});
	
	//validating the createsimpleGedcom form
	document.getElementById("filename-form").value="";
	document.getElementById("source-form").value="PAF";
	document.getElementById("version").value="5.5";
	document.getElementById("submitter-name").value="";
	document.getElementById("submitter-address").value="";
	$('#createGedSubmit').click(function(e) {
		e.preventDefault();
		let valid = true;
		if(document.getElementById("filename-form").value === "")
		{
			alert("please enter file name");
			valid = false;
		}
		else if(document.getElementById("source-form").value === "")
		{
			alert("please enter source");
			valid = false;
		}
		else if(document.getElementById("version").value === "")
		{
			alert("please enter version number");
			valid = false;
		}
		else if(document.getElementById("submitter-name").value === "")
		{
			alert("please enter submitter name");
			valid = false;
		}
		else if(document.getElementById("filename-form").value.split('.').pop() != "ged")
		{
			alert("please enter correct file extention");
			valid = false;
		}
		
		if(valid == true)
		{
			$('#status_a').append("Created " + document.getElementById("filename-form").value + "<br />");
			let objDiv =document.getElementById("status_a");
			objDiv.scrollTop = objDiv.scrollHeight;
			//document.getElementById("create_gedcom").submit();
			
			//adding things to file viewer
			let table = document.getElementById("fileLogTable");
			let row = table.insertRow(-1);
			let cell1 = row.insertCell(0);
			let cell2 = row.insertCell(1);
			let cell3 = row.insertCell(2);
			let cell4 = row.insertCell(3);
			let cell5 = row.insertCell(4);
			let cell6 = row.insertCell(5);
			let cell7 = row.insertCell(6);
			let cell8 = row.insertCell(7);
			
			let link = document.createElement("a");
			link.setAttribute("href", "/uploads/"+document.getElementById("filename-form").value);
			link.text = document.getElementById("filename-form").value;
			
			cell1.append(link);
			cell2.innerHTML = document.getElementById("source-form").value;
			cell3.innerHTML = document.getElementById("version").value;
			cell4.innerHTML = document.getElementById("encodingSelect").value;
			cell5.innerHTML = document.getElementById("submitter-name").value;
			cell6.innerHTML = document.getElementById("submitter-address").value;
			cell7.innerHTML = "0";
			cell8.innerHTML = "0";
			
			//adding things to dropdowns
			let opt1 = document.getElementById("mySelect1");
			let opt2 = document.getElementById("mySelect2");
			let opt3 = document.getElementById("mySelect3");
			let opt4 = document.getElementById("mySelect4");
			let option1 = document.createElement("option");
			option1.text = "uploads/"+document.getElementById("filename-form").value;
			option1.value = "uploads/"+document.getElementById("filename-form").value;
			let option2 = document.createElement("option");
			option2.text = "uploads/"+document.getElementById("filename-form").value;
			option2.value = "uploads/"+document.getElementById("filename-form").value;
			let option3 = document.createElement("option");
			option3.text = "uploads/"+document.getElementById("filename-form").value;
			option3.value = "uploads/"+document.getElementById("filename-form").value;
			let option4 = document.createElement("option");
			option4.text = "uploads/"+document.getElementById("filename-form").value;
			option4.value = "uploads/"+document.getElementById("filename-form").value;
			opt1.add(option1);
			opt2.add(option2);
			opt3.add(option3);
			opt4.add(option4);
			
			
			//making it into an object to call c function
			let myObj = new Object();
			//myObj.fileName = document.getElementById("filename-form").value;
			myObj.subName = document.getElementById("submitter-name").value;
			myObj.subAddress = document.getElementById("submitter-address").value;
			myObj.source = document.getElementById("source-form").value;
			myObj.gedcVersion = document.getElementById("version").value;
			myObj.encoding = document.getElementById("encodingSelect").value;
			
			let objJSON = JSON.stringify(myObj);
			console.log(objJSON);
			$.ajax({
				type: 'get',            //Request type
				dataType: 'json',       //Data type - we will use JSON for almost everything 
				data: {json:objJSON,filename:document.getElementById("filename-form").value},
				url: '/createsimple',   //The server endpoint we are connecting to
				success:function (data) {
					console.log("success");
					document.getElementById("storefiles_btn").disabled = false;
				},
				fail: function(error) {
					console.log("fail");
				},
			});
		}	
		
	});
	
	//adding an individual
	document.getElementById("create_indi").style.display="none";
	$('#crt_indi_btn').click(function(e){
		e.preventDefault();
		let x = document.getElementById("create_indi");
		if (x.style.display === "none") {
			x.style.display = "block";
		} else {
			x.style.display = "none";
		}
	});
	
	$('#createIndiSubmit').click(function(e){
		e.preventDefault();
		let myObj = new Object();
		myObj.givenName = document.getElementById("indi_givenname").value;
		myObj.surname = document.getElementById("indi_surname").value;
		let objJSON = JSON.stringify(myObj);
		$.ajax({
				type: 'get',            //Request type
				dataType: 'json',       //Data type - we will use JSON for almost everything 
				data: {json:objJSON,filename:document.getElementById("mySelect1").value},
				url: '/addindi',   //The server endpoint we are connecting to
				success:function (data) {
					$('#status_a').append("Added individual to " + document.getElementById("mySelect1").value + "<br />");
					let objDiv =document.getElementById("status_a");
					objDiv.scrollTop = objDiv.scrollHeight; 
				},
				fail: function(error) {
					$('#status_a').append("Failed to add individual to " + document.getElementById("mySelect1").value + "<br />");
					let objDiv =document.getElementById("status_a");
					objDiv.scrollTop = objDiv.scrollHeight; 
				},
		});
	});
	
	//getting descendants
	document.getElementById("get_des").style.display="none";
	document.getElementById("desTableDiv").style.display="none";
	$('#get_des_btn').click(function(e){
		e.preventDefault();
		let x = document.getElementById("get_des");
		if (x.style.display === "none") {
			x.style.display = "block";
		} else {
			x.style.display = "none";
			document.getElementById("desTableDiv").style.display="none";
		}
	});
	
	$('#getDesSubmit').click(function(e){
		e.preventDefault();
		document.getElementById("desTableDiv").style.display="block";
	});
	
	
	//getting ancestors
	document.getElementById("get_anc").style.display="none";
	document.getElementById("ancTableDiv").style.display="none";
	$('#get_anc_btn').click(function(e){
		e.preventDefault();
		let x = document.getElementById("get_anc");
		if (x.style.display === "none") {
			x.style.display = "block";
		} else {
			x.style.display = "none";
			document.getElementById("ancTableDiv").style.display="none";
		}
	});
	
	$('#getAncSubmit').click(function(e){
		e.preventDefault();
		document.getElementById("ancTableDiv").style.display="block";
	});
	
	
	//clear status bar
	$('#clearStatus').click(function(e){
		e.preventDefault();
		$('#status_a').html("");
	});
	
	$('#popup').modal('show');
	$('#dblogin').click(function(e){
		e.preventDefault();
		
		console.log(document.getElementById("uname").value);
		$.ajax({
			type: 'get',            //Request type
			dataType: 'json', 
			data: {usrname:document.getElementById("uname").value,pass:document.getElementById("psw").value,dbase:document.getElementById("db").value},
			url: '/dbconnect', 
			success:function (data) {
				console.log(data);
				if(data.code == "ER_ACCESS_DENIED_ERROR")
				{
					document.getElementById("errormsg").innerHTML = "ERROR LOGGING IN";
					$('#popup').modal('show');
				}
				else
				{
					document.getElementById("errormsg").innerHTML = "FATAL ERROR CONNECTING TO DB";
					$('#popup').modal('show');
				}
			},
			fail:function (data) {
				console.log("db connectioni failed");
				
			},
		});
		$('#popup').modal('hide');
		
	});
	
	$('#storefiles_btn').click(function(e){
		e.preventDefault();
		document.getElementById("storefiles_btn").disabled = false; 
		document.getElementById("cleardata_btn").disabled = false; 
		document.getElementById("dbstatus_btn").disabled = false; 
		document.getElementById("query_btn").disabled = false; 
		var array = new Array();
		var table = document.getElementById('fileLogTable');
		for (var r = 1, n = table.rows.length; r < n; r++) {
			//console.log(table.rows[r]);
			var obj = new Object();
			obj.fileName = table.rows[r].cells[0].innerText;
			obj.source = table.rows[r].cells[1].innerHTML;
			obj.version = table.rows[r].cells[2].innerHTML;
			obj.encoding = table.rows[r].cells[3].innerHTML;
			obj.subName = table.rows[r].cells[4].innerHTML;
			obj.subAddr = table.rows[r].cells[5].innerHTML;
			obj.numInd = table.rows[r].cells[6].innerHTML;
			obj.numFam = table.rows[r].cells[7].innerHTML;
			
			array.push(obj);
		}
		$.ajax({
			type: 'get',            //Request type
			dataType: 'json', 
			data: {obj:JSON.stringify(array)},
			url: '/dbfiletable', 
			success:function (data) {
				console.log(data);
				if(data.code == "ER_TABLE_EXISTS_ERROR")
				{
					
				}
			},
			fail:function (data) {
				//$('#dblogin').trigger('click');
			},
		});
		
		$.ajax({
			type: 'get',            //Request type
			dataType: 'json', 
			url: '/dbindividual', 
			success:function (data) {
				
			},
			fail:function (data) {
				//$('#dblogin').trigger('click');
			},
		});
		$.ajax({
			type: 'get',            //Request type
			dataType: 'json', 
			url: '/countdb', 
			success:function (data) {
				console.log(data);
				$('#status_a').append("Database has  " + data[0] + " files and "+data[1]+" individuals<br />");
				let objDiv =document.getElementById("status_a");
				objDiv.scrollTop = objDiv.scrollHeight;
			},
			fail:function (data) {
				
			},
		});
	});
	$('#cleardata_btn').click(function(e){
		e.preventDefault();
		$.ajax({
			type: 'get',            //Request type
			dataType: 'json', 
			url: '/cleardb', 
			success:function (data) {
			},
			fail:function (data) {
				//$('#dblogin').trigger('click');
			},
		});
		$.ajax({
			type: 'get',            //Request type
			dataType: 'json', 
			url: '/countdb', 
			success:function (data) {
				console.log(data);
				$('#status_a').append("Database has  " + data[0] + " files and "+data[1]+" individuals<br />");
				let objDiv =document.getElementById("status_a");
				objDiv.scrollTop = objDiv.scrollHeight;
			},
			fail:function (data) {
				
			},
		});
	});
	$('#dbstatus_btn').click(function(e){
		e.preventDefault();
		$.ajax({
			type: 'get',            //Request type
			dataType: 'json', 
			url: '/countdb', 
			success:function (data) {
				console.log(data);
				$('#status_a').append("Database has  " + data[0] + " files and "+data[1]+" individuals<br />");
				let objDiv =document.getElementById("status_a");
				objDiv.scrollTop = objDiv.scrollHeight;
			},
			fail:function (data) {
				
			},
		});
	});
	
	document.getElementById("query_form").style.display="none";
	$('#query_btn').click(function(e){
		e.preventDefault();
		let x = document.getElementById("query_form");
		if (x.style.display === "none") {
			x.style.display = "block";
			window.scrollTo(0,document.body.scrollHeight);
		} else {
			x.style.display = "none";
		}
	});
	
	$('#querySearch').click(function(e){
		e.preventDefault();
		
		$.ajax({
			type: 'get',            //Request type
			dataType: 'json', 
			data: {query:document.getElementById("queryStatement").value},
			url: '/customdb', 
			success:function (data) {
				console.log(data);
				$('#queryresult').append("RESULT:  " + JSON.stringify(data)+"<br />");
				let objDiv =document.getElementById("queryresult");
				objDiv.scrollTop = objDiv.scrollHeight;
			},
			fail:function (data) {
				
			},
		});
	});
	$('#clearquery').click(function(e){
		e.preventDefault();
		$('#queryresult').html("");
	});
	
	$('#querySubmit').click(function(e){
		e.preventDefault();
		if(document.getElementById("query1").checked == true)
		{
			$.ajax({
				type: 'get',            //Request type
				dataType: 'json', 
				url: '/query1', 
				success:function (data) {
					$('#queryresult').html("");
					let table = document.getElementById("queryresult");
					//console.log(data);
				
					for(let x of data)
					{
						let row = table.insertRow(-1);
						let cell1 = row.insertCell(0);
						let cell2 = row.insertCell(1);
						let cell3 = row.insertCell(2);
						let cell4 = row.insertCell(3);
					
						
						cell1.innerHTML = x.surname;
						cell2.innerHTML = x.given_name; 
						cell3.innerHTML = x.sex;
						cell4.innerHTML = x.fam_size;
						
					}
					
					
					let rowH = table.insertRow(0);
					let cell1H = rowH.insertCell(0);
					let cell2H = rowH.insertCell(1);
					let cell3H = rowH.insertCell(2);
					let cell4H = rowH.insertCell(3);
					
					cell1H.innerHTML = "<strong>Surname</strong>";
					cell2H.innerHTML = "<strong>Given Name</strong>"; 
					cell3H.innerHTML = "<strong>sex</strong>";
					cell4H.innerHTML = "<strong>fam size</strong>";
					//let objDiv =document.getElementById("queryresult");
					//objDiv.scrollTop = objDiv.scrollHeight;
				},
				fail:function (data) {
					
				},
			});
		}
		else if(document.getElementById("query2").checked == true)
		{
			$.ajax({
				type: 'get',            //Request type
				dataType: 'json',
				data: {query:document.getElementById("query2input").value}, 
				url: '/query2', 
				success:function (data) {
					$('#queryresult').html("");
					let table = document.getElementById("queryresult");
					//console.log(data);
				
					for(let x of data)
					{
						let row = table.insertRow(-1);
						let cell1 = row.insertCell(0);
						let cell2 = row.insertCell(1);
					
						
						cell1.innerHTML = x.surname;
						cell2.innerHTML = x.given_name; 
						
					}
					
					
					let rowH = table.insertRow(0);
					let cell1H = rowH.insertCell(0);
					let cell2H = rowH.insertCell(1);
					
					cell1H.innerHTML = "<strong>Surname</strong>";
					cell2H.innerHTML = "<strong>Given Name</strong>";
				},
				fail:function (data) {
					
				},
			});
		}
		else if(document.getElementById("query3").checked == true)
		{
			$.ajax({
				type: 'get',            //Request type
				dataType: 'json',
				data: {query1:document.getElementById("query3input1").value,query2:document.getElementById("query3input2").value}, 
				url: '/query3', 
				success:function (data) {
					$('#queryresult').html("");
					let table = document.getElementById("queryresult");
					//console.log(data);
				
					for(let x of data)
					{
						let row = table.insertRow(-1);
						let cell1 = row.insertCell(0);
					
						
						cell1.innerHTML = x.file_name;
						
					}
					
					
					let rowH = table.insertRow(0);
					let cell1H = rowH.insertCell(0);
					
					cell1H.innerHTML = "<strong>File Name</strong>";
				},
				fail:function (data) {
					
				},
			});
		}
		else if(document.getElementById("query4").checked == true)
		{
			$.ajax({
				type: 'get',            //Request type
				dataType: 'json',
				data: {query:document.getElementById("query4input").value}, 
				url: '/query4', 
				success:function (data) {
					$('#queryresult').html("");
					let table = document.getElementById("queryresult");
					//console.log(data);
				
					for(let x of data)
					{
						let row = table.insertRow(-1);
						let cell1 = row.insertCell(0);
						let cell2 = row.insertCell(1);
					
						
						cell1.innerHTML = x.surname;
						cell2.innerHTML = x.given_name; 
						
					}
					
					
					let rowH = table.insertRow(0);
					let cell1H = rowH.insertCell(0);
					let cell2H = rowH.insertCell(1);
					
					cell1H.innerHTML = "<strong>Surname</strong>";
					cell2H.innerHTML = "<strong>Given Name</strong>";
				},
				fail:function (data) {
					
				},
			});
		}
		else if(document.getElementById("query5").checked == true)
		{
			$.ajax({
				type: 'get',            //Request type
				dataType: 'json',
				data: {query:document.getElementById("query5input").value}, 
				url: '/query5', 
				success:function (data) {
					$('#queryresult').html("");
					let table = document.getElementById("queryresult");
					//console.log(data);
				
					for(let x of data)
					{
						let row = table.insertRow(-1);
						let cell1 = row.insertCell(0);
						let cell2 = row.insertCell(1);
					
						
						cell1.innerHTML = x.surname;
						cell2.innerHTML = x.given_name; 
						
					}
					
					
					let rowH = table.insertRow(0);
					let cell1H = rowH.insertCell(0);
					let cell2H = rowH.insertCell(1);
					
					cell1H.innerHTML = "<strong>Surname</strong>";
					cell2H.innerHTML = "<strong>Given Name</strong>";
				},
				fail:function (data) {
					
				},
			});
		}
		
	});
	
	$('#helpbtn').click(function(e){
		e.preventDefault();
		document.getElementById("helpdiv").style.display="block";
		$.ajax({
			type: 'get',            //Request type
			dataType: 'json',
			data: {query:document.getElementById("query4input").value}, 
			url: '/queryhelpfile', 
			success:function (data) {
				
				let table = document.getElementById("filetab");
				console.log(data);
			
				for(let x of data)
				{
					let row = table.insertRow(-1);
					let cell1 = row.insertCell(0);
					let cell2 = row.insertCell(1);
					let cell3 = row.insertCell(2);
					let cell4 = row.insertCell(3);
					let cell5 = row.insertCell(4);
					let cell6 = row.insertCell(5);
				
					
					cell1.innerHTML = x.Field;
					cell2.innerHTML = x.Type; 
					cell3.innerHTML = x.Null;
					cell4.innerHTML = x.Key;
					cell5.innerHTML = x.Default;
					cell6.innerHTML = x.Extra;
				}
				

			},
			fail:function (data) {
				
			},
		});
		$.ajax({
			type: 'get',            //Request type
			dataType: 'json',
			data: {query:document.getElementById("query4input").value}, 
			url: '/queryhelpind', 
			success:function (data) {
				
				let table = document.getElementById("indtab");
				console.log(data);
			
				for(let x of data)
				{
					let row = table.insertRow(-1);
					let cell1 = row.insertCell(0);
					let cell2 = row.insertCell(1);
					let cell3 = row.insertCell(2);
					let cell4 = row.insertCell(3);
					let cell5 = row.insertCell(4);
					let cell6 = row.insertCell(5);
				
					
					cell1.innerHTML = x.Field;
					cell2.innerHTML = x.Type; 
					cell3.innerHTML = x.Null;
					cell4.innerHTML = x.Key;
					cell5.innerHTML = x.Default;
					cell6.innerHTML = x.Extra;
				}
				

			},
			fail:function (data) {
				
			},
		});
		$('#helpbtn').off();
	});
	document.getElementById("helpdiv").style.display="none";
	
	
	
});
