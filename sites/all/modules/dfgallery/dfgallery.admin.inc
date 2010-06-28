<?php
// $Id: dfgallery.admin.inc,v 1.1 2009/03/06 08:14:13 eastcn Exp $

//hook_menu
function dfgallery_admin_settings(){
	$form['dfgallery_skin'] = array(
		'#type' => 'radios',
		'#title' => t('Default skin'),
		'#default_value' => variable_get('dfgallery_skin', 'vista'),
    '#required' => TRUE,
		'#options' => array('vista' => t('vista'), 'mac' => t('mac'), 'standard' => t('standard')),
	);
  return system_settings_form($form);
}