<?php
/**
*
*  	MIT Project
*  Get mobile position
*
**/
include_once('sqlConector.php');


$sql_query = "select task_list.id,task_list.type,value,name,compare,distance,sensor_id,mac from task_list, domotics where domotics.id = domotic;";



				
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
