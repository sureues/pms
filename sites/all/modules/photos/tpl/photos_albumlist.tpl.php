<?php
// $Id: photos_albumlist.tpl.php,v 1.1.2.1 2009/03/06 08:22:41 eastcn Exp $
?>
<div class="photos-album">
	<?php if($node->album['url']){
		print '<div class="album-left"><img src="'.$node->album['url'].'"></div><div class="album-right">';
	}?>
	<h2><?php print $node->titlelink;?></h2>
	<div class="photos-meta"><?php print theme('username', $node);?><span class="photos-time"><?php print t('Published in ').$node->time;?></span><span class="photos-imgnum"></span><?php print t('A total of %num images',array('%num' => $node->album['count']));?></div>
	<?php print $node->teaser;?>
	<?php if($node->album['url']){ print '</div>';}?>
	<div class="photos-editmenu"><?php print $node->albumlink;?></div>
</div>