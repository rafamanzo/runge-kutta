<?php

  $dim_x = 128;
  $dim_y = 128;
  $dim_z = 20;
  $h = 0.02;
  $v0_count = 1;
  $v0 = "64.0 64.0 10.0";  

  $out = fopen("rotationField", "w");

  fwrite($out, "$dim_x $dim_y $dim_z\n");
  fwrite($out, "$v0_count\n");
  fwrite($out, "$v0\n");
  fwrite($out, "$h\n");  
  for($k = 1; $k <= $dim_z; $k++)
    for($i = 1; $i <= $dim_x; $i++)
      for($j = 1; $j <= $dim_y; $j++)
        fwrite($out, ($j/$i)." ".(-$i/$j)." 0\n");

  fclose($out);
?>
