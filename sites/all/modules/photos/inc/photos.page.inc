<?php
// $Id: photos.page.inc,v 1.1.2.2 2009/03/22 18:53:55 eastcn Exp $
function photos_page_default(){
  global $user;
  if($user->uid && user_access('create photo')){
    $val = _photos_block_image('user', 5, "photos/user/$user->uid/image", $user->uid);
    $content['user']['image'] = $val[0];
    $val = _photos_block_album('user', 5, "photos/user/$user->uid/album", $user->uid);
    $content['user']['album'] = $val[0];
  }
  $content['site']['image'] = _photos_block_image('latest', 5, 'photos/image');
  $content['site']['album'] = _photos_block_album('latest', 5, 'photos/album');
  return theme('photos_default', $content);
}

function photos_page_album($ac = false){
  if($ac->uid){
    $result = pager_query(db_rewrite_sql('SELECT n.nid FROM {node} n INNER JOIN {x_album} p ON n.nid = p.pid WHERE n.uid = %d ORDER BY n.nid DESC'), 10, 0, NULL, $ac->uid);
    _photos_breadcrumb(array(l(t('Albums'), 'photos/album')));
  }else{
    $result = pager_query(db_rewrite_sql('SELECT n.nid FROM {node} n INNER JOIN {x_album} p ON n.nid = p.pid ORDER BY n.nid DESC'), 10, 0, NULL, $ac->uid);
  }
  while($node = db_fetch_object($result)){
    $output .= node_view(node_load($node->nid), 1);
  }
  if($output){
    $output .= theme('pager', NULL, 10);
  }else{
    $output .= t('@name has not yet create an album', array('@name' => $ac->name));
  }
  return $output;
}

function photos_page_image($ac = false){
  global $user;
  $order = explode('|', variable_get('photos_display_imageorder', 'timestamp|desc'));
  $order = _photos_order_value_change($order[0], $order[1]);
  $term = _photos_order_value($_GET['field'], $_GET['sort'], variable_get('photos_display_viewpager', 10), $order);
  if($ac->uid){
    $result = pager_query(db_rewrite_sql('SELECT f.filepath, f.filemime, f.timestamp, f.filename, f.filesize, u.uid, u.name, n.title, p.* FROM {files} f INNER JOIN {x_image} p ON f.fid = p.fid INNER JOIN {node} n ON p.pid = n.nid INNER JOIN {users} u ON f.uid = u.uid WHERE f.uid = %d'. $term['order']), $term['limit'], 0, NULL, $ac->uid);
    $album['links'] = _photos_order_link('photos/user/image/'.$ac->uid, $ac->album['image']['count'], l(t('Slideshow'), 'photos/user/'.$ac->uid.'/slide'), 1);
    _photos_breadcrumb(array(l(t('Images'), 'photos/image')));
  }else{
    $result = pager_query(db_rewrite_sql('SELECT f.filepath, f.filemime, f.timestamp, f.filename, f.filesize, u.uid, u.name, n.title, p.* FROM {files} f INNER JOIN {x_image} p ON f.fid = p.fid INNER JOIN {node} n ON p.pid = n.nid INNER JOIN {users} u ON f.uid = u.uid'. $term['order']), $term['limit']);
    $album['links'] = _photos_order_link('photos/image', photos_get_count('site_image'), 0, 1);
  }
  $com = variable_get('photos_comment', 0);
  if($user->uid && $ac->uid == $user->uid){
    $edit = 1;
  }
  $vote = variable_get('allowed to vote', 0);
  $label = variable_get('photos_display_list_imagesize', false);
  while($data = db_fetch_array($result)){
    $image = photos_get_info(0, $data);
    $image['view'] = theme('photos_imagehtml', $image['thumb'][$label], array('filename' => $image['filename']));
    $image['url'] = url('photos/image/'.$image['fid']);
    if($com){
      $image['links']['comment'] = theme('photos_comment_count', $image['comcount'], $image['url']);
    }
    if($image['count']){
      $image['links']['count'] = t('!cou visits',array('!cou' => $image['count']));
    }
    $image['links']['info'] = t('!name in !time upload to !title', array('!name' => theme('username', (object)$image), '!time' => format_date($image['timestamp'], 'small'), '!title' => l($image['title'], 'photos/album/'.$image['pid'])));
    if($edit){
      $image['ajax']['edit_url'] = $image['url'] . '/update';
      $image['ajax']['del_id'] = 'id="photos_ajax_del_'.$image['fid'].'"';
      $image['ajax']['del_link'] = l(t('Delete'), 'photos/image/'.$image['fid'].'/delete', array('query' => array('destination' => $_GET['q'], 'pid' => $image['pid'], 'uid' => $image['uid']), 'attributes' => array('class' => 'jQueryeditable_edit_delete', 'alt' => 'photos_ajax_del_'.$image['fid'])));
      $image['links']['cover'] = l(t('Set to Cover'),"node/$image[pid]/photos/cover/$image[fid]", array('query' => drupal_get_destination()));
    }
  	if($vote){
      $image['links']['vote'] = theme('photos_vote', $image['fid']);
    }
    $album['view'][] = theme('photos_imageview', $image, 'list');
  }
  if($album['view'][0]){
    $album['pager'] = theme('pager', NULL, $term['limit']);
    $v = theme('photos_albumview', $album, NULL);
  }else{
    $v = t('@name has not yet upload an image', array('@name' => $ac->name));
  }
  return $v;
}

function photos_share(){
  global $user;
  $order = explode('|', variable_get('photos_display_imageorder', 'timestamp|desc'));
  $order = _photos_order_value_change($order[0], $order[1]);
  $term = _photos_order_value($_GET['field'], $_GET['sort'], 20, $order);
  if(arg(2) != 'all'){
    $result = pager_query('SELECT f.fid, f.filepath, f.filemime, f.filename FROM {files} f INNER JOIN {x_image} p ON f.fid = p.fid WHERE f.uid = %d'. $term['order'], $term['limit'], 0, NULL, $user->uid);
    $images['menu'] = _photos_order_link('photos/share', 0, 0, 1);
  }else{
    $result = pager_query(db_rewrite_sql('SELECT f.fid, f.filepath, f.filemime, f.timestamp, f.filename, f.filesize, u.uid, u.name FROM {files} f INNER JOIN {x_image} p ON f.fid = p.fid INNER JOIN {users} u ON f.uid = u.uid WHERE f.uid != %d'. $term['order'], 'p', 'pid'), $term['limit'], 0, NULL, $user->uid);
    $images['menu'] = _photos_order_link('photos/share/all', 0, 0, 1);
  }
  $label = variable_get('photos_title_0', false);
  while($data = db_fetch_array($result)){
    $image = photos_get_info(0, $data);
    if(is_array($image['thumb'])){
      foreach($image['thumb'] as $key => $thumb){
        $thumbs[$key] = _photos_l($thumb);
      }
    }
    if($data['name']){
      $username = t('By: !name', array('!name' => l($data['name'], "photos/user/$data[uid]/image")));
    }
    $share[] = array(
      'link' => _photos_l('photos/image/'.$image['fid']),
      'fid' => $image['fid'],
      'filename' => $image['filename'],
      'thumbs' => $thumbs,
      'username' => $username,
      'view' => theme('photos_imagehtml', $image['thumb'][$label], array('filename' => $image['filename']))
    ); 
  }
  $images['links'] = l(t('« Back'), 'photos') .l(t('My images'), 'photos/share') . l(t('All the shared images'), 'photos/share/all') ;
  $images['data'] = $share[0]['fid'] ? $share : '';
  $v .= theme('photos_share', $images, 'image');
  $v .= theme('pager', NULL, $term['limit']);
  print $v;
}