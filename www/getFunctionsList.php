<?php
/**
*
*  	MIT Project
*  Get mobile position
*
**/
include_once('sqlConector.php');

$device = $_POST['device'];


$sql_query = "select function,value from domotics_functions natural join domotics where name ='$device';";


$sql_result = mysql_query($sql_query,$link);

if(!$sql_result){
	die(mysql_error());
}

$outArray = [];


while($row = mysql_fetch_assoc($sql_result)){
	array_push($outArray,$row);
}

echo json_encode($outArray);

?>
