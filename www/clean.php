<?php
/**
*
*  	MIT Project
*  Sensor Clean DB
*
**/

include_once('sqlConector.php');


$sensorId = $_POST['sensorId'];

$sql_query = "DELETE FROM sensorData where sensor_id = $sensorId;";

$sql_result = mysql_query($sql_query,$link);

if(!$sql_result){
	echo mysql_error();
};

mysql_close($link);
echo "cleaned";

?>
