<?php
// $Id: photos.admin.inc,v 1.1.2.4 2009/12/30 03:35:18 eastcn Exp $
drupal_add_js(
<<<EOT
$(function(){
  select_hide($("input[@name='photos_display_teaser_display'][@checked]").val(), '#photos-form-teaser');
  $("input[@name='photos_display_teaser_display']").click(function(){
    select_hide($(this).val(), '#photos-form-teaser');
  });
  select_hide($("input[@name='photos_display_page_display'][@checked]").val(), '#photos-form-page');
  $("input[@name='photos_display_page_display']").click(function(){
    select_hide($(this).val(), '#photos-form-page');
  });
})
EOT
,'inline');
function photos_admin_settings() {
	$form['basic'] = array(
		'#title' => t('Basic settings'),
		'#weight' => -5,
		'#type' => 'fieldset',
		'#collapsible' => TRUE,
	);
	$form['basic']['photos_access_photos'] = array(
		'#type' => 'radios',
		'#title' => t('privacy settings'),
		'#default_value' => variable_get('photos_access_photos', 0),
		'#description' => t('This option is enabled, users will be able to album set to confidentiality'). (!module_exists('photos_access') ? t('Please enable <a href="@url">photos_access</a> module',array('@url' => url('http://drupal.org/project/photos_access'))): ''),
		'#options' => array(t('Disabled'), t('Enabled')),
    '#required' => TRUE,
    '#disabled' => (module_exists('photos_access') ? false: true)
	);
	$form['basic']['photos_num'] = array(
		'#type' => 'select',
		'#title' => t('Display the number of form'),
		'#default_value' => variable_get('photos_num', 5),
    '#required' => TRUE,
		'#options' => drupal_map_assoc(array(1, 2, 3, 4, 5, 6, 7, 8, 9, 10)),
		'#description' => t('Upload through a form, permit the maximum number of'),
	);
	$form['basic']['num'] = array(
		'#title' => t('Number of albums'),
		'#weight' => 10,
		'#type' => 'fieldset',
    '#description' => t('The number of albums a user allowed to create. Administrater is not limited.'),
		'#collapsible' => TRUE,
	);
  $roles = user_roles(true);
  foreach($roles as $key => $role){
  	$form['basic']['num']['photos_pnum_'.$key] = array(
  		'#type' => 'textfield',
  		'#title' => $role,
      '#required' => TRUE,
  		'#default_value' => variable_get('photos_pnum_'.$key, 20),
      '#prefix' => '<div class="photos-admin-inline">',
      '#suffix' => '</div>',
      '#size' => 10
  	);
  }
	$form['basic']['photos_path'] = array(
		'#type'=> 'textfield',
		'#title' => t('Path of saving'),
		'#default_value' => variable_get('photos_path', 'photos/image_%uid'),
		'#description' => t('The path where the files should be saved, may save by user id or user name or time, e.g.: photos/%uid or photos/%username or image/%Y/%m/%d. Available variables: %uid, %username, %Y, %m, %d.'),
		'#size' => '40',
		'#required' => TRUE,
	);
	$form['basic']['photos_print_sizes'] = array(
		'#type' => 'radios',
		'#title' => t('How to display original image and all sizes?'),
		'#default_value' => variable_get('photos_print_sizes', 1),
		'#description' => t('Print rather than return. To avoid the theme layout been destroyed. Recommended selection "print"'),
    '#required' => TRUE,
		'#options' => array(t('return'), t('print')),
	);
  if(variable_get('comment_photos', COMMENT_NODE_READ_WRITE) == 2){
    $commenttrue = true;
  }
	$form['basic']['photos_comment'] = array(
		'#type' => 'radios',
		'#title' => t('Comment setting'),
		'#default_value' => variable_get('photos_comment', 0),
		'#description' => t('Open for the photo comments, you must first type of content in the overall set of options open comments.'),
    '#required' => TRUE,
		'#options' => array(t('Disabled'), t('Enabled')),
    '#disabled' => ($commenttrue ? false: true)
	);
	$form['basic']['photos_vote'] = array(
		'#type' => 'radios',
		'#title' => t('Images vote'),
		'#default_value' => variable_get('photos_vote', 0),
		'#description' => t('For the image to increase the voting feature, you must first install the votingapi.module.'),
    '#required' => TRUE,
		'#options' => array(t('Disabled'), t('Enabled')),
		'#disabled' => (module_exists('votingapi') ? false: true)
	);
	$form['basic']['photos_upzip'] = array(
		'#type' => 'radios',
		'#title' => t('zip compressed upload'),
		'#default_value' => variable_get('photos_upzip', 0),
		'#description' => t('Will be allowed to upload images compressed into a zip, extract the necessary support to open php'),
    '#required' => TRUE,
		'#options' => array(t('Disabled'), t('Enabled')),
    '#disabled' => ((version_compare(PHP_VERSION, '5') >= 0) ? false: true)
	);
	$form['basic']['photos_rname'] = array(
		'#type' => 'radios',
		'#title' => t('Rename image'),
		'#default_value' => variable_get('photos_rname', 0),
		'#description' => t('Rename uploaded image by random numbers, to solve problems with non-ASCII filenames such as Chinese.'),
    '#required' => TRUE,
		'#options' => array(t('Disabled'), t('Enabled')),
	);
  //thumb settings
  if($size = photos_upload_info(0)){
    $num = ($size['count'] + 3);
  }else{
    $num = 3;
    $size['size'] = array(
      array(
        'name' => '100x75',
        'w' => 100,
        'h' => 75,
        'r' => 'scale_and_crop'
      ),
      array(
        'name' => '320x240',
        'w' => 320,
        'h' => 240,
        'r' => 'scale'
      )
    );
  }
  $size['required'][0] = true;
	$form['photos_thumb_count'] = array(
		'#type'=> 'hidden',
    '#default_value' => $num,
	);
	$form['thumb'] = array(
		'#title' => t('Thumbnails settings'),
		'#weight' => -4,
		'#type' => 'fieldset',
    '#description' => t('Permit the use of the name of the characters: a-z, A-z ,0-9, e.g: "thumb", "100x75", "320x240". After confirmation, please do not change easily'),
		'#collapsible' => TRUE,
	);
  for ($i = 0; $i < $num; $i++){
  	$form['thumb']['photos_title_'.$i] = array(
  		'#type'=> 'textfield',
  		'#title' => t('Name'),
      '#default_value' => $size['size'][$i]['name'],
      '#size' => '10',
      '#required' => $size['required'][$i],
      '#prefix' => '<div class="photos-admin-inline">',
  	);
  	$form['thumb']['photos_size_'.$i] = array(
  		'#type'=> 'textfield',
  		'#title' => t('Size'),
  		'#default_value' => $size['size'][$i]['w'] ? $size['size'][$i]['w'].'x'.$size['size'][$i]['h']: null,
  		'#size' => '10',
      '#required' => $size['required'][$i],
  	);
  	$form['thumb']['photos_res_'.$i] = array(
  		'#type' => 'select',
  		'#title' => t('Format'),
      '#default_value' => $size['size'][$i]['r'],
      '#required' => $size['required'][$i],
  		'#options' => array('scale' => 'scale', 'crop' => 'scale_and_crop', 'resize' => 'resize'),
      '#suffix' => '</div>',
  	);
  }
	$form['thumb']['photos_thumb_create_time'] = array(
		'#type' => 'radios',
		'#title' => t('What time to create thumbnails'),
    '#default_value' => variable_get('photos_thumb_create_time', 0),
		'#options' => array(t('Immediately'), t('First visit')),
    '#required' => TRUE,
    '#description' => t('Album privacy settings must be enabled') . '<a href="#edit-photos-access-photos-1-wrapper">#</a>'
	);
  //thumb settings end
  //Display settings
  $form['display'] = array(
    '#title' => t('Display Settings'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
  );
  
	$form['display']['global'] = array(
		'#type' => 'fieldset',
		'#title' => t('Global Settings'),
		'#collapsible' => TRUE,
    '#description' => t('Albums basic display settings')
	);
	$form['display']['page'] = array(
		'#type' => 'fieldset',
		'#title' => t('Page Settings'),
		'#collapsible' => TRUE,
    '#description' => t('Page (e.g: node/[nid]) display settings'),
    '#prefix' => '<div id="photos-form-page">',
    '#suffix' => '</div>'
	);
	$form['display']['teaser'] = array(
		'#type' => 'fieldset',
		'#title' => t('Teaser Settings'),
		'#collapsible' => TRUE,
    '#description' => t('Teaser display settings'),
    '#prefix' => '<div id="photos-form-teaser">',
    '#suffix' => '</div>'
	);
  $form['display']['global']['photos_display_viewpager'] = array(
    '#type' => 'select',
    '#default_value' => variable_get('photos_display_viewpager', 10),
    '#title' => t('How many images show in each page?'),
    '#required' => TRUE,
    '#options' => drupal_map_assoc(array(5, 10, 15, 20, 25, 30, 35, 40, 45, 50))
  );
	$form['display']['global']['photos_display_imageorder'] = array(
		'#type' => 'select',
		'#title' => t('Image display order'),
    '#required' => TRUE,
		'#default_value' => variable_get('photos_display_imageorder', 'timestamp|desc'),
		'#options' => _photos_order_label()
	);
  switch($size['count']){
    case 1:
      $view_imagesize = $list_imagesize = variable_get('photos_title_0', 'thumb');
    break;
    case 2:
    case 3:
      $list_imagesize = variable_get('photos_title_1', false);
      $view_imagesize = variable_get('photos_title_1', false);
    break;
    default:
      $list_imagesize = variable_get('photos_title_2', false);
      $view_imagesize = variable_get('photos_title_2', false);
    break;
  }
	$form['display']['global']['photos_display_list_imagesize'] = array(
		'#type' => 'select',
		'#title' => t('Image display size(by list)'),
    '#required' => TRUE,
		'#default_value' => variable_get('photos_display_list_imagesize', $list_imagesize),
    '#description' => t('Displayed in the list(e.g: photos/album/[nid]) of image size.'),
		'#options' => _photos_select_size()
	);
	$form['display']['global']['photos_display_view_imagesize'] = array(
		'#type' => 'select',
		'#title' => t('Image display size(by page)'),
    '#required' => TRUE,
		'#default_value' => variable_get('photos_display_view_imagesize', $view_imagesize),
    '#description' => t('Displayed in the page(e.g: photos/image/[fid]) of image size.'),
		'#options' => _photos_select_size()
	);
  $form['display']['global']['photos_display_slide'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('photos_display_slide', '640x480'),
    '#title' => t('Slide width and height'),
    '#required' => TRUE,
    '#description' => t('The width and height of flash, e.g: 640x480.'),
  );
	$form['display']['global']['photos_display_user'] = array(
		'#type' => 'radios',
		'#title' => t('Create an album, when that allows users to modify this setting'),
		'#default_value' => variable_get('photos_display_user', 1),
		'#options' => array(t('Disabled'), t('Enabled(Recommend)'))
	);
  $form['display']['page']['photos_display_page_display'] = array(
    '#type' => 'radios',
    '#default_value' => variable_get('photos_display_page_display', 1),
    '#title' => t('Display setting'),
    '#required' => TRUE,
    '#options' => array(t('Do not display'), t('Display cover'), t('Slide show'), t('Display thumbnails'))
  );
  $form['display']['page']['photos_display_page_viewnum'] = array(
    '#type' => 'select',
    '#default_value' => variable_get('photos_display_page_viewnum', 10),
    '#title' => t('Display quantity'),
    '#required' => TRUE,
    '#options' => drupal_map_assoc(array(5, 10, 15, 20, 25, 30, 35, 40, 45, 50)),
    '#prefix' => '<div class="photos-form-count">'
  );
	$form['display']['page']['photos_display_page_imagesize'] = array(
		'#type' => 'select',
		'#title' => t('Image display size'),
    '#required' => TRUE,
		'#default_value' => variable_get('photos_display_page_imagesize', variable_get('photos_title_0', 'thumb')),
		'#options' => _photos_select_size(),
    '#suffix' => '</div>'
	);
  $form['display']['page']['photos_display_page_slide'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('photos_display_page_slide', '640x480'),
    '#title' => t('Slide width and height'),
    '#required' => TRUE,
    '#description' => t('The width and height of flash, e.g: 640x480.'),
    '#prefix' => '<div class="photos-form-slide">',
    '#suffix' => '</div>'
  );
	$form['display']['page']['photos_display_page_user'] = array(
		'#type' => 'radios',
		'#title' => t('Create an album, when that allows users to modify this setting'),
		'#default_value' => variable_get('photos_display_page_user', 1),
		'#options' => array(t('Disabled'), t('Enabled(Recommend)'))
	);
  $form['display']['teaser']['photos_display_teaser_display'] = array(
    '#type' => 'radios',
    '#default_value' => variable_get('photos_display_teaser_display', 1),
    '#title' => t('Display setting'),
    '#required' => TRUE,
    '#options' => array(t('Do not display'), t('Display cover'), t('Slide show'), t('Display thumbnails'))
  );
  $form['display']['teaser']['photos_display_teaser_viewnum'] = array(
    '#type' => 'select',
    '#default_value' => variable_get('photos_display_teaser_viewnum', 5),
    '#title' => t('Display quantity'),
    '#required' => TRUE,
    '#options' => drupal_map_assoc(array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10)),
    '#prefix' => '<div class="photos-form-count">'
  );
	$form['display']['teaser']['photos_display_teaser_imagesize'] = array(
		'#type' => 'select',
		'#title' => t('Image display size'),
    '#required' => TRUE,
		'#default_value' => variable_get('photos_display_teaser_imagesize', variable_get('photos_title_0', 'thumb')),
		'#options' => _photos_select_size(),
    '#suffix' => '</div>'
	);
  $form['display']['teaser']['photos_display_teaser_slide'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('photos_display_teaser_slide', '640x480'),
    '#title' => t('Slide width and height'),
    '#required' => TRUE,
    '#description' => t('The width and height of flash, e.g: 640x480.'),
    '#prefix' => '<div class="photos-form-slide">',
    '#suffix' => '</div>'
  );
	$form['display']['teaser']['photos_display_teaser_user'] = array(
		'#type' => 'radios',
		'#title' => t('Create an album, when that allows users to modify this setting'),
		'#default_value' => variable_get('photos_display_teaser_user', 1),
		'#options' => array(t('Disabled'), t('Enabled(Recommend)'))
	);
  //slideshow settings
	$form['slideshow'] = array(
		'#title' => t('Slideshow default settings'),
		'#type' => 'fieldset',
		'#collapsible' => TRUE,
	);
	$form['slideshow']['show_size'] = array(
		'#title' => t('Slide show image size'),
		'#type' => 'fieldset',
    '#weight' => 10,
		'#collapsible' => TRUE,
	);
	$form['slideshow']['photos_slide'] = array(
		'#type' => 'radios',
		'#title' => t('Slide show'),
		'#default_value' => variable_get('photos_slide', 1),
		'#description' => (!module_exists('dfgallery') ? t('Please enable <a href="@url">dfgallery</a> module',array('@url' => url('http://drupal.org/project/dfgallery'))): ''),
		'#options' => array(t('Disabled'), t('Enabled')),
    '#disabled' => (module_exists('dfgallery') ? false: true)
	);
	$form['slideshow']['photos_slide_zoom'] = array(
		'#type'=> 'textfield',
		'#title' => t('Slide default width and height'),
		'#default_value' => variable_get('photos_slide_zoom', '640x480'),
		'#description' => t('The width and height of flash, e.g: 640x480.'),
		'#size' => '20',
    '#disabled' => (module_exists('dfgallery') ? false: true)
	);
  $form['slideshow']['photos_slide_music'] = array(
    '#type' => 'textfield',
    '#default_value' => variable_get('photos_slide_music', false),
    '#title' => t('Slide default Music'),
    '#description' => t('e.g: http://mysite/music.mp3'),
  );
	$form['slideshow']['show_size']['photos_slide_show_thumb'] = array(
		'#type' => 'select',
		'#title' => t('Thumbnails'),
		'#default_value' => variable_get('photos_slide_show_thumb', variable_get('photos_title_0', 'thumb')),
		'#options' => _photos_select_size(),
    '#disabled' => (module_exists('dfgallery') ? false: true)
	);
	$form['slideshow']['show_size']['photos_slide_show_view'] = array(
		'#type' => 'select',
		'#title' => t('Medium image'),
		'#default_value' => variable_get('photos_slide_show_view', $view_imagesize),
		'#options' => _photos_select_size(),
    '#disabled' => (module_exists('dfgallery') ? false: true)
	);
  //slideshow settings end
  //count settings
	$form['count'] = array(
		'#title' => t('Statistics'),
		'#type' => 'fieldset',
		'#collapsible' => TRUE,
	);
	$form['count']['photos_image_count'] = array(
		'#type' => 'radios',
		'#title' => t('Count image views'),
		'#default_value' => variable_get('photos_image_count', 0),
		'#description' => t('Increment a counter each time image is viewed.'),
		'#options' => array( t('Enabled'), t('Disabled'))
	);
	$form['count']['photos_user_count_cron'] = array(
		'#type' => 'radios',
		'#title' => t('Image quantity statistics'),
		'#default_value' => variable_get('photos_user_count_cron', 1),
		'#description' => t('Users/Site images and albums quantity statistics.'),
		'#options' => array( t('When in the implementation of cron(affect the count update)'), t('When the image upload (affect the upload speed)'))
	);
  //count settings end
  //exif settings
  $form['exif'] = array(
    '#title' => t('Exif Settings'),
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
  );
	$form['exif']['photos_exif'] = array(
		'#type' => 'radios',
		'#title' => t('Show exif information'),
		'#default_value' => variable_get('photos_exif', 0),
		'#description' => t('When the image is available exif information, automatically read and display. Open this option, you must load the php exif.'),
		'#options' => array(t('Disabled'), t('Enabled')),
    '#disabled' => (extension_loaded('exif') ? false: true)
	);
	$form['exif']['photos_exif_cache'] = array(
		'#type' => 'radios',
		'#title' => t('Cache exif information'),
		'#default_value' => variable_get('photos_exif_cache', 1),
		'#description' => t('Exif information cache can improve access speed.'),
		'#options' => array(t('Do not cache'), t('To database')),
    '#disabled' => (extension_loaded('exif') ? false: true)
	);
	$form['exif']['photos_exif_help']['#value'] = t('Custom exif. Please modify function _photos_exif_tag, at this location: .../modules/photos/inc/photos.down.inc');
  //exif settings end
  //photos swfu settings
	$form['photos_swfu'] = array(
		'#title' => t('Photos swfu'),
		'#type' => 'fieldset',
    '#description' => t('SWFUpload could provide a better upload experience.'),
		'#collapsible' => TRUE,
	);
	$form['photos_swfu']['photos_swfu_num'] = array(
		'#type' => 'select',
		'#title' => t('Number of upload'),
		'#default_value' => variable_get('photos_swfu_num', 100),
		'#options' => drupal_map_assoc(array(1, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100)),
		'#description' => !module_exists('photos_swfu') ? t('Please enable photos_swfu module.'): t('Maxium amount allowed for one upload'),
    '#disabled' => (module_exists('photos_swfu') ? false: true)
	);
  //photos swfu settings end
  $form['#submit'][] = 'photos_admin_settings_submit';
  $form['#validate'][] = 'photos_admin_settings_validate';
	return system_settings_form($form);
}

function photos_admin_settings_validate($form, &$form_state) {
  $title = array();
  if($form_state['values']['photos_thumb_create_time'] && !$form_state['values']['photos_access_photos']){
    form_set_error('photos_thumb_create_time', t('Album privacy settings must be enabled: !url', array('!url' => '<a href="#edit-photos-thumb-create-time-0-wrapper">#</a>')));
    return;
  }
  for ($i = 0; $i < $form_state['values']['photos_thumb_count']; $i++){
    if($form_state['values']['photos_title_'.$i]){
      if(!$form_state['values']['photos_size_'.$i]){
        form_set_error('photos_size_'.$i, t('May not be empty'));
        return;
      }
      $array = array('%uid', '%username', '%Y', '%m', '%d');
      $topdir = current(explode('/', $form_state['values']['photos_path']));
      if(preg_match('/%uid|%username|%Y|%m|%d/i', $topdir)){
        form_set_error('photos_path', t('Variables(%uid, %username, %Y, %m, %d) can not be on the top floor'));
        return;
      }
      if(preg_match('/[^a-zA-z0-9]/i', $form_state['values']['photos_title_'.$i])){
        form_set_error('photos_title_'.$i, t('Allow only letters and numbers'));
        return;
      }
      if(preg_match('/[^0-9x]/i', $form_state['values']['photos_size_'.$i])){
        form_set_error('photos_size_'.$i, t('Formats such as: 100x100'));
        return;
      }
      if(in_array($form_state['values']['photos_title_'.$i], $title)){
        form_set_error('photos_title_'.$i, t('Not allowed to repeat'));
        return;
      }
      $title[] = $form_state['values']['photos_title_'.$i];
      if(strstr($form_state['values']['photos_title_'.$i], '_')){
        form_set_error('photos_title_'.$i, t('Not allowed to use "_"'));
        return;
      }
    }else if($form_state['values']['photos_size_'.$i]){
      form_set_error('photos_title_'.$i, t('May not be empty'));
      return;
    }
  }
}

function photos_admin_settings_submit($form, &$form_state) {
  global $user;
  for ($i = 0; $i < $form_state['values']['photos_thumb_count']; $i++){
    if($form_state['values']['photos_size_'.$i]){
      $size = explode('x', trim($form_state['values']['photos_size_'.$i]));
      if(is_numeric($size[0]) && is_numeric($size[0])){
        $t[] = array('w' => $size[0], 'h' =>$size[1], 'r' => $form_state['values']['photos_res_'.$i], 'name' => drupal_strtolower(trim($form_state['values']['photos_title_'.$i])));
      }
    }
  }
  variable_set('photos_size', $t);

  $filepath = file_directory_path() . '/' . current(explode('/', $form_state['values']['photos_path']));
	if (!file_check_directory($filepath, FILE_CREATE_DIRECTORY)) {
		return false;
	}
  if($form_state['values']['photos_access_photos']){
    if(!is_file($filepath. '/.htaccess')){
      drupal_set_message(t('%name creating successful', array('%name' => $filepath. '/.htaccess')));
      if ($handle = @fopen($filepath. '/.htaccess', 'wb')) {
        $str = '<FilesMatch "\.*$">Order allow,deny</FilesMatch>';
        if(fwrite($handle, $str)){
          fclose($handle);
        }
      }
    }
  }else if(is_file($filepath. '/.htaccess')){
    file_delete($filepath. '/.htaccess');
  }
  
  cache_clear_all('*', 'cache_menu', TRUE);
//  if($form_state['values']['photos_exif_cache'] == 1){
//    $filepath = $filepath . '/cache_exif';
//  	if (!file_check_directory($filepath, FILE_CREATE_DIRECTORY)) {
//  		return false;
//  	}
//    if(!is_file($filepath. '/.htaccess')){
//      drupal_set_message(t('%name creating successful', array('%name' => $filepath. '/.htaccess')));
//      if ($handle = @fopen($filepath. '/.htaccess', 'wb')) {
//        $str = '<FilesMatch "\.*$">Order allow,deny</FilesMatch>';
//        if(fwrite($handle, $str)){
//          fclose($handle);
//        }
//      }
//    }
//  }
}
function photos_admin_update(){
  if($_GET['count'] == true){
    _photos_res_count();
    drupal_set_message(t('Update is complete.'));
    return drupal_goto('admin/settings/photos');
  }
  $v = l(t('Thumbnail-size bulk update'), 'admin/settings/photos/update', array('query' => array('update' => 'true', 'limit' => 0))).'<p>'.t('Normally, you do not need to update it.').'</p>';
  if($_GET['update'] == 'true'){
    if(!ini_get('safe_mode') && function_exists('set_time_limit')){
      set_time_limit(0);
      $limit = 100;
    }else{
      $limit = 50;
    }
    $result = db_query_range('SELECT f.filepath, f.fid FROM {files} f INNER JOIN {x_image} i ON f.fid = i.fid ORDER BY f.fid ASC', $_GET['limit'], $limit);
    $i = 0;
    while ($file = db_fetch_object($result)) {
      photos_image_crop(array('path' => $file->filepath, 'fid' => $file->fid));
      $i++;
    };
    if($i){
      drupal_set_message(t('Update the %num images. Please wait, the next group is being updated ...', array('%num' => $i)));
      $url = url('admin/settings/photos/update', array('query' => array('update' => 'true', 'limit' => ($_GET['limit'] + $limit))));
drupal_add_js(<<<EOT
$(function(){
  location = "$url";
});
EOT
, 'inline');
    }else{
      drupal_set_message(t('Update is complete.'));
      drupal_goto('admin/settings/photos/update');
    }
  }
  return $v;
}

function photos_admin_import(& $form_state){
  $form['pid'] = array(
    '#title' => t('Upload to album'),
    '#type' => 'select',
    '#options' => _photos_useralbum_option(),
    '#required' => TRUE,
    '#prefix' => '<div id="photos-pid">',
    '#suffix' => '</div>',
  );
  $form['url'] = array(
    '#title' => t('Filepath'),
    '#type' => 'textfield',
    '#description' => t('From the server directory into zip files, to extract. You can upload images to a directory server, enter here path, you can extract and add to the album phtos. Path such as: !zip/test.zip. Documents must be read and write, php can operate.'),
    '#required' => TRUE,
  );
  $form['title'] = array(
    '#type'=> 'textfield',
    '#title' => t('Image title'),
  );
  $form['des'] = array(
    '#type'=> 'textarea',
    '#title' => t('Image description'),
    '#cols' => 40,
    '#rows' => 3,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Confirm import'),
  );
  $form['#validate'] = array('photos_admin_import_validate');
  return $form;
}

function photos_admin_import_validate($form, & $form_state){
  $value = new stdClass();
  $value->pid = $form_state['values']['pid'];
  $value->des = $form_state['values']['des'];
  $value->title = $form_state['values']['title'];
  if($msg = _photos_unzip($form_state['values']['url'], $value)){
    drupal_set_message($msg);
  }
}