<?php
/**
*
*  	MIT Project
*  Registry Sensor in Database
*
**/


$sensorId = $_POST['sensorId'];
$x = $_POST['x'];
$y = $_POST['y'];
$macAddress = $_POST['macAddress'];

include_once('sqlConector.php');


$sql_query = "INSERT into sensors(sensor_id,x,y,sensor_mac) values ($sensorId,$x,$y,'$macAddress');";

$sql_result = mysql_query($sql_query,$link);

if(!$sql_result){
	die("Error: "+mysql_error());
}else{
	echo "Success";
}

?>
