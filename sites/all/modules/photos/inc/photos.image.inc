<?php
// $Id: photos.image.inc,v 1.1.2.1 2009/03/06 08:22:41 eastcn Exp $

//photos/image/*
//2009/2/17 23:51
function photos_image_page($fid){
  global $user;
  $image = db_fetch_array(db_query('SELECT f.filepath, f.filemime, f.timestamp, f.filename, n.title, u.uid, u.name, b.data, p.* FROM {files} f INNER JOIN {x_image} p ON f.fid = p.fid INNER JOIN {x_album} b ON p.pid = b.pid INNER JOIN {node} n ON p.pid = n.nid INNER JOIN {users} u ON f.uid = u.uid WHERE p.fid = %d', $fid));
  if(!$image) return drupal_not_found();
  $image = photos_get_info(0, $image);
  $node = db_fetch_object(db_query('SELECT r.format, n.* FROM {node} n INNER JOIN {node_revisions} r ON n.nid = r.nid WHERE n.nid = %d', $image['pid']));
  if(_photos_access('imageEdit', $node)){
    $image['ajax']['edit_url'] = url('photos/image/' . $image['fid'] . '/update');
    if(_photos_select_sub_album()){
      $image['links']['to_sub'] = l(t('To sub-album...'), 'photos/image/' . $image['fid'] . '/to_sub', array('query' => 'KeepThis=true&TB_iframe=true&height=450&width=600', 'attributes' => array('class' => 'thickbox')));
    }
    $image['links']['cover'] = l(t('Set to Cover'),"node/$image[pid]/photos/cover/$fid", array('query' => drupal_get_destination()));
  }
  if(_photos_access('imageDelete', $node)){
    $image['ajax']['del_link'] = l(t('Delete'), 'photos/image/'.$image['fid'].'/delete', array('query' => array('destination' => 'node/'.$image['pid'])));
  }
	if(variable_get('photos_vote', 0)){
    $image['vote'] = theme('photos_vote', $fid);
  }
  if(variable_get('photos_comment', 0)){
    $image['links']['comment'] = theme('photos_comment_count', $image['comcount']);
  }
  $data = unserialize($image['data']);
  $label = $data['view_imagesize'] ? $data['view_imagesize']: variable_get('photos_display_view_imagesize', $view_imagesize) ;
  $image['links']['more'] = l(t('All sizes'), 'photos/zoom/'.$fid);
  $image['links']['pager'] = photos_image_pager($fid, $image['pid']);
  $image['view'] = theme('photos_imagehtml', $image['thumb'][$label], $image);
  $image['comment']['view'] = _photos_vote_comment($fid, $image['comcount'], $image['pid']);
  if (variable_get('photos_comment', 0) && user_access('post comments')) {
  	$image['comment']['box'] = comment_form_box(array('nid' => $image['pid']), t('Post new comment'));
  }
	if(!variable_get('photos_image_count', 0)){
		db_query('UPDATE {x_image} SET count = count + 1 WHERE fid = %d', $fid);
	}
	drupal_set_title($image['filename']);
	_photos_breadcrumb(array(l(t('Images'), 'photos/image'), l(t('@name\'s images', array('@name' => $image['name'])), 'photos/user/'.$image['uid'].'/image'), l($image['title'], "photos/album/$image[pid]")));
  $GLOBALS['photos'][$image['fid'].'_pid'] = $image['pid'];
  return theme('photos_imageview', $image, 'view');
}

//photos/image/*/vote
//2009/2/17 23:50
function photos_image_vote($fid){
  global $user;
  if(user_access('allowed to vote')){
		switch (arg(4)){
      case 'up':
        $value = 1;
      break;
      case 'down':
        $value = -1;
      break;
      default:
        exit();
    }
		$vote = array('content_type' => 'image', 'content_id' => $fid, 'tag' => 'vote', 'value_type' => 'points', 'uid' => $user->uid, 'value' => $value);
		$t = votingapi_set_votes($vote);
    if($t['image'][$fid][0]['value']){
			$x['sum'] = $t['image'][$fid][2]['value'];
      $x['average'] = $t['image'][$fid][1]['value'];
			$x['count'] = $t['image'][$fid][0]['value'];
    }
  }
	if(!$_GET['destination']){
    drupal_set_header('Content-Type: text/plain;');
    if($x['count']){
      foreach($x as $key => $val){
        $array[] = '"'.$key.'":'.$val;
      }
      echo '{'.implode(',', $array).'}';
    }else{
      echo '{"count": 0}';
    }
	}else {
    if($x['count']){
      drupal_set_message(t('Vote success.'));
    }else{
      drupal_set_message(t('Vote failure.'));
    }
		return drupal_goto();
	}
}

//输出图片
function photos_image_get($fid, $thumb){
  $filepath = str_replace('photos/get/'.$fid.'/'.$thumb.'/', '', $_GET['q']);
  if(!is_file($filepath)){
    if($thumb != 'original' && ($thumb = _photos_get_thumbname($thumb))){
      $str = "/thumb_{$fid}.*?thumb_/i";
      $original = preg_replace($str, '', $filepath);
      if(is_file( $original) && ($filepath = photos_get_thumb( $original, $fid, $thumb, 1)))
        $true = 1;
    }
    if(!$true){
      drupal_set_header('Content-Type: text/plain;');
      echo t('Image does not exist');
      exit;
    }
  }
  $image = end(explode('.', $_GET['q']));
  header('Content-type: image/'.$image);
  if ($fd = @fopen($filepath, 'rb')) {
    while (!feof($fd)) {
      print fread($fd, 1024);
    }
    fclose($fd);
    exit();
  }
  drupal_set_header('Content-Type: text/plain;');
  echo t('Image does not exist');
  exit;
}

//单张图片下评论样式
function _photos_vote_comment($fid, $com_count, $nid){
	global $user;
	if(!$com_count){
    return false;
  }
	$output = '';
	if(module_exists('comment') && user_access('access comments')) {
    $node->type = 'photos';
    $node->nid = $nid;
    $mode = _comment_get_display_setting('mode', $node);
    $order = _comment_get_display_setting('sort', $node);
    $comments_per_page = _comment_get_display_setting('comments_per_page', $node);
    $query = 'SELECT c.cid as cid, c.pid, c.nid, c.subject, c.comment, c.format, c.timestamp, c.name, c.mail, c.homepage, u.uid, u.name AS registered_name, u.signature, u.picture, u.data, c.thread, c.status FROM {comments} c INNER JOIN {users} u ON c.uid = u.uid INNER JOIN {x_vote} x ON c.cid = x.cid WHERE x.fid = %d';
    $query_args = array($fid);
    if (!user_access('administer comments')) {
      $query .= ' AND c.status = %d';
      $query_args[] = COMMENT_PUBLISHED;
    }
    if ($order == COMMENT_ORDER_NEWEST_FIRST) {
      if ($mode == COMMENT_MODE_FLAT_COLLAPSED || $mode == COMMENT_MODE_FLAT_EXPANDED) {
        $query .= ' ORDER BY c.cid DESC';
      }else {
        $query .= ' ORDER BY c.thread DESC';
      }
    }else if ($order == COMMENT_ORDER_OLDEST_FIRST) {
      if ($mode == COMMENT_MODE_FLAT_COLLAPSED || $mode == COMMENT_MODE_FLAT_EXPANDED) {
        $query .= ' ORDER BY c.cid';
      }else {
        $query .= ' ORDER BY SUBSTRING(c.thread, 1, (LENGTH(c.thread) - 1))';
      }
    }
    $query = db_rewrite_sql($query, 'c', 'cid');

    $result = pager_query($query, $comments_per_page, 0, NULL, $query_args);

    $divs = 0;
    $num_rows = FALSE;
    $comments = '';
    drupal_add_css(drupal_get_path('module', 'comment') .'/comment.css');
    while ($comment = db_fetch_object($result)) {
      $comment = drupal_unpack($comment);
      $comment->name = $comment->uid ? $comment->registered_name : $comment->name;
      $comment->depth = count(explode('.', $comment->thread)) - 1;
      if ($mode == COMMENT_MODE_THREADED_COLLAPSED || $mode == COMMENT_MODE_THREADED_EXPANDED) {
        if ($comment->depth > $divs) {
          $divs++;
          $comments .= '<div class="indented">';
        }else {
          while ($comment->depth < $divs) {
            $divs--;
            $comments .= '</div>';
          }
        }
      }
      $links = module_invoke_all('link', 'comment', $comment, 0);
      if ($mode == COMMENT_MODE_FLAT_COLLAPSED) {
        $comments .= theme('comment_flat_collapsed', $comment, $node);
      }else if ($mode == COMMENT_MODE_FLAT_EXPANDED) {
        $comments .= theme('comment_view', $comment, $node, $links);
      }else if ($mode == COMMENT_MODE_THREADED_COLLAPSED) {
        $comments .= theme('comment_thread_collapsed', $comment, $node);
      }else if ($mode == COMMENT_MODE_THREADED_EXPANDED) {
        $comments .= theme('comment_view', $comment, $node, $links);
      }
      $num_rows = TRUE;
    }
    while ($divs-- > 0) {
      $comments .= '</div>';
    }
    $comment_controls = variable_get('comment_controls_'. $node->type, COMMENT_CONTROLS_HIDDEN);
    if ($num_rows && ($comment_controls == COMMENT_CONTROLS_ABOVE || $comment_controls == COMMENT_CONTROLS_ABOVE_BELOW)) {
      $output .= drupal_get_form('comment_controls', $mode, $order, $comments_per_page);
    }
    $output .= $comments;
    $output .= theme('pager', NULL, $comments_per_page, 0);
    if ($num_rows && ($comment_controls == COMMENT_CONTROLS_BELOW || $comment_controls == COMMENT_CONTROLS_ABOVE_BELOW)) {
      $output .= drupal_get_form('comment_controls', $mode, $order, $comments_per_page);
    }
    $output = theme('comment_wrapper', $output, $node);
	}
	return $output;
}