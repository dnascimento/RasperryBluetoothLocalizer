<?php
/**
*
*  	MIT Project
*  Add a task to Database
*
**/


include_once('sqlConector.php');



$sql_query = "INSERT into task_list(type,value,domotic,compare,distance,sensor_id,mac) values ('{$_POST['action']}','{$_POST['value']}','{$_POST['domotic']}','{$_POST['compare']}','{$_POST['distance']}','{$_POST['sensor']}','{$_POST['mobile']}');";


echo $sql_query;


$sql_result = mysql_query($sql_query,$link);

echo $sql_result;
if(!$sql_result){
	die("Erro" + mysql_error());
}
else{
	echo "Task Added";
}

?>
