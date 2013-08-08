<?php
/**
*
*  	MIT Project
*  Registry Sensor in Database
*
**/


$sensor_id = $_POST['sensor_id'];
$factor = $_POST['factor'];

include_once('sqlConector.php');

$sql_query = "update sensors set sensor_id='$sensor_id', correction_factor='$factor' where sensor_id='$sensor_id';";

echo $sql_query;

$sql_result = mysql_query($sql_query,$link);

if(!$sql_result){
	die("Erro" + mysql_error());
}
else{
	echo "Changed with success";
}

?>
