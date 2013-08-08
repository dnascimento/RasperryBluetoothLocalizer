<?php
/**
*
*  	MIT Project
*  Registry Sensor in Database
*
**/


$name = $_POST['name'];
$macAddress = $_POST['macAddress'];

include_once('sqlConector.php');

$sql_query = "update mobiles set name='$name', mac='$macAddress' where mac='$macAddress';";


$sql_result = mysql_query($sql_query,$link);

if(!$sql_result){
	die("Erro" + mysql_error());
}
else{
	echo "Changed with success";
}

?>
