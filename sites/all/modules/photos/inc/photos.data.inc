<?php
// $Id: photos.data.inc,v 1.1.2.3 2009/03/22 18:53:55 eastcn Exp $

function photos_page_slide($ac){
  if($ac->album['album']['count'] && $ac->album['image']['count']){
    list($width, $height) = explode('x', variable_get('photos_display_slide', '640x480'));
    $v = dfgallery_html(array('url' => url("photos/data/user/$ac->uid/json.json", array('absolute' => true)), 'width' => $width, 'height' => $height));
  }else{
    $v = photos_page_title($ac, t('has not yet create an album'));
  }
  return $v;
}

function photos_data_user($ac){
  drupal_set_header('Content-Type: text/plain;');
  drupal_set_header('Cache-Control: no-cache, must-revalidate');
  drupal_set_header('Expires: Sat, 26 Jul 1997 05:00:00 GMT');
  $result = db_query_range(db_rewrite_sql('SELECT n.nid, n.title, f.fid, f.filepath, f.filemime, f.timestamp, f.filename, f.filesize FROM {files} f INNER JOIN {x_image} p ON f.fid = p.fid INNER JOIN {node} n ON p.pid = n.nid INNER JOIN {x_album} x ON n.nid = x.pid WHERE n.uid = %d AND x.count > 0 ORDER BY f.fid DESC'), $ac->uid, 0, 500);
  $thumb = variable_get('photos_slide_show_thumb', 0);
  $view = variable_get('photos_slide_show_view', 0);
  $true = false;
  while ($image = db_fetch_array($result)){
    $t[$image['nid']]['node'] = array(
      'title' => $image['title'],
      'link' => _photos_l('photos/album/'.$image['nid']),
    );
    $images = photos_get_info(0, $image);
    $t[$image['nid']]['image'][] = array(
      'title' => $images['filename'],
      'timestamp' => $images['timestamp'],
      'thumbnail' => _photos_l($images['thumb'][$thumb]),
      'image' => _photos_l($images['thumb'][$view])
    );
    $true = true;
  }
  if($_GET['type'] != 'xml' && $true){
    foreach ($t as $key => $v){
      $album[] = array(
        'properties' => $t[$key]['node'],
        'images' => $t[$key]['image']
      );
    }
    if(variable_get('photos_slide_music', false)){
      $setting['config']['theme']['config_theme_music'] = variable_get('photos_slide_music', false);
    }
    print dfgallery_json($album, $setting);
  }
}

function photos_data_sub_block($node, $op){
  drupal_set_header('Content-Type: text/plain;');
  drupal_set_header('Cache-Control: no-cache, must-revalidate');
  drupal_set_header('Expires: Sat, 26 Jul 1997 05:00:00 GMT');
  switch($op){
    case 'block_new':
      $result = db_query_range('SELECT f.fid, f.filepath, f.filemime, f.timestamp, f.filename, f.filesize, n.title, u.uid, u.name, p.pid, p.comcount, p.count FROM {files} f INNER JOIN {x_image} p ON f.fid = p.fid INNER JOIN {node} n ON p.pid = n.nid INNER JOIN {users} u ON f.uid = u.uid INNER JOIN {x_node} a ON p.fid = a.fid WHERE a.nid = %d ORDER BY f.fid DESC', $node->nid, 0, 3);
      $i = 0;
      while ($images = db_fetch_array($result)){
        $image = photos_get_info(0, $images);
        $t = array();
        if(is_array($image['thumb'])){
          foreach($image['thumb'] as $key => $thumb){
            $t['url'] = _photos_l($thumb);
            break;
          }
        }
        $t['filename'] = $image['filename'];
        $t['link'] = _photos_l('photos/image/'.$image['fid']);
        $array[] = $t;
        ++$i;
      }
      $array['count'] = $i;
  }
  if($_GET['type'] != 'xml' && $array['count']){
    print _photos_json('en', $array);
  }
}

function photos_data_album($node){
  drupal_set_header('Content-Type: text/plain;');
  drupal_set_header('Cache-Control: no-cache, must-revalidate');
  drupal_set_header('Expires: Sat, 26 Jul 1997 05:00:00 GMT');
  if($node->type == 'photos'){
    $order = explode('|', ($node->album['imageorder'] ? $node->album['imageorder'] : variable_get('photos_display_imageorder', 'timestamp|desc')));
    $order = _photos_order_value_change($order[0], $order[1]);
    $result = db_query_range(db_rewrite_sql('SELECT f.fid, f.filepath, f.filemime, f.timestamp, f.filename, f.filesize FROM {files} f INNER JOIN {x_image} p ON f.fid = p.fid WHERE p.pid = %d'. $order, 'p', 'pid'), $node->nid, 0, 100);
  }else{
    $order = explode('|', variable_get('photos_display_imageorder', 'timestamp|desc'));
    $order = _photos_order_value_change($order[0], $order[1]);
    $result = db_query_range(db_rewrite_sql('SELECT f.fid, f.filepath, f.filemime, f.timestamp, f.filename, f.filesize FROM {files} f INNER JOIN {x_image} p ON f.fid = p.fid INNER JOIN {x_node} x ON p.fid = x.fid WHERE x.nid = %d'. $order, 'p', 'pid'), $node->nid, 0, 100);
  }
  $thumb = variable_get('photos_slide_show_thumb', 0);
  $view = variable_get('photos_slide_show_view', 0);
  $true = false;
  while ($images = db_fetch_array($result)){
    $image = photos_get_info(0, $images);
    $array[] = array(
      'title' => $image['filename'],
      'timestamp' => $image['timestamp'],
      'thumbnail' => _photos_l($image['thumb'][$thumb]),
      'image' => _photos_l($image['thumb'][$view])
    );
    $true = true;
  }
  if($true){
    $albums = array(
      array(
        'properties' => array(
          'title' => check_plain($node->title),
          'description' => check_plain($node->teaser),//相册描述
          'icon' => $node->album['cover']['url'],//相册封面
        ),
        'images' => $array,
      )
    );
    if(variable_get('photos_slide_music', false)){
      $setting['config']['theme']['config_theme_music'] = $node->album['slide_music'] ? $node->album['slide_music'] : variable_get('photos_slide_music', false);
    }
    print dfgallery_json($albums, $setting);
  }
}

function _photos_thumb_json($image){
  $t['file']['fid'] = $image['fid'];
  $t['file']['filename'] = $image['filename'];
  $t['user']['link'] = _photos_l('user/'.$image['uid']);
  $t['user']['name'] = $image['name'];
  $t['file']['album_title'] = $image['title'];
  $t['file']['album_link'] = _photos_l('photos/album/'.$image['pid']);
  if($t['file']['des']) $t['file']['des'] = $image['des'];
  $t['original'] = _photos_l($image['original']);
  if(is_array($image['thumb'])){
    foreach($image['thumb'] as $key => $thumb){
      $t['thumb'][$key] = _photos_l($thumb);
    }
  }
  return $t;
}