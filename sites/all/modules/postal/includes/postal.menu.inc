<?php //$Id: postal.menu.inc,v 1.3 2009/08/19 00:19:55 vauxia Exp $

/**
 * Implementation of hook_menu().
 */
function postal_menu_menu() {
  $path = drupal_get_path('module', 'postal') .'/includes';
  return array(
    'admin/settings/postal' => array(
      'title' => 'Postal',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('postal_admin'),
      'access arguments' => array('administer site configuration'),
      'description' => 'Configure postal address settings and defaults.',
      'file' => 'postal.admin.inc',
      'file path' => $path,
    ),
    'admin/settings/postal/import/%postal_country' => array(
      'page callback' => 'postal_admin_import_states',
      'page arguments' => array(4),
      'access arguments' => array('administer site configuration'),
      'type' => MENU_CALLBACK,
      'file' => 'postal.admin.inc',
      'file path' => $path,
    ),
    'postal/js/ahah' => array(
      'page callback'    => 'postal_form_ahah',
      'access arguments' => array('access content'),
      'file'             => 'postal.form.inc',
      'file path'        => $path,
    ),
  );
}
