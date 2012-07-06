<?php
  
  //the sum of random numbers has normal distribution
  function gaussian_random(){
    return ((rand(-1000, 1000) + rand(-1000, 1000) + rand(-1000, 1000) + rand(-1000, 1000) + rand(-1000, 1000) + rand(-1000, 1000) + rand(-1000, 1000) + rand(-1000, 1000) + rand(-1000, 1000) + rand(-1000, 1000))/1000.0);
  }

  $dim_x = 256;
  $dim_y = 256;
  $dim_z = 256;
  $h = 0.1;

  for($i = 0; $i < 1; $i++)
    $v0[] = rand(0,255)." ".rand(0,255)." ".rand(0,255);

  $out = fopen("gaussianField", "w");

  fwrite($out, "$dim_x $dim_y $dim_z\n");
  fwrite($out, count($v0)."\n");
  foreach($v0 as $v)
    fwrite($out, "$v\n");
  fwrite($out, "$h\n");  
  for($k = 1; $k <= $dim_z; $k++)
    for($i = 1; $i <= $dim_x; $i++)
      for($j = 1; $j <= $dim_y; $j++)
        fwrite($out, gaussian_random()." ".gaussian_random()." ".gaussian_random()."\n");

  fclose($out);
?>
