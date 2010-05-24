<?php // $Id: postal_views_handler_argument.inc,v 1.1 2009/08/20 05:18:11 vauxia Exp $

class postal_views_handler_argument extends views_handler_argument {

  function title() {
    // TODO if we know the country, limit it further.
    $type = $this->definition['postal field'];
    if ($title = db_result(db_query("SELECT name FROM {postal} 
      WHERE type = '%s' AND code = '%s'", $type, $this->argument))) {
      return filter_xss($title);
    } 
    return $this->argument;
  }

  function title_query() {
    $titles = array();
    $placeholders = implode(', ', array_fill(0, sizeof($this->value), "'%s'"));
    $type = $this->definition['postal field'];

    // Both countries and states use a code => name convention in the postal
    // table.  Select the name value to represent the title, if availabe.
    $res = db_query("SELECT code, name FROM {postal} 
      WHERE type = '%s' 
      AND code IN ($placeholders)", $type, $this->value);

    while ($row = db_fetch_object($res)) {
    drupal_set_message('<pre>'. print_r($row, 1));
      $name = $row->name ? $row->name : $row->code;
      $titles[] = check_plain($name);
    }
    return $titles;
  }
}
