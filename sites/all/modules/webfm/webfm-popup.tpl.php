<?php // $Id: webfm-popup.tpl.php,v 1.2 2009/07/21 12:35:40 robmilne Exp $ ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en" dir="ltr">
<head>
  <title><?php print t('File Browser'); ?></title>
  <?php print drupal_get_html_head(); ?>
  <?php print drupal_get_css(); ?>
  <?php print drupal_get_js('header'); ?>
</head>

<body class="webfm">
<div id="messages"><?php print theme('status_messages'); ?></div>
<?php print $content; ?>
<?php print drupal_get_js('footer'); ?>
</body>
</html>
