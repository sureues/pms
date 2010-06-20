<?php

// $Id:


require_once (drupal_get_path('module', 'document') . '/document.inc');

function document_search() {
  try {
    _document_headers();

    $criteria = $_REQUEST['criteria'];

    if (!isset($criteria)) {
      die(t('Invalid input.'));
    }

    $sql = sprintf('SELECT * FROM {node} n INNER JOIN {document} d ON n.vid = d.vid WHERE n.status = %d', DOCUMENT_STATUS_PUBLISHED);
    $where = '';

    $criteria = json_decode($criteria);
    switch ($criteria->searchFields) {
      case 0:
        $where .= sprintf(" AND author LIKE '%%%1\$s%%' ", db_escape_string($criteria->searchText));
        break;
      case 1:
        $where .= sprintf(" AND keywords LIKE '%%%1\$s%%' ", db_escape_string($criteria->searchText));
        break;
      case 2:
        $where .= sprintf(" AND author LIKE '%%%1\$s%%' OR keywords LIKE '%%%1\$s%%' ", db_escape_string($criteria->searchText));
        break;
      case 3:
        break;
      default:
        die('Invalid Input');
    }

    $year = $criteria->searchYear;
    if (isset($year) && strlen($year) > 0) {
      $where .= sprintf(" AND publish_year = %1\$s ", db_escape_string($year));
    }

    $docType = $criteria->searchDocType;
    if (isset($docType) && strlen($docType) > 0) {
      $where .= sprintf(" AND d.type = '%1\$s' ", db_escape_string($docType));
    }

    $sql .= $where;

    //db_query adds parameter substitution, so things like %d, %s are processed as arguments to the db_query call.
    //A side effect is that the use of % in a query needs to be double so for example "%blah%" would become "%%blah%%"
    $sql = str_replace('%', '%%', $sql);

    $table = document_search_table($sql);

    die($table);
  }
  catch (Exception $e) {
    die($e->getMessage() + "\n" + $e->getTraceAsString());
  }
}

function document_add_type() {
  try {
    _document_headers();

    $type = $_REQUEST['type'];

    if (!isset($type)) {
      die(t('Invalid input.'));
    }

    $types = document_get_types(FALSE);
    if (array_key_exists($type, $types)) {
      die(t('The specified Document Type already exists.'));
    }

    $obj = array(
        'tid' => '',
        'vid' => document_get_vocid(),
        'name' => $type,
        'description' => '',
        'weight' => 0);
    drupal_write_record('term_data', $obj);

    $hier = array('tid' => $obj['tid'], 'parent' => 0);
    drupal_write_record('term_hierarchy', $hier);

    cache_clear_all();
    cache_clear_all('document_types', 'cache');
    die($obj['tid']);
  }
  catch (Exception $e) {
    die($e->getMessage() + "\n" + $e->getTraceAsString());
  }
}

function document_delete_types() {
  try {
    _document_headers();

    $ids = $_REQUEST['ids'];

    if (!isset($ids)) {
      die(t('Invalid input.'));
    }

    $ids = explode(',', $ids);
    $types = document_get_types(TRUE);

    foreach ($ids as $id) {
      $count = db_result(db_query("SELECT COUNT(*) FROM document AS d WHERE d.type = '%s'", $types[$id]));
      if ($count > 0) {
        die('The type(s) have documents associated to them. Please delete the documents first before deleting the type.');
      }
    }

    foreach ($ids as $id) {
      taxonomy_del_term($id);
    }

    cache_clear_all();
    cache_clear_all('document_types', 'cache');
    die();
  }
  catch (Exception $e) {
    die($e->getMessage() + "\n" + $e->getTraceAsString());
  }
}

function document_delete_doc() {
  try {
    _document_headers();

    $ids = $_REQUEST['ids'];

    if (!isset($ids)) {
      die(t('Invalid input.'));
    }

    $ids = explode(',', $ids);

    foreach ($ids as $id) {
      node_delete($id);
    }

    die();
  }
  catch (Exception $e) {
    die($e->getMessage() + "\n" + $e->getTraceAsString());
  }
}

function document_change_doc_status() {
  try {
    _document_headers();

    $ids = $_REQUEST['ids'];
    $status = $_REQUEST['status'];

    if (!isset($ids) || !isset($status)) {
      die(t('Invalid input.'));
    }
    else if ($status != DOCUMENT_STATUS_PUBLISHED && $status != DOCUMENT_STATUS_UNPUBLISHED) {
      die(t('Invalid input.'));
    }

    $ids = explode(',', $ids);

    foreach ($ids as $id) {
      $node = node_load($id, NULL, TRUE);
      $node->status = $status;
      $node->document_publishing = TRUE;
      node_save($node);

      if ($status == DOCUMENT_STATUS_PUBLISHED && variable_get('document_publish_email', TRUE)) {
        $uid = $node->uid;
        $user = user_load(array('uid' => $uid));
        if ($user) {
          drupal_mail('document', 'publish', $user->mail, user_preferred_language($user), array(
              'node' => $node,
              'account' => $user));
        }
      }
    }

    die();
  }
  catch (Exception $e) {
    die($e->getMessage() + "\n" + $e->getTraceAsString());
  }
}

function _document_headers() {
  header("Content-type: text/html");
  header("Expires: Wed, 29 Jan 1975 04:15:00 GMT");
  header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
  header("Cache-Control: no-cache, must-revalidate");
  header("Pragma: no-cache");
}
