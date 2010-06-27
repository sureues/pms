<?php
// $Id: photos_exif.tpl.php,v 1.1.2.1 2009/03/06 08:22:41 eastcn Exp $
?>
<?php
  //print_r($exif);
?>
<?php
  if(is_array($exif)){
    print '<table>';
    foreach($exif as $key => $tag){
      if(is_array($tag)){
        print '<tr class="photos_exif_title"><td colspan="2">'.$key.'</td></tr>';
        foreach($tag as $ctag => $val){
          print '<tr><td class="photos_exif_name" width="30%">'.$ctag.'</td><td>';
          if(!is_array($val)){
            print $val;
          }else{
            print implode(',', $val);
          }
          print '</td></tr>';
        }
      }
    }
    print '</table>';
  }else{
    print '<h2>'.$exif.'</h2>';
  }
?>