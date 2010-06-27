<?php
// $Id: photos_share.tpl.php,v 1.1.2.1 2009/03/06 08:22:41 eastcn Exp $
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="<?php print $GLOBALS['language']->language; ?>" xml:lang="<?php print $GLOBALS['language']->language; ?>">
<head>
  <title><?php print drupal_get_title();?></title>
  <?php print drupal_get_html_head(); ?>
  <?php print drupal_get_css(); ?>
  <?php print drupal_get_js('header'); ?>
</head>
<body>
<div id="photos_share">
  <div class="photos_share_links">
    <?php print $images['links'];?>
  </div>
  <div><?php print $images['menu'];?></div>
  <p>
    <?php print t('Made selected images sharing code: ');?><a href="#" title="<?php print t('Can only be used at node');?>" class="photos_share_copy" alt="ubb">UBB code</a> | <a href="#" title="<?php print t('Can be used at other sites');?>" class="photos_share_copy" alt="html">HTML code</a>
  </p>
  <div class="photos_share_textarea">
    <h2><?php print t('Please copy the code:');?></h2>
    <div><textarea id="photos_share_textarea" rows="6" cols="100"></textarea></div>
  </div>
  <h2><?php print t('Please select Image Size:');?></h2>
  <ul id="photos_share_ul">
  <?php if(is_array($images['data'])) { foreach ($images['data'] as $image){ ?>
    <li alt="<?php print $image['link'];?>" title="<?php print $image['filename'];?>">
      <?php print $image['view']; ?>
      <div class="photos_share_select"><?php print t('Sizes:');?><select size="1" class="photos_share_select_val" alt="<?php print $image['fid'];?>">
      <option value="0">None</option>
      <?php if(is_array($image['thumbs'])): foreach($image['thumbs'] as $key => $thumb){ ?>
          <option value="<?php print $thumb;?>"><?php print $key;?></option>
      <?php } endif; ?>
      </select></div>
      <?php if($image['username']) : ?>
        <div><?php print $image['username'];?></div>
      <?php endif; ?>
    </li>
  <?php 
      }
    }else{ 
      print t('No image'); 
    } 
  ?>
  </ul>
</div>
<?php print drupal_get_js('footer'); ?>
</body>
</html>