<?php
  $dim_x = $dim_y = 512;
  $dim_z = 128;
  $h=0.2;
  
  $v0[] = "0.0 64.0 64.0";
  
  $out = fopen("linesField", "w");
  
  fwrite($out, "$dim_x $dim_y $dim_z\n");
  fwrite($out, count($v0)."\n");
  foreach($v0 as $v)
    fwrite($out, "$v\n");
  fwrite($out, "$h\n");
  
  
  for($k = 1; $k <= $dim_z; $k++){
    $signal = -1;
    for($i = 1; $i <= $dim_x; $i++){
      if(($i % 8) == 0){ $signal *= -1;}
      for($j = 1; $j <= $dim_y; $j++)
        if($signal == -1){
          fwrite($out, "1 -1 0\n");
        }else{
          fwrite($out, "1 1 0\n");
        }
    }
  }
?>
