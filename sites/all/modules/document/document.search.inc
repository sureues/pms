<?php

// $Id:


require_once (drupal_get_path('module', 'document') . '/document.inc');

function document_search_form(&$form_state) {
  drupal_add_css(drupal_get_path('module', 'document') . '/document.css');

  drupal_add_js(drupal_get_path('module', 'document') . '/document.js');
  document_register_status();

  $form = array();

  $form['document_search_fieldset'] = array(
      '#type' => 'fieldset',
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#title' => t('Search Documents'),
      '#attributes' => array(
          'class' => 'document_search_fieldset'));

  $form['document_search_fieldset']['lbl_search_for'] = array(
      '#type' => 'item',
      '#title' => 'Search For',
      '#prefix' => '<div class="search-row-1"><div class="lbl-search-for">',
      '#suffix' => '</div>');

  $form['document_search_fieldset']['search_text'] = array(
      '#type' => 'textfield',
      '#attributes' => array(
          'onkeypress' => 'return(doc.suppressSearchEnter(event));'),
      '#prefix' => '<div class="search-text">',
      '#suffix' => '</div>');

  $form['document_search_fieldset']['search_button'] = array(
      '#type' => 'button',
      '#prefix' => '<div class="search-button form-item">',
      '#value' => 'Search',
      '#attributes' => array(
          'onclick' => 'return(doc.performSearch());'),
      '#suffix' => '</div><img src="' . document_image_url('loading.gif') . '" id="document_loading" style="display:none" /></div>');

  $form['document_search_fieldset']['lbl_search_in'] = array(
      '#type' => 'item',
      '#title' => 'Search In',
      '#prefix' => '<div class="search-row-2"><div class="lbl-search-in">',
      '#suffix' => '</div>');

  $form['document_search_fieldset']['search_fields'] = array(
      '#type' => 'radios',
      '#options' => array(
          t('Author |'),
          t('Keywords |'),
          t('Both'),
          t('None')),
      '#default_value' => 3,
      '#prefix' => '<div class="search-fields" style="float: left;">',
      '#suffix' => '</div></div>');

  $form['document_search_fieldset']['lbl_search_year'] = array(
      '#type' => 'item',
      '#title' => 'Year of Publication',
      '#prefix' => '<div class="search-row-3"><div class="lbl-search-year">',
      '#suffix' => '</div>');

  $years = drupal_map_assoc(document_get_years());
  $years[''] = '--Select Year--';
  $years = array_reverse($years, TRUE);

  $form['document_search_fieldset']['search_year'] = array(
      '#type' => 'select',
      '#options' => $years,
      '#required' => TRUE,
      '#prefix' => '<div class="search-year">',
      '#suffix' => '</div>');

  $form['document_search_fieldset']['lbl_search_doctype'] = array(
      '#type' => 'item',
      '#title' => 'Document Type',
      '#prefix' => '<div class="lbl-search-doctype">',
      '#suffix' => '</div>');

  $types = document_get_types(FALSE);
  $types[''] = '--Select Type--';

  $form['document_search_fieldset']['search_doctype'] = array(
      '#type' => 'select',
      '#options' => $types,
      '#required' => TRUE,
      '#prefix' => '<div class="search-doctype">',
      '#suffix' => '</div></div>');

  $sql = sprintf('SELECT * FROM {node} n INNER JOIN {document} d ON n.vid = d.vid WHERE n.status = %d', DOCUMENT_STATUS_PUBLISHED);

  $form['document_results'] = array(
      '#type' => 'markup',
      '#value' => document_search_table($sql),
      '#prefix' => '<div id="document_search_results">',
      '#suffix' => '</div>');

  return ($form);
}
