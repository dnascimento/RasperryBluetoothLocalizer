<?php
/**
*
*  	MIT Project
*  Sensor Receive HTTP Post from Sensors
*
**/

$rss = $_POST['rss'];
$sensorId = $_POST['sensorId'];
$macAddress = $_POST['macAddress'];


include_once('sqlConector.php');


$sql_query = "insert into mobiles(name,mac) values('new_mobile','$macAddress');";
$sql_result = mysql_query($sql_query,$link);

//Calcular a distância
$sql_query = "select correction_factor from sensors where sensor_id = '$sensorId';";
$sql_result = mysql_query($sql_query,$link);


while($row = mysql_fetch_assoc($sql_result)){
	$correction_factor = $row['correction_factor'];
}


	$distance =  pow(10, ($rss + 57.37) / (-23.87))*$correction_factor;


$sql_query = "insert into sensorData(sensor_id,rss,mobile_mac,distance) values($sensorId,$rss,'$macAddress','$distance') ON DUPLICATE KEY UPDATE rss=$rss, ts=CURRENT_TIMESTAMP;";

//echo $sql_query;

$sql_result = mysql_query($sql_query,$link);

if(!$sql_result){
	echo mysql_error();
};



//ler a task_list associada a este mac e sensor
$sql_query = "select task_list.type, task_list.compare,task_list.distance, task_list.value, domotics.name, domotics.code  from task_list,domotics where mac='$macAddress' and sensor_id = '$sensorId' and domotics.id = task_list.domotic;";


$sql_result = mysql_query($sql_query,$link);

//Ver o compare e verificar se é verdade
while($row = mysql_fetch_assoc($sql_result)){

	if(($row['compare'] == "bigger" && $distance >= $row['distance']) || ($row['compare'] == "lower" && $distance < $row['distance'])){
	

		$execString = "{$row['code']} {$row['value']}";	
		
		
		$execString = "java -jar Domotics.jar set {$execString}";
		
 		echo $execString;
		
exec($execString);
	}
}




mysql_close($link);

?>
