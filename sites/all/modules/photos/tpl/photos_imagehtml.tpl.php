<?php
// $Id: photos_imagehtml.tpl.php,v 1.1.2.2 2009/03/11 02:38:31 eastcn Exp $
?>
<?php 
  $filename = strip_tags($image['filename']);
  $alt = $image['alt'] ? strip_tags($image['alt']) : $filename;
?>
<?php if(!$image['href']){ ?>
<img src="<?php print _photos_l($imagepath); ?>" alt="<?php print $alt ?>" title="<?php print $filename ?>"/>
<?php }else{ ?>
  <?php if($image['thickbox']) : ?>
    <div class="photos_imagehtml">
  <?php endif; ?>
    <a href="<?php print url($image['href']); ?>"><img src="<?php print _photos_l($imagepath); ?>" alt="<?php print $alt ?>" title="<?php print $filename ?>"/></a>
  <?php if($image['thickbox']) : ?>
      <a class="photos_imagehtml_thickbox thickbox" rel="thickbox_<?php print $image['pid']; ?>" href="<?php print _photos_l($image['thickbox']); ?>" title="<?php print $filename; ?>"></a>
    </div>
  <?php endif; ?>
<?php } ?>