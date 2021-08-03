<?php

include 'test2.php';
comp_inject_start(
	"echo '/*# ' . __FILE__ . ':0 */' . PHP_EOL;", 
	"echo '/*# ' . __FILE__ . ':$ */' . PHP_EOL;"
);
include 'test2.php';
comp_inject_stop();
include 'test2.php';
