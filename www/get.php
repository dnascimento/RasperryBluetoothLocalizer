<?php
/**
*
*  	MIT Project
*  Get mobile position
*
**/
include_once('sqlConector.php');


$sql_query = "select function,name from domotics_functions natural join domotics order by name;";

$sql_result = mysql_query($sql_query,$link);

if(!$sql_result){
	die(mysql_error());
}


$outArray = [];
while($row = mysql_fetch_assoc($sql_result)){
	echo $row['name'];
	echo $row['function'];
	echo ", ";
}

echo json_encode($outArray);

?>
