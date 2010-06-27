<?php
// $Id: photos_imageblock.tpl.php,v 1.1.2.1 2009/03/06 08:22:41 eastcn Exp $
?>
<?php print t('Uploaded on !time by !name', array('!name' => theme('username', (object)$image), '!time' => format_date($image['timestamp'], 'small')));?>
<table cellspacing="0" cellpadding="0" border="0" class="photos_block_pager">
  <tr>
    <td class="photos_block_pager_open">&nbsp;</td>
    <td colspan="3">
      <div class="photos_block_pager_title">
        <?php if($image['slide_url']) :?>
          <a href="<?php print $image['slide_url'];?>" class="photos_block_pager_flash" title="<?php print t('View as slideshow');?>">&nbsp;</a>
        <?php endif; ?>
        <h3 class="photos_block_pager_name"><?php print l(t('@name\'s images', array('@name' => $image['name'])), 'photos/user/'.$image['uid'].'/image');?></h3>
      </div>
    </td>
  </tr>
  <tr class="photos_prev">
    <td rowspan="2" class="photos_block_pager_left">&nbsp;</td>
    <td valign="top" class="photos_block_pager_center">
      <?php if($image['pager']['prev_view']): ?>
        <a href="<?php print $image['pager']['prev_url'];?>#image-load"><?php print $image['pager']['prev_view'];?></a>
        <div><a href="<?php print $image['pager']['prev_url'];?>#image-load"><?php print t('previou');?></a></div>
      <?php endif;?>
    </td>
    <td valign="top" class="photos_block_pager_center">
      <?php print $image['pager']['current_view'];?>
      <div>▲</div>
    </td>
    <td valign="top" class="photos_block_pager_center">
      <?php if($image['pager']['next_view']): ?>
        <a href="<?php print $image['pager']['next_url'];?>#image-load"><?php print $image['pager']['next_view'];?></a>
        <div><a href="<?php print $image['pager']['next_url'];?>#image-load"><?php print t('next');?></a></div>
      <?php endif;?>
    </td>
  </tr>
  <tr class="photos-block-link">
    <td colspan="3">
      <?php print t('By !album', array('!album' => l($image['title'], 'photos/album/'.$image['nid']))); ?>
    </td>
  </tr>
</table>
<?php print $image['links'];?>
<?php if($image['sub_album']): ?>
<p class="photos_block_info">
  <?php print t('This photo also belongs to:');?>
</p>
  <?php foreach($image['sub_album'] as $sub_album){ ?>
    <div class="photos_block_sub">
      <div class="photos_block_sub_title">
        <div class="photos_block_sub_open" alt="<?php print $sub_album['geturl']; ?>">&nbsp;</div>
        <div class="photos_block_sub_name">
          <a href="<?php print $sub_album['url'];?>" title="<?php print $sub_album['info']; ?>"><?php print $sub_album['title'];?></a>
        </div>
      </div>
      <div class="photos_block_sub_body photos_block_sub_body_load">
      </div>
    </div>
  <?php } ?>
<?php endif;?>