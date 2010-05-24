<?php // $Id: postal_views_handler_filter_country.inc,v 1.1 2009/08/20 05:18:11 vauxia Exp $

class postal_views_handler_filter_country extends views_handler_filter_in_operator {
  var $value_form_type = 'select';

  function construct() {
    parent::construct();
    $this->value_title = t('Country');
  }

  function get_value_options() {
    if (isset($this->value_options)) {
      return;
    }

    $this->value_options = postal_country_names();
  }
}
