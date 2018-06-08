<?php
$output = shell_exec('subsetbanddepth subsetdata subsetdata.json $_GET["tsv"]');
echo $output;
?>
