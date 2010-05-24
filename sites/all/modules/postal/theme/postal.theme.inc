<?php // $Id: postal.theme.inc,v 1.1 2009/08/16 18:59:56 vauxia Exp $

/**
 * Implementation of hook_theme().
 */
function postal_theme_theme() {
  $path = drupal_get_path('module', 'postal') .'/theme';
  return array(
    'postal' => array(
      'arguments' => array('element' => NULL),
      'file' => 'postal.theme.inc',
      'path' => $path,
    ),
    'postal_general' => array(
      'arguments' => array('element' => NULL),
      'file' => 'postal.theme.inc',
      'path' => $path,
    ),
    'postal_address' => array(
      'arguments' => array('address' => NULL),
      'file' => 'postal.theme.inc',
      'path' => $path,
    ),
    'postal_country' => array(
      'arguments' => array('address' => NULL),
      'file' => 'postal.theme.inc',
      'path' => $path,
    ),
    'postal_city' => array(
      'arguments' => array('address' => NULL),
      'file' => 'postal.theme.inc',
      'path' => $path,
    ),
    'postal_state' => array(
      'arguments' => array('address' => NULL),
      'file' => 'postal.theme.inc',
      'path' => $path,
    ),
    'postal_zip' => array(
      'arguments' => array('address' => NULL),
      'file' => 'postal.theme.inc',
      'path' => $path,
    ),
  );
}

function theme_postal($element) {
  drupal_add_css(drupal_get_path('module', 'postal') . '/postal.css', all);
  return theme('form_element', $element, $element['#children']);
}

function theme_postal_general($element) {
  return theme('form_element', $element, $element['#children']);
}

function theme_postal_address($address) {
  $address = postal_set_names(postal_address($address));
  $out = '';
  if ($address->name)        $out .= $address->name;
  if ($address->street1)     $out .= '<br />'. $address->street1;
  if ($address->street2)     $out .= '<br />'. $address->street2;
  if ($out && ($address->city || $address->state || $address->zip)) $out .= '<br />';
  if ($address->city)        $out .= $address->city .', ';
  if ($address->state)       $out .= $address->state .' ';
  if ($address->zip)         $out .= $address->zip;
  if ($address->country)     $out .= '<br />'. $address->country;

  return $out;
}

function theme_postal_country($address) {
  $address = postal_set_names(postal_address($address));
  return $address->country;
}

function theme_postal_state($address) {
  $address = postal_set_names(postal_address($address));
  return $address->state;
}

function theme_postal_city($address) {
  $address = postal_address($address);
  return $address->city;
}

function theme_postal_zip($address) {
  $address = postal_address($address);
  return $address->zip;
}
