<?php
// $Id: photos_default.tpl.php,v 1.1.2.1 2009/03/06 08:22:41 eastcn Exp $
?>
<div id="photos_default">
  <?php if($content['user']['image']) : ?>
    <h2><?php print t('My latest images');?></h2>
    <?php print $content['user']['image']; ?>
  <?php endif; ?>
  <?php if($content['user']['album']) : ?>
    <h2><?php print t('My latest albums');?></h2>
    <?php print $content['user']['album']; ?>
  <?php endif; ?>
  <?php if($content['site']['image']) : ?>
    <h2><?php print t('Latest images');?></h2>
    <?php print $content['site']['image']; ?>
  <?php endif; ?>
  <?php if($content['site']['album']) : ?>
    <h2><?php print t('Latest albums');?></h2>
    <?php print $content['site']['album']; ?>
  <?php endif; ?>
</div>