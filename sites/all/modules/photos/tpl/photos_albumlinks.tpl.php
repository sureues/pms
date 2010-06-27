<?php
// $Id: photos_albumlinks.tpl.php,v 1.1.2.1 2009/03/06 08:22:41 eastcn Exp $
?>
<div class="photos_album_links">
  <?php if($links['link'] || $links['count']) : ?>
    <div class="photos_link_info">
      <?php if($links['count']) print t('!cou images', array('!cou' => $links['count'])); ?>
      <?php print $links['link'];?>
    </div>
  <?php endif; ?>
  <div class="photos_link_sort">
    <span><?php print t('Sort by:'); ?></span>
    <?php print $links['sort']; ?>
  </div>
  <?php if($links['limit']) : ?>
  <div class="photos_link_limit">
    <span><?php print t('Limit by:'); ?></span>
    <?php print $links['limit']; ?>
  </div>
  <?php endif; ?>
</div>