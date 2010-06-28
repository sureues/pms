<?php
// $Id: dfgallery_flash.tpl.php,v 1.1 2009/03/06 08:14:13 eastcn Exp $
?>
<object classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=9,0,0,0" width="<?php print $flash['width'];?>" height="<?php print $flash['height'];?>">
	<param name="allowFullScreen" value="true" />
	<param name="movie" value="<?php print $flash['swf'];?>" />
	<param name="quality" value="high" />
	<param name="FlashVars" value="xmlUrl=<?php print $flash['url'];?>" />
	<embed src="<?php print $flash['swf'];?>" quality="high"width="<?php print $flash['width'];?>" height="<?php print $flash['height'];?>" FlashVars="xmlUrl=<?php print $flash['url'];?>" allowFullScreen="true" type="application/x-shockwave-flash" pluginspage="http://www.macromedia.com/go/getflashplayer" />
</object>