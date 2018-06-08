<?php
$output = shell_exec('tsv2banddepth $_GET["tsv"] data.json');
echo $output;
?>
