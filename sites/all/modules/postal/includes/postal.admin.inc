<?php // $Id: postal.admin.inc,v 1.2 2009/08/11 02:03:33 vauxia Exp $

/**
 * Menu callback for the postal administration screen.
 */
function postal_admin(&$form_state, $scope = 'default') {
  $countries = postal_country_load();
  $enabled   = postal_country_load('all', $scope);
  
  // 
  if (!$countries) {
    postal_admin_load_countries();
    if (!$countries = postal_country_load(NULL, NULL, TRUE)) {
      drupal_set_message('Unable to get information about available countries.  Please check your server\'s connection to the internet and try again', 'error');
    }
    return array();
  }

  $form = array(
    '#postal_scope' => $scope,
  );

  foreach ($countries as $c) {
    $key = $c->iso;

    $form[$key] = array(
      '#tree' => TRUE,
//      '#weight' => isset($enabled[$key]) ? $enabled[$key]['weight'] : 0,
    );

    $form[$key]['id'] = array('#type' => 'value', '#value' => $country->id);
    $form[$key]['scope'] = array('#type' => 'value', '#value' => $scope);
    
    // If the scope hasn't been initialized, default status is "ON"
    $default = $enabled ? 0 : 1;
    $status = isset($enabled[$key]) ? $enabled[$key]['status'] : $default;

    $form[$key]['status'] = array(
      '#type' => 'checkbox',
      '#title' => $c->name,
      '#default_value' => $status,
    );

    // TODO weight (tabledrag)

    // Import children/update children.
    $path = $_GET['q'] .'/import/'. $key;
    if (is_null($c->children)) {
      $form[$key]['import'] = array(
        '#type' => 'markup',
        '#value' => l('import states/provinces', $path, array('destination' => $_GET['q'])),
      );
    }
    elseif ($c->children !== 0) {
      $form[$key]['update'] = array(
        '#type' => 'markup',
        '#value' => l('update states/provinces', $path, array('destination' => $_GET['q'])),
      );
    }
  }

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));

  return $form;
}

/**
 * Submit callback for the postal administration form.
 */
function postal_admin_submit($form, &$form_state) {
  foreach (element_children($form) as $country) {
    if (isset($form[$key]['id'])) {
      $key = array('id', 'scope');
      drupal_write_record('postal_admin', $form_state['values'][$key], $key);
    }
  }
}

function postal_admin_import_states($country) {
  postal_admin_load_states($country);
  drupal_goto();
}

/**
 * Helper function to import the entire country list.
 * This is a quick and dirty function call to the country information provided 
 * by the GeoNames service, updated regularly.
 */
function postal_admin_load_countries() {
  // Get existing values to test whether we're inserting or updating records.
  $existing = postal_country_load();

  $ret = drupal_http_request('http://download.geonames.org/export/dump/countryInfo.txt');

  foreach (explode("\n", $ret->data) as $row) {
    if ($row[0] == '#') continue;  // Skip comments.

    $c = explode("\t", $row);
    $item = array(
      'id' => $c[16],
      'parent_id' => NULL,
      'type' => 'country',
      'code' => $c[0],
      'name' => $c[4],
      'iso' => $c[0],
      'iso3' => $c[1],
      'ison' => $c[2],
      'fips' => $c[3],
      'zip_format' => $c[13],
      'zip_regex' => $c[14],
    );

    if ($item['code']) {
      $update = ($existing && isset($existing[$item['code']])) ? 'id' : NULL;
      drupal_write_record('postal', $item, $update);
    }
  }
}

function postal_admin_load_states($country) {
  $req = 'http://ws.geonames.org/childrenJSON?geonameId='. (int)$country->id;
  $ret = drupal_http_request($req);

  if ($result = json_decode($ret->data)) {
    db_query("DELETE FROM {postal} WHERE parent_id = %d", $country->id);

    foreach ($result->geonames as $item) {
      $state = array(
       'id' => $item->geonameId,
       'code' => $item->adminCode1,
       'parent_id' => $country->id,
       'type' => 'state',
       'name' => $item->name,
       'children' => $item->numberOfChildren,
      );

      drupal_write_record('postal', $state);
    }

    // Store the totalResultsCount value in the country's "children" attribute.
    $update = array('id' => $country->id, 'children' => $result->totalResultsCount);
    drupal_write_record('postal', $update, 'id');
  }
}
