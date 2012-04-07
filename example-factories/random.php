<?php

  $dim_x = 256;
  $dim_y = 256;
  $dim_z = 256;
  $h = 0.1;

  for($i = 0; $i < 10000; $i++)
    $v0[] = rand(0,255)." ".rand(0,255)." ".rand(0,255);

  $out = fopen("randomField", "w");

  fwrite($out, "$dim_x $dim_y $dim_z\n");
  fwrite($out, count($v0)."\n");
  foreach($v0 as $v)
    fwrite($out, "$v\n");
  fwrite($out, "$h\n");  
  for($k = 1; $k <= $dim_z; $k++)
    for($i = 1; $i <= $dim_x; $i++)
      for($j = 1; $j <= $dim_y; $j++)
        fwrite($out, rand(-($dim_x - 1)/2, ($dim_x - 1)/2)." ".rand(-($dim_y - 1)/2, ($dim_y - 1)/2)." ".rand(-($dim_z - 1)/2, ($dim_z - 1)/2)."\n");

  fclose($out);
?>
