<?php // $Id: postal.views.inc,v 1.1 2009/08/20 05:18:11 vauxia Exp $

/**
 * Implementation of hook_views_handlers().
 */
function postal_views_handlers() {
  $path = drupal_get_path('module', 'postal') . '/includes/views';
  return array(
    'handlers' => array(
      'postal_views_handler_field' => array(
        'parent' => 'views_handler_field',
        'path' => $path,
      ),
      'postal_views_handler_filter' => array(
        'parent' => 'views_handler_filter',
        'path' => $path,
      ),
      'postal_views_handler_argument' => array(
        'parent' => 'views_handler_argument',
        'path' => $path,
      ),
      'postal_views_handler_filter_state' => array(
        'parent' => 'postal_views_handler_filter_country',
        'path' => $path,
      ),
      'postal_views_handler_filter_country' => array(
        'parent' => 'views_handler_filter_in_operator',
        'path' => $path,
      ),
    ),
  );
}

/**
 * API function: return generic information about a postal entry for views
 */
function postal_views_definition($table, $prefix = NULL) {
  $data = array();

  // The whole postal address.
  $data[$table][$prefix .'postal'] = array(
    'title' => t('Street'),
    'help' => t('A street address.'),
    'field' => array(
      'handler' => 'views_handler_field_postal',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_postal',
    ),
  );

  // City.
  $data[$table][$prefix .'city'] = array(
    'title' => t('City'),
    'help' => t('Name of the city.'),
    'field' => array(
      'handler' => 'views_handler_field_xss',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // State.
  $data[$table][$prefix .'state'] = array(
    'title' => t('State/province'),
    'help' => t('Name of the state or province.'),
    'field' => array(
      'handler' => 'postal_views_handler_field_state',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'postal_views_handler_filter_state',
    ),
    'argument' => array(
      'handler' => 'postal_views_handler_argument_state',
    ),
  );

  // Country.
  $data[$table][$prefix .'country'] = array(
    'title' => t('Country'),
    'help' => t('Name of the country.'),
    'field' => array(
      'handler' => 'postal_views_handler_field_country',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'postal_views_handler_filter_country',
    ),
    'argument' => array(
      'handler' => 'postal_views_handler_argument_country',
    ),
  );

  return $data;
}
