<?php

  $dim_x = 128;
  $dim_y = 128;
  $dim_z = 20;
  $h = 0.1;
  $v0[] = "0.0 16.0 10.0";
  $v0[] = "0.0 32.0 10.0";
  $v0[] = "0.0 48.0 10.0";
  $v0[] = "0.0 64.0 10.0";
  $v0[] = "0.0 80.0 10.0";
  $v0[] = "0.0 96.0 10.0";
  $v0[] = "0.0 112.0 10.0";

  $out = fopen("rotationField", "w");

  fwrite($out, "$dim_x $dim_y $dim_z\n");
  fwrite($out, count($v0)."\n");
  foreach($v0 as $v)
    fwrite($out, "$v\n");
  fwrite($out, "$h\n");  
  for($k = 1; $k <= $dim_z; $k++)
    for($i = 1; $i <= $dim_x; $i++)
      for($j = 1; $j <= $dim_y; $j++)
        fwrite($out, ($j/$i)." ".(-$i/$j)." 0\n");

  fclose($out);
?>
