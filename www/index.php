<html>
<header>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>MIT Domotics - Bluetooth Control</title>
<script src="jquery-1.8.2.min.js"></script>
<script src="raphael-min.js"></script>
<script> 
function Mobile(name,mac,ip,distance)
{
	this.name=name;
	this.mac=mac;
	this.ip=ip;
	this.distance=distance;
	return this;
}



var mobileArray = new Array();
p = Raphael(0,0,1000,600);	/**Raphael papper (x,y,width,height);*/
var sensorsArray = new Array();
var sensorsPositionArray = new Array();
var emptyCounter = 0;

var selectedElement;


window.onload = function() {


	//Rectangulo da sala	
	p.rect(0,0,1000,600).attr({
		fill: "#ffe589",  
		stroke: "#4d4f54",
		"stroke-width":20
	}).click(function(e){positionSensorsChange(e);});
	
		

	//mesa do prof
	p.rect(10,200,100,200).attr({
		fill: "white",
	}).click(function(e){positionSensorsChange(e);});

		
		
	//lateral
	p.rect(180,10,180,100).attr({
		fill: "white",
	}).click(function(e){positionSensorsChange(e);});

	
	p.rect(450,10,180,100).attr({
		fill: "white",
	}).click(function(e){positionSensorsChange(e);});


	p.rect(700,10,180,100).attr({
		fill: "white",
	}).click(function(e){positionSensorsChange(e);});



	//lateral
		
	p.rect(410,450,100,140).attr({
		fill: "white",
	}).click(function(e){positionSensorsChange(e);});


	p.rect(640,450,100,140).attr({
		fill: "white",
	}).click(function(e){positionSensorsChange(e);});


	//Fundo da sala
	p.rect(980,130,10,400).attr({
		fill: "white",
	}).click(function(e){positionSensorsChange(e);});
	
	
		p.text(500,150,"MIT Room")
		.attr({
			"font-family":"Lucida Grande",
			"font-size" : 20
		});

	
	//Porta
	p.path("M900 600, c0 -50 0 -40 40 0")
		.attr({
				fill: "#4d4f54",
				stroke: "#4d4f54",
				"stroke-width":20
		});
	var periodic = setInterval(function(){updateMobiles()},5000);
	
	 updateSensors();	
	 updateTasks();
	 

	 $("#addTask").click(function(){
	 	data = new Object();
	 	data.action = $("#action option:selected").text();
	 	data.value = $("#value").val();
	 	data.domotic = $("#domoticElement").val();
	 	data.compare = $("#compare").val();
	 	data.sensor = $("#sensor option:selected").val();
	 	data.distance = $("#distance").val();
	 	data.mobile = $("#mobileList").val();
	 		 	
	 	if(data.value == ""){
		 	data.value = "0";
	 	}

		 $.post("addTask.php",data,function(result){
			 alert(result);
			 updateTasks();
		 });
	 });
	 
	 $("#domoticElement").change(function(){
	 	updateFunctions();
	 });
	 updateFunctions();
	 updateShow();
}



function updateMobiles(){
	/** Read database positions*/
	$.getJSON('getPositionList.php',function(data){
			console.log(data);

		if(data.length == 0 && emptyCounter < 10){
				emptyCounter++;
				return;
		}
	
	
						
		deleteMobiles();
		
		var mobileList = new Array();
		
		
		var mobileColorList = new Array();
		$("#mobile-distance-table").empty();
		$("#mobile-distance-table").append("<tr><th>Mobile Name</th><th>MAC</th><th>Last Distance</th><th>Sensor</th><th>RSS</th><th>Last Detection</th></tr>");
		
		$("#mobileList").empty();
		
		
		$.each(data,function(key,value){
		//mobile_mac,x,y,name,distance,rss,ts
			
			var mac = this['mobile_mac'];
			var x = parseFloat(this['x']);
			var y = parseFloat(this['y']);
			var distance = parseFloat(this['distance']);
			var rss = parseFloat(this['rss']);
			var timestamp = this['ts'];
			var sensor_ip = this['sensor_ip'];
			var sensor_id = this['sensor_id'];

			
			var name = this['name'];
			if(name == null){
				name = "no name";
			}
			
			var color; 
			if(mobileColorList[mac] == undefined){
				mobileColorList[mac] = get_random_color();
			}
			color = mobileColorList[mac];
			
			
			mobileArray.push(addMobile(sensor_ip,name,mac,x,y,distance,rss,timestamp,color,p));
			$("#mobile-distance-table").append("<tr><td><input class='mobileName' type='text' value='"+name+"'></input></td><td class='mobileMac'>"+mac+"</td><td>"+distance.toFixed(2)+"</td><td>"+sensor_id+"</td><td>"+rss+"</td><td>"+timestamp+"</td></tr>");

			$("#mobileList").append("<option value='"+mac+"'>"+name+"</option>");

			
			if(mobileList[mac] == undefined || mobileList[mac].distance > distance){
				mobileList[mac] = new Mobile(name,mac,sensor_ip,distance);
			}			
		});
		
		
			 $(".mobileName").change(function(){
				 	data = new Object();
				 	data.macAddress = $(this.parentNode.parentNode).children(".mobileMac").text();
				 	data.name = this.value;
				 		 	
					 $.post("changeMobileName.php",data,function(result){
						 alert(result);
					 });
				});
					
	});
	
}


function updateTasks(){
	
	$.getJSON('getTasks.php',function(data){
			console.log(data);
		
			$("#tasks-table").empty();
			$("#tasks-table").append("<tr><th>Domotic Element</th><th>Action Type</th><th>Value (0-100)</th><th>Compare type</th><th>Distance</th><th>Sensor</th><th>Mobile</th><th>Remove</th></tr>");
		
			$.each(data,function(key,value){
				var id = this['id'];
				var type = this['type'];
				var value = this['value'];
				var domotic_name = this['name'];
				var compare = this['compare'];
				var distance =this['distance'];
				var sensor_id = this['sensor_id'];
				var mac = this['mac'];
	
				$("#tasks-table").append("<tr><td>"+domotic_name+"</td><td>"+type+"</td><td>"+value+"</td><td>"+compare+"</td><td>"+distance+"</td><td>"+sensor_id+"</td><td>"+mac+"</td><td><button class='removeButton' value='"+id+"'>X</button></td><tr>");
			});
			
			
			$(".removeButton").click(function(){				
					data = new Object();
				 	data.task_id = this.value;
				 		 	
					 $.post("removeTask.php",data,function(result){
						 alert(result);
						 updateTasks();
					 });
				
				
			});
		});
		
	
}

function deleteMobiles(){
	for(var i=0; i < mobileArray.length; i++){
		mobileArray[i].remove();
	}
}

function positionSensorsChange(e){
			if(selectedElement != null){
				$.post("changeSensorPosition.php",{ip: selectedElement, x: e.pageX, y: e.pageY});		
				deleteSensors();		
				updateSensors();
				selectedElement = null;
			}
}
/**
	Adds a mobile representation to paper p at position x,y with name and mac
*/

function addMobile(sensor_ip,name,mac,x,y,distance,rss,timestamp,color,p){
	
	var sensor; 
		
	for(var i=0; i < sensorsPositionArray.length; i++){
		if(sensorsPositionArray[i].ip == sensor_ip)
			sensor = sensorsPositionArray[i];
	}		
	if(sensor == null){
		alert("error");
		return;
	}
	
	x = sensor.x+10;
	y = sensor.y+10;
	raio = distance * 10;
	
	var randomLocation = Math.random();
	var side;
	if(randomLocation < 0.5){
		 side = x+raio;
	}else{
		 side = x-raio;
	}
	
		
	
	var circle = p.circle(x,y,raio)
		.attr({
			stroke:color,
			opacity: 0.5,
			"stroke-width":5
		})
	
	
	mac = mac.substr(-9);
	
	var mobileMac =	p.text(side,y+20,mac)
	.attr({
		"font-family":"Lucida Grande",
		"font-size" : 0,
		fill: color
	})
	.animate({
			"font-size" : 10
		},500,"ease-in");
		
	var st = p.set();
		 st.push(circle,mobileMac);

	return st;
}


function updateFunctions(){
	
	data = new Object();
	data.device = $("#domoticElement option:selected").text();
	
	$.post('getFunctionsList.php',data,	function(data){
		console.log(data);
		$("#action").empty();
		var data = $.parseJSON(data);
		
		$.each(data,function(key,value){
			var action = value.function;
			$("#action").append("<option value='"+value.value+"'>"+action+"</option>");
		});
	});
	
	$("#action").change(function(){
		updateShow()
	});
}

function updateShow(){
	//var show = $("#action").val();
	//	if(show == 1){
	//		$("#value").show();
	//	}else{
	//		$("#value").hide();
	//	}
}




function updateSensors(){
	$.getJSON('getSensorsList.php',function(data){
		console.log(data);
		$("#sensor-table").empty();
		$("#sensor-table").append("<tr><th>Id</th><th>IP</th><th>Adjustment Factor</th></tr>");
		$("#sensor").empty();
		
		$.each(data,function(key,value){
			var x = parseFloat(this['x']);
			var y = parseFloat(this['y']);
			var name = this['sensor_id'];
			var mac = this['sensor_mac'];
			var factor = this['correction_factor'];
			
			var ip = this['sensor_ip'];
			sensorsPositionArray.push({"x":x,"y":y,"ip":ip});
			sensorsArray.push(addSensor(name,ip,x,y,p));
			
			$("#sensor-table").append("<tr><td class='sensor_id'>"+name+"</td><td>"+ip+"</td><td><input type='text' class='factor' value='"+factor+"'></input></td></tr>");
			$("#sensor").append("<option>"+name+"</option>");
		});
		
		
		
		
		//ON CHANGE
		 $(".factor").change(function(){
				 	data = new Object();
				 	data.sensor_id = $(this.parentNode.parentNode).children(".sensor_id").text();
				 	data.factor = this.value;
				 		 	
					 $.post("changeSensor.php",data,function(result){
						 alert(result);
					 });
				});
		
	});
	
}


/**
	Adds a mobile representation to paper p at position x,y with name and mac
*/
function addSensor(name,ip,x,y,p){
	var name = "Sensor "+name;
	var circle =	
		p.rect(x,y,20,20)
			.attr({
			fill:"#319300",
			stroke:"#4d4f54",
			cursor: "pointer",
		})
		.click(function(){
			this.attr({fill:"green"});
			//alert("IP: "+ip+" -> x:"+x+"   y:"+y);
			selectedElement = ip;
		})
		.hover(function(){/*IN*/
				this.attr({fill:"#48ce00"});
			},
			function() {  /** out*/
				this.attr({fill:"#4d4f54"});
		});



	var sensorName = p.text(x+8,y+28,name)
	.attr({
		"font-family":"Lucida Grande",
		"font-size" : 0
	})
	.animate({
		"font-size" : 12
	},500,"ease-in");
	
	
	
	var sensorIP =	p.text(x+8	,y+38,ip)
	.attr({
		"font-family":"Lucida Grande",
		"font-size" : 0,
	})
	.animate({
			"font-size" : 10
		},500,"ease-in");
		
	var st = p.set();
	 st.push(circle,sensorName,sensorIP);
	return st;
}


function deleteSensors(){
	for(var i=0; i < sensorsArray.length; i++){
		sensorsArray[i].remove();
	}
	
	sensorsPositionArray = new Array();

}

function get_random_color() {
    var letters = '0123456789ABCDEF'.split('');
    var color = '#';
    for (var i = 0; i < 6; i++ ) {
        color += letters[Math.round(Math.random() * 15)];
    }
    return color;
}








</script>
<style type="text/css">
body{
	background-color: #f1f1f1;
	font-family: "Lucida Grande";
}
h1,h2,h3,h4,p{
	font-family: "Lucida Grande";
}

td{
	border-style: solid;
	border-width: 1px;
}
</style>
</header>
<body>


<div style="margin-top:650px"> 
	<div  style="float:left; margin-left: 40px;">
		<div style="margin-top: 100px">
			<h1>MIT Room Access Control</h1>

			<table id="mobile-distance-table" style="border-width:4px; border-style: solid">
			</table>
		</div>
	</div>
</div>
<div style="clear:both; height:20px;"></div>	


<h2>Sensor list</h2>
<table id="sensor-table">

</table>



<h2>Add new Tasks</h2>
<table>
<tr><th>Domotic Element</th><th>Action Type</th><th>Value (0-100)</th><th>Compare type</th><th>Distance</th><th>Sensor</th><th>Mobile</th></tr>
<tr>
		<td><select id="domoticElement">
		<?php
			include_once('sqlConector.php');
	
			$sql_query = "select * from domotics;";
			$sql_result = mysql_query($sql_query,$link);
			
			while($row = mysql_fetch_assoc($sql_result)){
				echo "<option value='{$row['id']}'>{$row['name']} </option>";
			}		
		?>
		
		
		</select></td>

<td><select id="action">
			<option>set</option>
			<option>increase</option>
			<option>decrease</option>
		</select></td>
		<td><input id="value" type="text"/></td>

		<td><select id="compare">
			<option>bigger</option>
			<option>lower</option>
		</select></td>
		<td><input id="distance" type="text"/></td>
		<td><select id="sensor">
	
		</select></td>
		<td><select id="mobileList">
		</select></td>
</table>
<button id="addTask">Add Task</button>
<br>
<h2>Task List</h2>
<table id="tasks-table">
</table>
</body>
</html>
