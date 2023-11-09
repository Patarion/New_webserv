<!DOCTYPE html>
<html>
<head>
    <title>400 - Bad Request</title>
</head>
<body>
    <h1>400 - Bad Request</h1>
    <p>Your request appears to be malformed or invalid.</p>
    <p>TODAY:<?php 
	date_default_timezone_set('America/New_York');
	echo date("Y-m-d H:i:s"); ?></p>
    <p>Return to the <a href="/">home page</a>.</p>
</body>
</html>
