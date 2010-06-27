<?php
// $Id: photos_albumview.tpl.php,v 1.1.2.2 2009/04/05 05:13:01 eastcn Exp $
?>
<?php
  //print_r($album);
  //print_r($node);
?>
<?php if($album['album_url'] || $album['node_url']) : ?>
<div class="photos_album_menu">
  <div class="photos_album_float_right">
    <a href="<?php print $album['album_url'];?>/slide"><?php print t('Slideshow');?></a>
    (<a href="<?php print $album['album_url'];?>/slide" target="_blank"><?php print t('Open a new window');?></a>)
  </div>
  <?php if($album['access']['edit']) : ?>
    <a href="<?php print $album['node_url'];?>/edit"><?php print t('Album settings');?></a>
    <a href="<?php print $album['node_url'];?>/photos"><?php print t('Images Management');?></a>
  <?php endif; ?>
</div>
<?php endif; ?>
<?php if($node->type == 'photos') : ?>
  <div class="photos_album_header">
    <div class="photos_album_cover">
      <img src="<?php print $node->album['cover']['url'];?>" title="<?php print $node->title;?>"/>
    </div>
    <h2><?php print $node->title;?></h2>
    <?php print $node->teaser;?>
    <div class="photos_album_right">
      <?php print t('!cou images', array('!cou' => $node->album['count']));?>
      <?php print theme_node_submitted($node);?>
    </div>
  </div>
<?php endif; ?>

<?php print $album['links'];?>
<?php print $album['pager'];?>
<?php 
  foreach($album['view'] as $view){
    print $view;
  }
?>
<?php print $album['pager'];?>