<?php
// $Id: photos_imageview.tpl.php,v 1.1.2.1 2009/03/06 08:22:41 eastcn Exp $
?>
<?php
  //print_r($image);
?>
<?php
  //ajax edit or delete
  if($image['ajax']['edit_url']){
    $name_class = ' jQueryeditable_edit_filename';
    $des_class = ' jQueryeditable_edit_des';
    $filename_edit = 'ajaxedit="'.$image['ajax']['edit_url'].'/filename"';
    $des_edit = 'ajaxedit="'.$image['ajax']['edit_url'].'/des"';
  }
  //Modify the code above could lead to Ajax Failure.
  //修改以上代码可能导致ajax编辑功能失效。
?>
<?php if($type == 'list') : ?>
  <div class="photos_image_list_view" <?php print $image['ajax']['del_id'];?>>
    <div class="photos_list_view_thumb">
      <a href="<?php print $image['url'] ?>#image-load"><?php print $image['view']; ?></a>
    </div>
    <h2 class="photos_list_view_filename<?php print $name_class ?>" <?php print $filename_edit ?>>
      <?php print $image['filename'];?>
    </h2>
    <div class="photos_list_view_des<?php print $des_class ?>" <?php print $des_edit ?>><?php print $image['des']; ?></div>
    <div class="photos_list_view_info">
      <?php print $image['ajax']['del_link'];?>
      <?php print $image['links']['vote']; ?>
      <?php print $image['links']['info']; ?>
      <?php print $image['links']['count']; ?>
      <?php print $image['links']['cover']; ?>
      <?php print $image['links']['comment']; ?>
    </div>
  </div>
<?php endif; ?>
<?php if($type == 'view') : ?>
  <div id="image-load" class="image-load">
    <h2 class="photos_view_filename<?php print $name_class ?>" <?php print $filename_edit ?>>
      <?php print $image['filename'];?>
    </h2>
  	<div class="photos_view_des<?php print $des_class ?>" <?php print $des_edit ?>>
  		<?php print $image['des'];?>
    </div>
    <div class="photos_view_links">
      <?php print $image['vote']; ?>
  		<div class="photo-rg">
        <?php print $image['ajax']['del_link'];?>
        <?php print $image['links']['to_sub'];?>
  			<?php if($image['count']) { print t('!cou visits',array('!cou' => $image['count'])); } ?>
  			<?php
  				print $image['links']['comment'];
          print $image['links']['cover'];
  				print $image['links']['more'];
  			?>
  		</div>
  	</div>
  	<div class="photo_link_pager">
      <?php if($image['links']['pager']['prev_url']) { ?>
        <div class="photo-pager-left">
          <a href="<?php print $image['links']['pager']['prev_url'];?>#image-load">« <?php print t('previou');?></a>
        </div>
      <?php } ?>
      <?php if($image['links']['pager']['next_url']) { ?>
        <div class="photo-pager-right">
          <a href="<?php print $image['links']['pager']['next_url'];?>#image-load"><?php print t('next');?> »</a>
        </div>
      <?php } ?>
    </div>
  	<div class="image-a"><?php print $image['view'];?></div>
  </div>
  <?php print $image['comment']['view'];?>
  <?php print $image['comment']['box'];?>
<?php endif; ?>