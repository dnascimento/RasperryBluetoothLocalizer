<?php
/**
*
*  	MIT Project
*  Connect do MySQL Server
*
**/

$serverIP = "127.0.0.1";

$link = mysql_connect($serverIP,'root','mit');
if(!$link){
	die(mysql_error());
}else{
//	echo 'Connected sucessfully';
}	

mysql_select_db("mit",$link);
?>
