<?php
// $Id: photos.album.inc,v 1.1.2.3 2009/03/22 18:53:55 eastcn Exp $

//相册展示界面。
//tpl.php: photos_albumview.tpl.php
//2009/2/25 17:16
function photos_album_page($node){
  $order = explode('|', ($node->album['imageorder'] ? $node->album['imageorder']: variable_get('photos_display_imageorder', 'timestamp|desc')));
  $order = _photos_order_value_change($order[0], $order[1]);
  $limit = $node->album['viewpager'] ? $node->album['viewpager'] : variable_get('photos_display_viewpager', 10);
  $term = _photos_order_value($_GET['field'], $_GET['sort'], $limit, $order);
  $result = pager_query('SELECT f.filepath, f.filemime, f.timestamp, f.filename, f.filesize, u.uid, u.name, p.* FROM {files} f INNER JOIN {x_image} p ON f.fid = p.fid INNER JOIN {users} u ON f.uid = u.uid WHERE p.pid = %d'. $term['order'], $term['limit'], 0, NULL, $node->nid);
  $com = variable_get('photos_comment', 0);
  $edit = node_access('update', $node);
  $del = node_access('delete', $node);
  $vote = variable_get('photos_vote', 0);
  $label = $node->album['list_imagesize'] ? $node->album['list_imagesize'] : variable_get('photos_display_list_imagesize', false);
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
    $image['links']['info'] = t('Uploaded on !time by !name', array('!name' => theme('username', (object)$image), '!time' => format_date($image['timestamp'], 'small')));
    if($edit){
      $image['ajax']['edit_url'] = $image['url'] . '/update';
      $image['links']['cover'] = l(t('Set to Cover'),"node/$image[pid]/photos/cover/$image[fid]", array('query' => drupal_get_destination()));
    }
    if($del){
      $image['ajax']['del_id'] = 'id="photos_ajax_del_'.$image['fid'].'"';
      $image['ajax']['del_link'] = l(t('Delete'), 'photos/image/'.$image['fid'].'/delete', array('query' => array('destination' => $_GET['q'], 'pid' => $node->nid, 'uid' => $image['uid']), 'attributes' => array('class' => 'jQueryeditable_edit_delete', 'alt' => 'photos_ajax_del_'.$image['fid'])));
    }
  	if($vote){
      $image['links']['vote'] = theme('photos_vote', $image['fid']);
    }
    $album['view'][] = theme('photos_imageview', $image, 'list');
  }
  if($album['view'][0]){
    $album['access']['edit'] = $edit;
    $album['node_url'] = url("node/$node->nid");
    $album['album_url'] = url("photos/album/$node->nid");
    $album['links'] = _photos_order_link("photos/album/$node->nid", 0, 0, 1);
    $album['pager'] = theme('pager', NULL, $term['limit']);
    $v = theme('photos_albumview', $album, $node);
  }else{
    $v = t('Album is empty');
  }
  _photos_breadcrumb(array(l(t('@name\'s albums', array('@name' => $node->name)), "photos/user/$node->uid/album"), l($node->title, "node/$node->nid")));
  drupal_set_title(t('Albums: @title', array('@title' => check_plain($node->title))));
  return $v;
}

//子相册(非photos类型节点)展示界面。
//tpl.php: photos_albumview.tpl.php
//2009/2/26 0:33
function photos_sub_album_page($node){
  $order = explode('|', variable_get('photos_display_imageorder', 'timestamp|desc'));
  $order = _photos_order_value_change($order[0], $order[1]);
  $term = _photos_order_value($_GET['field'], $_GET['sort'], variable_get('photos_display_viewpager', 10), $order);
  $result = pager_query(db_rewrite_sql('SELECT f.filepath, f.filemime, f.timestamp, f.filename, f.filesize, u.uid, u.name, n.title, p.* FROM {files} f INNER JOIN {x_image} p ON f.fid = p.fid INNER JOIN {x_node} a ON f.fid = a.fid INNER JOIN {node} n ON p.pid = n.nid INNER JOIN {users} u ON f.uid = u.uid WHERE a.nid = %d'. $term['order']), $term['limit'], 0, NULL, $node->nid);
  $com = variable_get('photos_comment', 0);
  $edit = node_access('update', $node);
  $del = node_access('delete', $node);
  $vote = variable_get('photos_vote', 0);
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
      $image['links']['cover'] = l(t('Set to Cover'),"node/$image[pid]/photos/cover/$image[fid]", array('query' => drupal_get_destination()));
    }
    if($del){
      $image['ajax']['del_id'] = 'id="photos_ajax_del_'.$image['fid'].'"';
      $image['ajax']['del_link'] = l(t('Move out'), 'photos/image/'.$image['fid'].'/delete', array('query' => array('destination' => $_GET['q'], 'type' => 'sub_album', 'nid' => $node->nid), 'attributes' => array('class' => 'jQueryeditable_edit_delete', 'alt' => 'photos_ajax_del_'.$image['fid'])));
    }
  	if($vote){
      $image['links']['vote'] = theme('photos_vote', $image['fid']);
    }
    $album['view'][] = theme('photos_imageview', $image, 'list');
  }
  if($album['view'][0]){
    $album['node_url'] = url("node/$node->nid");
    $album['album_url'] = url("photos/sub_album/$node->nid");
    $album['links'] = _photos_order_link("photos/album/$node->nid", $node->subalbum['count'], 0, 1);
    $album['pager'] = theme('pager', NULL, $term['limit']);
    $v = theme('photos_albumview', $album, $node);
  }else{
    $v = t('Sub-Album is empty');
  }
  _photos_breadcrumb(array(l(t('@name\'s albums', array('@name' => $node->name)), "photos/user/$node->uid/album"), l($node->title, "node/$node->nid")));
  drupal_set_title(t('Sub-Album: @title', array('@title' => check_plain($node->title))));
  return $v;
}

function photos_album_share($node){
  $type =  $node->type == 'photos' ? 'album' : 'sub_album';
  $content['flash'] = _photos_slide($node);
  $content['html'] = array(
    'image' => $node->album['cover']['url'] ? _photos_l($node->album['cover']['url']) : '',
    'title' => $node->title,
    'link' => _photos_l("photos/$type/$node->nid"),
   );
  print theme('photos_print', $content, 'share');
}

function photos_album_slide($node){
  if($node->album['count'] || $node->subalbum['count']){
    $type =  $node->type == 'photos' ? 'album' : 'sub_album';
    
    _photos_breadcrumb(array(l(t('@name\'s albums', array('@name' => $node->name)), 'photos/user/'.$node->uid.'/album'), l($node->title, "photos/$type/$node->nid")));
    
    drupal_set_title(t('Slide: @title', array('@title' => check_plain($node->title))));
    
    $v = '<p>'.l(t('Back'), "photos/$type/$node->nid"). ' | '.l(t('Share code'), "photos/$type/$node->nid/share", array('query' => 'KeepThis=true&TB_iframe=true&height=450&width=650', 'attributes' => array('title' =>t('Share code'), 'class' => 'thickbox'))).'</p>';
    $v .= _photos_slide($node);
  }else{
    $v = t('No image');
  }
  return $v;
}

function _photos_slide($node){
  if($node->album['slide']){
    list($width, $height) = explode('x', $node->album['slide']);
  }else{
    list($width, $height) = explode('x', variable_get('photos_display_slide', '640x480'));
  }
  $type =  $node->type == 'photos' ? 'album' : 'sub_album';

  return dfgallery_html(array('url' => url("photos/data/$type/$node->nid/json.json", array('absolute' => true)), 'width' => $width, 'height' => $height));
}