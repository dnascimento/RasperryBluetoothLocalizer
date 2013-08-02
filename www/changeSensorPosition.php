<?php
/**
*
*  	MIT Project
*  Sensor Receive HTTP Post from Sensors
*
**/

$ip = $_POST['ip'];
$x = $_POST['x'];
$y = $_POST['y']; 

include_once('sqlConector.php');

echo "ada";
$sql_query = "UPDATE sensors SET x=$x, y=$y where sensor_ip='$ip';";

echo $sql_query;

$sql_result = mysql_query($sql_query,$link);

if(!$sql_result){
	echo mysql_error();
};



mysql_close($link);

?>
