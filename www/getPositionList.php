<?php
/**
*
*  	MIT Project
*  Get mobile position
*
**/
include_once('sqlConector.php');


$sql_query = "select sensor_id,sensor_ip,mobile_mac,x,y,name,distance,rss,ts from sensorData natural join mobiles natural join sensors where mobile_mac = mac order by mobile_mac, distance;";

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
