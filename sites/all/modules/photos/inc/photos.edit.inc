<?php
// $Id: photos.edit.inc,v 1.1.2.3 2009/03/22 18:53:55 eastcn Exp $
function photos_edit_upload(){
  global $user;
	$header = array(
		array('data' => t('Album title'), 'field'=> 'n.title'),
		array('data' => t('Image count'), 'field'=> 'p.count'),
    array('data' => t('Link')),
		array('data' => t('Create time'), 'field'=> 'n.nid', 'sort' => 'desc'),
	);
  $result = pager_query('SELECT n.*, p.count FROM {node} n INNER JOIN {x_album} p ON n.nid = p.pid WHERE n.uid = %d' .tablesort_sql($header), 20, 0, NULL, $user->uid);
  while($node = db_fetch_object($result)){
    $table[] = array(
      l($node->title, 'node/'.$node->nid),
      $node->count,
      array(
        'data' => l(t('Upload'), "node/$node->nid/photos") . l(t('View'), "photos/album/$node->nid") . l(t('Slideshow'), "photos/Slide/$node->nid"),
        'class' => 'photos_album_edit_table_link'
      ),
      format_date($node->created, 'small')
    );
  }
  $v = drupal_get_form('photos_upload_form', 0, array('submit' => true));
	if (empty($table)) {
		$table[] = array(array('data' => t('Please create a album.'), 'colspan' => 4));
	}else{
    $v .= t('Or select an album: ');
  }
  $v .= theme('table', $header, $table);
  $v .= theme('pager', NULL, 20);
  return $v;
}
//图片上传
function photos_upload_form(& $form_state, $node = false, $edit = array()) {
	global $user;
	$form['new'] = array(
		'#title' => t('Image upload'),
		'#weight' => -4,
		'#type' => 'fieldset',
    '#collapsible' => TRUE,
		'#description' => t('Allow the type:').' jpg gif png jpeg '.((variable_get('photos_upzip',0) && $_SESSION['photos_swfu_switch']) ? ' zip': NULL),
	);
	if($node->type == 'photos' || photos_get_count('user_album', $user->uid)){
		for($i =0; $i < variable_get('photos_num',5); ++$i){
      $form['new']['images_'.$i] = array(
        '#type' => 'file'
      );
      $form['new']['title_'.$i] = array(
        '#type'=> 'textfield',
        '#title' => t('Image title'),
      );
      $form['new']['des_'.$i] = array(
        '#type'=> 'textarea',
        '#title' => t('Image description'),
        '#cols' => 40,
        '#rows' => 3,
      );
		}
	}else{
	    drupal_set_message(t('You must first !create an album to upload images.', array('!create' => l(t('create'), 'node/add/photos', array('query' => drupal_get_destination())))));
	    return $form;
	}
	if($node->type == 'photos'){
		$form['new']['pid'] = array(
			'#type' => 'value',
			'#value' => $node->nid
		);
	}else{
		$form['new']['pid'] = array(
			'#title' => t('Upload to album'),
			'#type' => 'select',
			'#options' => _photos_useralbum_option($user->uid),
			'#default_value' => $_GET['pid'],
			'#required' => TRUE,
      '#prefix' => '<div id="photos-pid">',
      '#suffix' => '</div>',
			'#weight' => -5,
		);
		$form['new']['nid'] = array(
			'#type' => 'value',
			'#value' => $node->nid
		);
	}
	if($edit['submit']){
		$form['new']['submit'] = array(
			'#type' => 'submit',
			'#value' => t('Confirm upload'),
			'#weight' => 10,
			'#submit' => array('photos_upload_form_submit'),
		);
    $form['#action'] = url($_GET['q'], array('query' => drupal_get_destination()));
		$form['#attributes']['enctype'] = 'multipart/form-data';
	}
	return $form;
}
//hook_form_submit
function photos_upload_form_submit($form, & $form_state) {
	global $user;
	$limits = _upload_file_limits($user);
	$validators = array(
		'file_validate_is_image' => array(),
		'file_validate_image_resolution' => array($limits['resolution']),
		'file_validate_size' => array($limits['file_size'], $limits['user_size'])
	);
  $photos_num = variable_get('photos_num', 5);
	for ($i = 0; $i < $photos_num; ++$i){
    _photos_rename();
		if($_FILES['files']['name']['images_'.$i]){
      $value = new stdClass();
      $value->pid = $form_state['values']['pid'];
      $value->nid = $form_state['values']['nid'];
      $value->des = $form_state['values']['des_'.$i];
      $value->title = $form_state['values']['title_'.$i];
			$ext = drupal_substr($_FILES['files']['name']['images_'.$i], -3);
			if($ext != 'zip' && $ext != 'ZIP'){
				if ($file = file_save_upload('images_'.$i, $validators, photos_check_path())) {
					$msg[] = photos_image_date((object)array_merge((array)$file, (array)$value), 1);
				}
			}else{
				if(!variable_get('photos_upzip',0)){
					return form_set_error('errer', t('Please Album photos set to open zip upload'));
				}
        $zip = file_destination(file_create_path(photos_check_path() .'/'.trim(basename($_FILES['files']['name']['images_'.$i]))), FILE_EXISTS_RENAME);
				if (move_uploaded_file($_FILES['files']['tmp_name']['images_'.$i], $zip)) {
					if(!$msg = _photos_unzip($zip, $value, $limits)){
						$msg = t('Failed to upload');
					}
				}
			}
		}
	}
	if(is_array($msg)){
		return drupal_set_message(t('%count image(s) uploaded successfully.', array('%count'=> count($msg))));
	}else{
		return drupal_set_message($msg);
	}
}
//图片管理
function photos_edit_page($node){
  $output = '<div id="photos_upload_new">'.drupal_get_form('photos_upload_form', $node, array('submit' => true)).'</div>';
	if($node->type == 'photos'){
    $output .= _photos_edit_page_album($node);
    drupal_set_title(t('Organize Albums: @title', array('@title' => check_plain($node->title))));
	}else{
    $output .= _photos_edit_page_node($node);
    drupal_set_title(t('Organize Sub-Album: @title', array('@title' => check_plain($node->title))));
	}
	return $output;
}

function _photos_edit_page_node($node){
	if($node->subalbum['count']){
    $term = _photos_order_value($_GET['field'], $_GET['sort'], 10);
    $result = pager_query('SELECT f.filepath, f.filemime, f.timestamp, f.filename, f.filesize, n.title, u.uid, u.name, p.* FROM {files} f INNER JOIN {x_image} p ON f.fid = p.fid INNER JOIN {node} n ON p.pid = n.nid INNER JOIN {users} u ON f.uid = u.uid INNER JOIN {x_node} a ON p.fid = a.fid WHERE a.nid = %d'. $term['order'], $term['limit'], 0, NULL, $node->nid);
    $output .= _photos_order_link("node/$node->nid/photos", $node->subalbum['count'], l(t('Album view'), "photos/sub_album/$node->nid"), 1);
    $output .= '<div class="messages">'.t('Move image: Move out of the image, but it will not delete it.').'</div>';
  }else{
    return;
  }
  while($data = db_fetch_array($result)){
    $images[] = photos_get_info(0, $data);
  }
  if($images[0]['fid']){
    $images[0]['info'] = array(
      'title' => $node->title,
      'nid' => $node->nid,
      'uid' => $node->uid
    );
    $output .= theme('pager', NULL, $term['limit']);
    $output .= drupal_get_form('photos_editlist', $images, 'node');
    $output .= theme('pager', NULL, $term['limit']);
  }
	return $output;
}

function _photos_edit_page_album($node){
	if($node->album['count']){
    $term = _photos_order_value($_GET['field'], $_GET['sort'], 10);
    $result = pager_query('SELECT f.filepath, f.filemime, f.timestamp, f.filename, f.filesize, u.uid, u.name, p.* FROM {files} f INNER JOIN {x_image} p ON f.fid = p.fid INNER JOIN {users} u ON f.uid = u.uid WHERE p.pid = %d'. $term['order'], $term['limit'], 0, NULL, $node->nid);
    $output .= _photos_order_link("node/$node->nid/photos", $node->album['count'], l(t('Album view'), "photos/album/$node->nid"), 1);
  }else{
    return;
  }
  while($data = db_fetch_array($result)){
    $images[] = photos_get_info(0, $data);
  }
  if($images[0]['fid']){
    $images[0]['info'] = array(
      'cover' => $node->album['cover'],
      'pid' => $node->nid,
      'title' => $node->title,
      'uid' => $node->uid
    );
    $output .= theme('pager', NULL, $term['limit']);
    $output .= drupal_get_form('photos_editlist', $images);
    $output .= theme('pager', NULL, $term['limit']);
  }
	return $output;
}

//2009/2/18 16:33
function photos_editlist(&$form_state, $images = array(), $type = 'album') {
  global $user;
  if($type == 'album'){
    if($user->uid != $images[0]['info']['uid']) {
      $albumUpdate = array($images[0]['info']['pid'], $images[0]['info']['title']);
    }
    $albumPid = _photos_useralbum_option($user->uid, $albumUpdate);
    $delLabel = _photos_del_checkbox(0, t('Delete'));
    $form['cover_fid'] = array('#type' => 'hidden', '#default_value' => $images[0]['info']['cover']['fid']);
    $form['oldpid'] = array('#type' => 'hidden', '#default_value' => $images[0]['info']['pid']);
    $submit = 'photos_editlist_submit';
  }else{
    $delLabel = _photos_del_checkbox(0, t('Move out'));
    $submit = 'photos_editlist_submit_node';
    $form['nid'] = array('#type' => 'hidden', '#default_value' => $images[0]['info']['nid']);
  }
	$form['photos']['#theme'] = 'photos_editlist_x';
	$form['photos']['#tree'] = true;
	$form['#action'] = url($_GET['q'], array('query' => drupal_get_destination(). (isset($_GET['page']) ? '&page=' . intval($_GET['page']): NULL)));
  $thumb = variable_get('photos_title_0', false);
	foreach ($images as $image) {
		$form['photos'][$image['fid']]['del'] = $delLabel;
    $image['user']->uid = $image['uid'];
    $image['user']->name = $image['name'];
    $item = array();
    $item[] = theme('photos_imagehtml', $image['thumb'][$thumb], array('href' => 'photos/image/'.$image['fid']));
    if($type == 'album' && $images[0]['info']['cover']['fid'] != $image['fid']){
      $item[] = l(t('Set to Cover'),"node/$image[pid]/photos/cover/$image[fid]");
    }
    $item[] = t('Filesize: !size KB', array('!size' => round($image['filesize']/1024)));
    if($image['count']){
      $item[] = t('Visits: !count', array('!count' => $image['count']));
    }
    if($image['comcount']){
      $item[] = t('Comments: !count', array('!count' => $image['comcount']));
    }
		$form['photos'][$image['fid']]['path']['#value'] = theme('item_list', $item);
		$form['photos'][$image['fid']]['des'] = array(
      '#title' => t('Image description'),
      '#type' => 'textarea',
      '#default_value' => preg_replace('/<.*?>/', '', $image['des']),
      '#cols' => 40,
      '#rows' => 4
    );
		$form['photos'][$image['fid']]['filename'] = array(
      '#title' => t('Image title'),
      '#type' => 'textfield',
      '#default_value' => $image['filename'],
      '#required' => TRUE
    );
		$form['photos'][$image['fid']]['wid'] = array(
      '#title' => t('Weight'),
      '#type' => 'weight',
      '#default_value' => $image['wid'],
      '#delta' => 100,
    );
    $form['photos'][$image['fid']]['filepath'] = array('#type' => 'value','#value' => $image['filepath']);
    if($type == 'album'){
      $uploadInfo = t('Uploaded on !time by !name', array('!name' => theme('username', $image['user']), '!time' => format_date($image['timestamp'], 'small')));
      $form['photos'][$image['fid']]['pid'] = array(
        '#title' => t('Move to the album'),
        '#type' => 'select',
        '#options' => $albumPid,
        '#default_value' => $image['pid'],
        '#required' => TRUE
      );
    }else{
      $uploadInfo = t('!name in !time upload to !title', array('!name' => theme('username', $image['user']), '!time' => format_date($image['timestamp'], 'small'), '!title' => l($image['title'], 'node/'.$image['pid'])));
    }
    $form['photos'][$image['fid']]['time']['#value'] = $uploadInfo;
		$form['photos'][$image['fid']]['uid'] = array('#type' => 'hidden', '#default_value' => $image['uid']);
    $form['photos'][$image['fid']]['oldfilename'] = array('#type' => 'hidden', '#default_value' => $image['filename']);
	};
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Confirm the change'),
		'#submit' => array($submit),
	);
	return $form;
}

//节点下提交处理
function photos_editlist_submit_node($form, &$form_state) {
	foreach($form_state['values']['photos'] as $fid => $key) {
		if (!empty($key['del'])) {
      $msg[] = db_query('DELETE FROM {x_node} WHERE fid = %d', $fid);
		}else{
			db_query("UPDATE {x_image} SET des = '%s', wid = %d WHERE fid = %d", $key['des'], $key['wid'], $fid);
      if($key['filename'] != $key['oldfilename']){
        db_query("UPDATE {files} SET filename = '%s' WHERE fid = %d", check_plain($key['filename']), $fid);
      }
		}
	}
  if($msg) {
    photos_set_count('node_node', $form_state['values']['nid']);
    drupal_set_message(t('%count images are move out.', array('%count' => count($msg))));
  }
}

//相册下提交处理
function photos_editlist_submit($form, &$form_state) {
	foreach($form_state['values']['photos'] as $fid => $key) {
		if (!empty($key['del'])) {
      if($form_state['values']['cover_fid'] == $fid){
        db_query('UPDATE {x_album} SET fid = 0 WHERE pid = %d', $form_state['values']['oldpid']);
      }
			$msg[] = photos_file_del($fid, $key['filepath']);
      $uids[] = $key['uid'];
		}else{
			db_query("UPDATE {x_image} SET pid = %d, des = '%s', wid = %d WHERE fid = %d", $key['pid'], $key['des'], $key['wid'], $fid);
      if($key['filename'] != $key['oldfilename']){
        db_query("UPDATE {files} SET filename = '%s' WHERE fid = %d", check_plain($key['filename']), $fid);
      }
      if ($key['pid'] != $form_state['values']['oldpid']) {
        db_query('UPDATE {comments} SET nid = %d WHERE cid IN (SELECT cid FROM {x_vote} WHERE fid = %d)', $key['pid'], $fid);
        $pid[] = $key['pid'];
        $uids[] = $key['uid'];
      }
		}
	}
  if($msg) {
    $pid[] = $form_state['values']['oldpid'];
    drupal_set_message(t('%count images are deleted.', array('%count' => count($msg))));
  }
  if(count($pid)){
    foreach($pid as $nid){
      _comment_update_node_statistics($nid);
      photos_set_count('node_album', $nid);
    }
    $uid = array_unique($uids);
    foreach ($uid as $id){
      photos_set_count('user_image', $id);
    }
  }
}

function _photos_del_checkbox($t = 0, $title = false){
  if($t){
    return  (($t == 1) ? t('Delete') : $t).'<input type="checkbox" id="del_check"/>';
  }else{
    return  array('#title' => $title, '#type' => 'checkbox', '#prefix' => '<div class="edit-del-all">', '#suffix' => '</div>');
  }
}

function theme_photos_editlist_x($form) {
	$header = array(
		array('data' => _photos_del_checkbox(t('Select All'))),
		array('data' => t('Thumbnails')),
		array('data' => t('Content')),
	);
  foreach (element_children($form) as $key) {
    $rows[] = array(
      drupal_render($form[$key]['wid']) . drupal_render($form[$key]['del']),
      array(
        'data' => drupal_render($form[$key]['path']), 
        'class' => 'photos_edit_thumb'
      ),
      '<div class="photos-edit-filename">'.drupal_render($form[$key]['filename']) .'</div><div class="photos-edit-des">'. drupal_render($form[$key]['des']) . '</div><div class="photos-edit-info">'.drupal_render($form[$key]['time']) . drupal_render($form[$key]['pid']) .'</div>'
    );
  }
  if(!$rows){
    $rows[] = array(array('data' => t('No image'), 'colspan' => '3'));
  }
	$output = theme('table', $header, $rows, array('id' => 'photos_album_edit'));
	return $output;
}
//设置封面
function photos_edit_cover($node, $fid){
  $pid = db_result(db_query('SELECT pid FROM {x_image} WHERE fid = %d', $fid));
  if($pid == $node->nid){
    db_query('UPDATE {x_album} SET fid = %d WHERE pid = %d', $fid, $node->nid);
    drupal_set_message(t('Cover successfully set.'));
    $goto = $_GET['destination'] ? $_GET['destination'] : "node/$node->nid/photos";
    return drupal_goto($goto);
  }else{
    drupal_not_found();
  }
}
//ajax更新图片信息
//2009/2/18 1:12
function photos_edit_update($fid){
  drupal_set_header('Content-Type: text/plain;');
  if($value = trim($_POST['value'])){
    switch(arg(4)){
      case 'filename':
        db_query("UPDATE {files} SET filename = '%s' WHERE fid = %d", $value, $fid);
        echo $value;
      break;
      case 'des':
        db_query("UPDATE {x_image} SET des = '%s' WHERE fid = %d", $value, $fid);
        echo check_markup($value, 0, FALSE);
      break;
      case 'del':
        echo photos_file_del($fid, 0, 1);
    }
  }
}
//ajax删除单张图片
//2009/2/24 22:27
function photos_edit_delete($fid){
  if($_GET['type'] != 'sub_album'){
    $v = photos_file_del($fid, 0, 1);
    if($_GET['pid'] && intval($_GET['pid']) == $_GET['pid']) photos_set_count('node_album', $_GET['pid']);
    if($_GET['uid'] && intval($_GET['uid']) == $_GET['uid']) photos_set_count('user_image', $_GET['uid']);
  }else{
    $v = db_query('DELETE FROM {x_node} WHERE fid = %d', $fid);
    if($_GET['nid'] && intval($_GET['nid']) == $_GET['nid']) photos_set_count('node_node', $_GET['nid']);
  }
  if($_GET['go'] == 1){
    drupal_set_header('Content-Type: text/plain;');
    echo $v;
  }else if($v){
    drupal_set_message(t('Delete success'));
    return drupal_goto();
  }else{
    drupal_set_message(t('Delete failure'));
    return drupal_goto('photos/image/'.$fid);
  }
}
//发送到子相册
//2009/2/28 1:03
function photos_edit_to_sub($fid){
  global $user;
  $content = drupal_get_form('_photos_to_sub', $fid);
  $content .= theme('pager', NULL, 50);
  print theme('photos_print', $content);
}
function _photos_to_sub(& $form_state, $fid){
  $select_type = _photos_select_sub_album();
  if($select_type[0]){
    $form['title']['#value'] = '<h2>'.t('Please select sub-album: ').'</h2>';
    $result = db_query('SELECT nid FROM {x_node} WHERE fid = %d', $fid);
    while ($sub = db_fetch_object($result)){
      $select_sub[] = $sub->nid;
    }
    if(!$select_sub[0]) $select_sub[0] = 0;
    $result = pager_query('SELECT nid, title FROM {node} WHERE type IN ('.implode(',', $select_type).') AND nid NOT IN ('.implode(',', $select_sub).')', 50, 0);
    $form['sub']['#tree'] = true;
    $true = false;
    while($node = db_fetch_object($result)){
      $form['sub'][$node->nid] = array(
        '#type' => 'checkbox',
        '#title' => l($node->title, 'node/'.$node->nid, array('attributes' => array('target' => '_blank'))),
       );
      $true = true;
    }
    if($true){
      $form['fid'] = array(
        '#type' => 'value',
        '#value' => $fid
      );
      $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Send confirmation'),
        '#submit' => array('_photos_to_sub_submit')
      );
    }else{
      $form['help']['#value'] = t('You do not have can be used to receive a subset of the album.');
    }
  }else{
    $form['help']['#value'] = t('Sub-album feature is not turned on.');
  }
  return $form;
}

function _photos_to_sub_submit($form, &$form_state){
  if(!$form_state['values']['fid']) return;
  foreach($form_state['values']['sub'] as $key => $sub){
    if($sub){
      $into[] = '('.$key.','.$form_state['values']['fid'].')';
      $nids[] = $key;
    }
  }
  if($into[0]){
    db_query('INSERT INTO {x_node} (nid, fid) VALUES '.implode(',', $into));
    foreach($nids as $nid){
      photos_set_count('node_node', $nid);
    }
    drupal_set_message(t('Successfully sent the %count sub-album', array('%count' => count($into))));
  }
}