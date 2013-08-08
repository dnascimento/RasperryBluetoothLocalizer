<?php
/**
*
*  	MIT Project
*  Registry Sensor in Database
*
**/


$taskID = $_POST['task_id'];

include_once('sqlConector.php');

$sql_query = "delete from task_list where id='$taskID';";

$sql_result = mysql_query($sql_query,$link);

if(!$sql_result){
	die("Erro" + mysql_error());
}
else{
	echo "Deleted with success";
}

?>
