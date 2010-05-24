<?php // $Id: postal.form.inc,v 1.4 2009/09/25 20:10:25 jerdavis Exp $

/**
 * An element #process callback for postal elements.
 */
function postal_form_process_postal($element, $edit, &$form_state, $form) {
  if (!$settings = $element['#postal_settings']) $settings = array();

  $id = is_object($address) ? $address->id : $address;
  $id = check_plain($id);

  if (isset($element['#postal_address'])) {
    $address = postal_address($element['#postal_address']);
  }
  elseif (isset($element['#default_value'])) {
    $address = postal_address($element['#default_value']);
  }

  $country = $address->country;

  $element['#prefix'] = '<div class="postal-form">';
  $element['#suffix'] = '</div>';
  $element['street1'] = array(
    '#type' => 'textfield',
    '#title' => $element['#title'] ? $element['#title'] : t('Street Address'),
    '#default_value' => $address->street1,
    '#size' => 50,
    '#maxlength' => 75,
    '#required' => $element['#required'],
  );
  $element['street2'] = array(
    '#type' => 'textfield',
    '#title' => '',
    '#default_value' => $address->street2,
    '#size' => 50,
    '#maxlength' => 75,
  );
  $element['locale'] = array(
    '#prefix' => '<div class="postal-locale">',
    '#suffix' => '</div><div style="clear: both"></div>',
  );
  $element['locale']['city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
    '#default_value' => $address->city,
    '#size' => 15,
    '#maxlength' => 64,
    '#parents' => array_merge($element['#parents'], array('city')),
    '#required' => $element['#required'],
  );

  // We don't know if country has states.
  if (is_null($country->children)) {
    $element['locale']['state'] = array(
      '#type' => 'textfield',
      '#title' => t('State/Province'),
      '#default_value' => $address->state,
      '#parents' => array_merge($element['#parents'], array('state')),
      '#size' => 12,
      '#maxlength' => 64,
      '#required' => $element['#required'],
    );
  }
  elseif ($country->children > 0) {
    $element['locale']['state'] = array(
      '#type' => 'select',
      '#title' => t('State/Province'),
      '#default_value' => $address->state,
      '#parents' => array_merge($element['#parents'], array('state')),
      '#options' => postal_country_state_names($country->code),
      '#required' => $element['#required'],
    );
  }

  $element['locale']['zip'] = array(
    '#type' => 'textfield',
    '#title' => t('Postal code'),
    '#default_value' => $address->zip,
    '#parents' => array_merge($element['#parents'], array('zip')),
    '#size' => 10,
    '#maxlength' => 20,
    '#required' => $element['#required'],
  );

  // Only show requested fields.
  if (isset($element['#postal_fields'])) {
    foreach ($element as $field => $val) {
      if (!in_array(''. $field, $element['#postal_fields'])) {
        $element[$field]['#access'] = FALSE;
      }
    }
  }

  // The title was used on the street1 field.  It's redundant now.
  if (isset($element['street1'])) unset($element['#title']);

  // "Required" was passed through to the sub-elements.
  unset($element['#required']);

  return $element;
}

/**
 * An element #process callback for postal_general elements.
 */
function postal_form_process_postal_general($element, $edit, &$form_state, $form) {
  if (!$o = $element['#postal_settings']) $o = array();
  $id = $element['#id'];
  $none = array('' => t('- None -'));

  // During an AHAH callback, $edit will have our values on the first pass.
  // Store this value so it's available for the second call.
  if ($edit) $form_state[$id] = postal_element_value($element, $edit);

  // If the stored value exists, set the element's value accordingly.
  if (isset($form_state[$id])) $element['#value'] = $form_state[$id];

  $address = postal_address($element['#value']);

  // Respect any user-configured labels for these elements.
  $labels = $o['labels'];
  $type_label = $labels['type'] ? $labels['type'] : $element['#title'];
  $country = $labels ? $labels['country'] : t('Country');
  $state   = $labels ? $labels['state'] : t('State/Province');
  $city    = $labels ? $labels['city'] : t('City');
  $zip     = $labels ? $labels['zip'] : t('Postal code');

  // These are the types of granularity this input can support.
  $types = array(
    'country' => $country,
    'state'   => $state,
    'city'    => $city,
    'zip'     => $zip,
  );

  // Limit the granularity selection based on configuration settings.
  if (isset($o['selection_types'])) {
    foreach ($o['selection_types'] as $type => $status) {
      if (!$status) unset($types[$type]);
    }
  }

  // Don't allow users to select country/state-level inputs if they're hiding.
  if ($o['force_country']) unset($types['country']);
  if ($o['force_state']) unset($types['state']);

  // Set the default value if there's only one selected type.
  if (count($types) == 1) {
    $address->type = current(array_keys($types));
    $element['type'] = array(
      '#type' => 'value', '#value' => $address->type
    );
  }
  else {
    $element['type'] = array(
      '#type' => 'select',
      '#title' => $type_label,
      '#options' => $none + $types,
      '#default_value' => $address->type,
      '#required' => $element['#required'],
    );

    // Hide a potentially redundant title.
    if ($type_label == $element['#title']) unset($element['#title']);

    // Hide the non-pertinent inputs.
    $js = 'Drupal.behaviors.postalGeneral = function(context) {
      $("#'. $id .'-type").change( function() {
        var typeValue = $(this).val();
        $("div[id*='. $id .'-]:not(#'. $id .'-wrapper):not(#'. $id .'-type-wrapper)").hide();
        $("#'. $id .'-" + typeValue + "-wrapper").show();
        if (typeValue=="state" || typeValue=="city") { $("#'. $id .'-country-wrapper").show(); }
        if (typeValue=="city") { $("#'. $id .'-state-wrapper").show(); }
      });
      $("#'. $id .'-type").trigger("change");
    }';
    drupal_add_js($js, 'inline');
  }

  if ($address->country && $o['force_country']) {
    $element['country'] = array(
      '#type' => 'value', '#value' => $address->country
    );
  }
  else {
    $countries = $none + postal_country_names();
    $element['country'] = array(
      '#type' => 'select',
      '#multiple' => FALSE,
      '#options' => $countries,
      '#title' => t('Country'),
      '#default_value' => $address->country,
      '#required' => $element['#required'],
      '#ahah' => array(
        'path' => 'postal/js/ahah/'. join('/', $element['#array_parents']),
        'wrapper' => $element['#id'] .'-wrapper',
        'method' => 'replace',
        'effect' => 'fade',
      ),
    );
  }

  if ($address->country) {
    $country = postal_country_load($address->country);
    $state_access = (isset($types['state']) || isset($types['city']));

    if ($o['force_state']) {
      $element['state'] = array(
        '#type' => 'value', '#value' => $address->state
      );
    }
    elseif (is_null($country->children)) {
     $element['state'] = array(
       '#type' => 'textfield',
       '#title' => t('State/province'),
       '#default_value' => $address->state,
       '#size' => 12,
       '#maxlength' => 64,
       '#required' => $element['#required'],
       '#access' => $state_access,
     );
    }
    elseif ($country->children > 0) {
      $states = $none + postal_country_state_names($country->code);
      $element['state'] = array(
        '#type' => 'select',
        '#title' => t('State/province'),
        '#default_value' => $address->state,
        '#options' => $states,
        '#required' => $element['#required'],
        '#access' => $state_access,
      );
    }
    $element['city'] = array(
      '#type' => 'textfield',
      '#title' => t('City'),
      '#default_value' => $address->city,
      '#size' => 15,
      '#maxlength' => 64,
      '#required' => $element['#required'] && $address->type == 'city',
      '#access' => isset($types['city']),
    );
    $element['zip'] = array(
      '#type' => 'textfield',
      '#title' => t('Postal code'),
      '#default_value' => $address->zip,
      '#size' => 10,
      '#maxlength' => 20,
      '#required' => $element['#required'] && $address->type == 'zip',
      '#access' => isset($types['zip']),
    );
  }
 
  // How many elements do we have visible?
  $visible = array();
  foreach (element_children($element) as $key) {
    if ($element[$key]['#type'] != 'value' && ($element[$key]['#access'] !== FALSE)) {
      $visible[] = $key;
    }
  }

  // No visible components.  Hide the element entirely.
  if (!$visible) $element['#type'] = 'value';

  // One visible component.  Set its title to the element's title.
  elseif (count($visible) == 1) {
    $element[current($visible)]['#title'] = $element['#title'];
    $element[current($visible)]['#required'] = $element['#required'];
    unset($element['#title']);
  }

  return $element;
}

/**
 * A settings form for the postal_general element. Used by postal_field's
 * "widget settings", and anyplace else that might require similar settings
 */
function postal_form_postal_general_settings($settings = array()) {
  $form = array('#tree' => TRUE);

  // These are the types of granularity this input can support.
  $types = array(
    'country' => t('Country'),
    'state'   => t('State/province'),
    'city'    => t('City'),
    'zip'     => t('Postal code'),
  );

  $form['selection_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Available components'),
    '#options' => $types,
    '#default_value' => $settings ? $settings['selection_types'] : array_keys($types),
    '#description' => t('Select the level of granularity available for selection.  "State" will show a state selector, "Postal code" will show a text box for postal code entry.  If more than one option is checked, visitors can choose which to use.'),
  );

  $form['force_country'] = array(
    '#type' => 'checkbox',
    '#title' => t('Restrict country selection'),
    '#default_value' => $settings ? $settings['force_country'] : FALSE,
    '#description' => t('Hide the country selector and use the default value'),
  );

  $form['force_state'] = array(
    '#type' => 'checkbox',
    '#title' => t('Restrict state selection'),
    '#default_value' => $settings ? $settings['force_country'] : FALSE,
    '#description' => t('Hide the state/province selector and use the default value'),
  );

  $form['labels'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Labels'),
  );
  $form['labels']['country'] = array(
    '#type' => 'textfield',
    '#title' => t('Country'),
    '#default_value' => $settings ? $settings['labels']['country'] : t('Country'),
  );
  $form['labels']['state'] = array(
    '#type' => 'textfield',
    '#title' => t('State/province'),
    '#default_value' => $settings ? $settings['labels']['state'] : t('State/province'),
  );
  $form['labels']['city'] = array(
    '#type' => 'textfield',
    '#title' => t('City'),
    '#default_value' => $settings ? $settings['labels']['city'] : t('City'),
  );
  $form['labels']['zip'] = array(
    '#type' => 'textfield',
    '#title' => t('Postal code'),
    '#default_value' => $settings ? $settings['labels']['zip'] : t('Postal code'),
  );
  return $form;
}

/**
 * An AHAH callback for the postal element.
 */
function postal_form_ahah() {
  // This hack ensures that node forms will work.  YMMV for other forms!
  if (substr($_POST['form_id'], -9) == 'node_form') {
    module_load_include('pages.inc', 'node', 'node');
  }
  elseif ($_POST['form_id'] == 'content_field_edit_form') {
    module_load_include('admin.inc', 'content', 'includes/content');
  }

  $form_build_id = $_POST['form_build_id'];
  $form_state = array('storage' => NULL, 'submitted' => FALSE, 'rebuild' => TRUE);
  // Step #4.
  $form = form_get_cache($form_build_id, $form_state);

  // Preparing for #5.
  $args = $form['#parameters'];
  $form_id = array_shift($args);
  $form_state['post'] = $form['#post'] = $_POST;
  $form['#programmed'] = $form['#redirect'] = FALSE;

  // Step #5.
  drupal_process_form($form_id, $form, $form_state);

  // Step #6 and #7 and #8.
  $form = drupal_rebuild_form($form_id, $form_state, $args, $form_build_id);

  // Clear out the form_errors array.  We're not here for validation.
  drupal_get_messages('error', TRUE);

  // Get the #array_parents value that was passed in to locate the element.
  $parents = func_get_args();
  foreach ($parents as $key) {
    if (isset($form[$key])) $form = $form[$key];
  }
  drupal_render($form);

  $output = theme('status_messages') . $form['#children'];

  // Final rendering callback.
  drupal_json(array('status' => TRUE, 'data' => $output));
}
