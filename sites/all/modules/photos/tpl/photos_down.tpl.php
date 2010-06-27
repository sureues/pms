<?php
// $Id: photos_down.tpl.php,v 1.1.2.2 2009/04/05 05:13:01 eastcn Exp $
?>
<?php if($type == 'print') : ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $GLOBALS['language']->language; ?>" xml:lang="<?php print $GLOBALS['language']->language; ?>">
<head>
  <title><?php print drupal_get_title();?></title>
  <?php print drupal_get_html_head(); ?>
  <?php print drupal_get_css(); ?>
  <?php print drupal_get_js('header'); ?>
</head>
<body>
<?php endif; ?>
<div class="photos_download_bucket">
  <div class="photos_album_float_right">
    <?php print $content['info']; ?>
  </div>
  <?php print $content['back']; ?>
</div>
<div class="photos_download_menu">
  <div class="photos_download_menu_t">
    <?php print $content['link']['sizes'];?>
    <?php print $content['link']['exif'];?>
    <?php print $content['link']['vote'];?>
  </div>
  <?php if($content['sizes']) : ?>
    <div class="photos_download_menu_b"><ul>
      <?php
        foreach($content['sizes'] as $size){
          print '<li>'.$size['link'].'</li>';
        }
      ?>
      <?php if($content['link']['original']) print '<li>'.$content['link']['original'].'</li>';?>
    </ul></div>
  <?php endif; ?>
</div>
 
<?php if($content['sharing_url']) : ?>
<div id="photos-image-sharing">
<input type="text" class="image-quote-link" value='<a title="<?php print $content['sharing_title'];?>" href="<?php print $content['sharing_link'];?>"><img src="<?php print $content['sharing_url'];?>"></a>'/>
<input type="text" class="image-quote-link" value="<?php print $content['sharing_url'];?>"/>
</div>
<?php endif; ?>
 
<div class="photos_download_view">
  <?php print $content['view']; ?>
</div>
<?php if($type == 'print') : ?>
<?php print drupal_get_js('footer'); ?>
</body>
</html>
<?php endif; ?>