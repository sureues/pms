<?php

// $Id:


define('DOCUMENT_STATUS_UNPUBLISHED', 0);
define('DOCUMENT_STATUS_PUBLISHED', 1);
define('DOCUMENT_INTERNAL', 0);
define('DOCUMENT_EXTERNAL', 1);

function document_get_types($idAsKey = TRUE, $reset = FALSE) {
  static $types;
  if (!isset($types) || $reset) {
    if (!$reset && ($cache = cache_get('document_types')) && !empty($cache->data)) {
      $types = unserialize($cache->data);
    }
    else {
      $vocid = document_get_vocid();
      $result = db_query('SELECT * FROM {term_data} WHERE vid = %d', $vocid);
      $types = array();
      while ($type = db_fetch_object($result)) {
        $types[$type->tid] = $type->name;
      }
      uasort($types, "_document_type_comparer");

      cache_set('document_types', serialize($types));
    }
  }

  $arr = array();
  foreach ($types as $key => $value) {
    if ($idAsKey)
      $arr[$key] = $value;
    else
      $arr[$value] = $value;
  }

  return $arr;
}

function document_get_vocid() {
  $vocid = variable_get('document_vocabulary', '');
  if (empty($vocid)) {
    // Check to see if document vocabulary exists
    $vocid = db_result(db_query("SELECT vid FROM {vocabulary} WHERE module='document'"));
    if ($vocid) {
      // We found a vocabulary, so make sure it is associated with our content.
      $vocabulary = (array) taxonomy_vocabulary_load($vocid);
      $vocabulary['nodes'] = array('document' => 1);
      $status = taxonomy_save_vocabulary($vocabulary);
    }
    else {
      // Didn't find one, so create vocabulary from scratch.
      $vocabulary = array(
          'name' => 'Documents types',
          'multiple' => 0,
          'required' => 1,
          'hierarchy' => 2,
          'relations' => 0,
          'module' => 'document',
          'nodes' => array(
              'document' => 1));
      $status = taxonomy_save_vocabulary($vocabulary);
      $vocid = $vocabulary['vid'];
    }
    variable_set('document_vocabulary', $vocid);
  }
  return $vocid;
}

function document_get_vocab() {
  $vocid = variable_get('document_vocabulary', '');
  $vocab = NULL;
  if (empty($vocid)) {
    $vocid = document_get_vocid();
    $vocab = taxonomy_vocabulary_load($vocid);
  }
  else {
    $vocab = taxonomy_vocabulary_load($vocid);
    if (!$vocab) {
      $vocid = document_get_vocid();
      $vocab = taxonomy_vocabulary_load($vocid);
    }
  }
  return ($vocab);
}

function document_get_years() {
  $years = array();
  for ($i = 1950; $i <= date('Y'); $i++) {
    $years[] = $i;
  }

  return ($years);
}

function document_get_path() {
  $path = variable_get('document_path', '');
  $path = file_directory_path() . '/' . $path;

  return ($path);
}

function document_register_status() {
  drupal_add_js(array(
      'document' => array(
          'STATUS_UNPUBLISHED' => DOCUMENT_STATUS_UNPUBLISHED,
          'STATUS_PUBLISHED' => DOCUMENT_STATUS_PUBLISHED)), 'setting');
}

function document_search_table($sql) {
  $headers = array(
      array('data' => t('Type'), 'field' => 'd.type'),
      array(
          'data' => t('Title'),
          'field' => 'title',
          'sort' => 'asc'),
      array('data' => t('Author'), 'field' => 'author'),
      array(
          'data' => t('Year of Publication'),
          'field' => 'publish_year'),
      'Keywords',
      '');

  //add the order by clause
  $sql .= tablesort_sql($headers);
  $results = pager_query($sql, 10);

  $moderate = user_access('moderate document');
  if ($moderate) {
    array_unshift($headers, '');
  }

  $imgUnpublish = '<img nid="%1$d" src="' . document_image_url('spacer.gif') . '" onclick="doc.changeDocStatus(this, \'icon-unpublish\', false);" class="icon-unpublish" title="Unpublish" width="16" height="16" />';
  $imgDelete = '<img nid="%1$d" src="' . document_image_url('spacer.gif') . '" onclick="doc.deleteDoc(this, \'icon-delete\');" class="icon-delete" title="Delete" width="16" height="16" />';

  $rows = array();
  while ($doc = db_fetch_object($results)) {
    $row = array(
        $doc->type,
        l($doc->title, 'node/' . $doc->nid),
        $doc->author,
        $doc->publish_year,
        $doc->keywords,
        l('Download', $doc->url, array(
            'attributes' => array(
                'target' => '_blank'))));

    if ($moderate) {
      array_unshift($row, sprintf($imgUnpublish . '&nbsp;&nbsp;&nbsp;' . $imgDelete, $doc->nid));
    }
    $rows[] = $row;
  }
  $table = theme('table', $headers, $rows);
  $table .= theme('pager', array(), 10, 0);

  return ($table);
}

function document_image_url($name) {
  $url = base_path() . drupal_get_path('module', 'document') . '/images/' . $name;
  return ($url);
}

function document_mail_text($key, $language = NULL, $variables = array()) {
  $langcode = isset($language) ? $language->language : NULL;

  if ($admin_setting = variable_get('document_' . $key, FALSE)) {
    // An admin setting overrides the default string.
    return strtr($admin_setting, $variables);
  }
  else {
    // No override, return default string.
    switch ($key) {
      case 'publish_subject':
        return t('Your Document got published at !site', $variables, $langcode);
      case 'publish_body':
        return t("!username,\n\nThank you for uploading a document at !site. Your document, \"!node_title\" has been published by the moderators. You can now view your document at: !node_title_link.\n\n\n--  !site team", $variables, $langcode);
    }
  }
}

function document_mail_tokens($account, $node, $language) {
  global $base_url;
  $node_link = 'node/' . $node->nid;
  $tokens = array(
      '!username' => $account->name,
      '!doc_link' => l($node->document_url, $node->document_url),
      '!node_title' => $node->title,
      '!node_title_link' => l($node->title, $node_link),
      '!node_link' => l($node_link, $node_link),
      '!site' => variable_get('site_name', 'Drupal'),
      '!uri' => $base_url,
      '!uri_brief' => preg_replace('!^https?://!', '', $base_url),
      '!mailto' => $account->mail,
      '!date' => format_date(time(), 'medium', '', NULL, $language->language));
  return $tokens;
}

function _document_type_comparer($a, $b) {
  return (strcasecmp($a['type'], $b['type']));
}
