<?php
// $Id: photos.down.inc,v 1.1.2.1 2009/03/06 08:22:41 eastcn Exp $

function photos_down_page($fid){
  $image = db_fetch_array(db_query('SELECT f.filepath, f.filemime, f.timestamp, f.filename, u.uid, u.name, p.* FROM {files} f INNER JOIN {x_image} p ON f.fid = p.fid INNER JOIN {users} u ON f.uid = u.uid WHERE p.fid = %d', $fid));
  drupal_set_title(t('Sharing code and download image: @title', array('@title' => $image['filename'])));
  $content['back'] = l(t('Back to the image page'), 'photos/image/'.$fid);
  $content['info'] = t('Uploaded on !time by !name', array('!name' => theme('username', (object)$image), '!time' => format_date($image['timestamp'], 'small')));
  if(arg(3) != 'exif' && arg(3) != 'vote'){
    $image = photos_get_info(0, $image);
    $content['sizes'] = _photos_down_count($image['filepath'], $fid);
    $content['link']['original'] = user_access('view original') ? l(t('Original'), "photos/zoom/$fid/original") : NULL;
    $op['class'] = 'active';
  }
  if($image['exif'] && variable_get('photos_exif', 0) && ($exif = _photos_down_exif($image['filepath'], $fid))){
    $content['link']['exif'] = l(t('Exif'), "photos/zoom/$fid/exif");
  }
  if(variable_get('photos_vote', 0) && user_access('view vote list')){
    $content['link']['vote'] = l(t('Vote list'), "photos/zoom/$fid/vote");
  }
  switch(arg(3)){
    case 'thumb':
      if($label = _photos_down_label(arg(4))){
        $content['view'] = theme('photos_imagehtml', $image['thumb'][$label], array('filename' => $image['filename']));
        $content['sharing_url'] = _photos_l($image['thumb'][$label]);
        $content['sharing_link'] = _photos_l('photos/image/'.$fid);
        $content['sharing_title'] = $image['filename'];
      }else{
        $content['view'] = t('Without this size image');
      }
    break;
    case 'original':
      $content['view'] = theme('photos_imagehtml', $image['original'], array('filename' => $image['filename']));
    break;
    case 'exif':
      $content['view'] = $exif ? $exif : t('This image there is no Exif information.');
    break;
    case 'vote':
			if(!user_access('view vote list')){
				drupal_access_denied();
        return;
			}
      $content['view'] = _photos_down_vote($fid);
    break;
    default:
      if($content['sizes'][0]['name']){
        $sizes = end($content['sizes']);
        $content['view'] = theme('photos_imagehtml', $image['thumb'][$sizes['name']], array('filename' => $image['filename']));
        $content['sharing_url'] = _photos_l($image['thumb'][$sizes['name']]);
      }
      $content['sharing_link'] = _photos_l('photos/image/'.$fid);
      $content['sharing_title'] = $image['filename'];
  }
  $content['link']['sizes'] = l(t('All sizes'), 'photos/zoom/'.$fid, array('attributes' => $op));
  if(variable_get('photos_print_sizes', 1)){
    print theme('photos_down', $content, 'print');
  }else{
    return theme('photos_down', $content, 'return');
  }
}

function _photos_down_exif($filepath, $fid, $clear = 0){
  $cid = 'exif_'.$fid;
  if($data = cache_get($cid, 'cache_photos')){
    return theme('photos_exif', $data->data);
  }else if($exif = @exif_read_data($filepath, 0, true)){
    unset($data);
    $tags = _photos_exif_tag();
    foreach($tags as $key => $tag){
      if(!is_array($tag)){
        if($exif[$tag]){
          $data[$tag] = $exif[$tag];
          print_r($exif[$tag]);
        }
      }else{
        foreach($tag as $label){
          if($exif[$key][$label]){
            $data[$key][$label] = $exif[$key][$label];;
          }
        }
      }
    }
    if($data){
      cache_set($cid, $data, 'cache_photos');
      return theme('photos_exif', $data);
    }else{
      db_query('UPDATE {x_image} SET exif = 0 WHERE fid = %d', $fid);
    }
  }
}

function _photos_down_count($filepath, $fid){
  $image = image_get_info($filepath);
  $count = 0;
  if($size = photos_upload_info()){
    foreach($size['size'] as $v){
      if($v['w'] < $image['width'] || $v['h'] < $image['height']){
        $count++;
      }
    }
  }
  $t = photos_upload_info(0);
  for($i = $a = 0; $i < $count; $i++, $a++){
    if($_GET['q'] == "photos/zoom/$fid" && $i == ($count - 1)) {
      $op['class'] = 'active';
    }
    $c[] = array(
      'name' => $t['size'][$i]['name'],
      'link' => l($t['size'][$i]['name'], "photos/zoom/$fid/thumb/".$t['size'][$i]['name'], array('attributes' => $op)),
      'size' => $t['size'][$i]['w'].'x'.$t['size'][$i]['h']
    );
  }
  return $c;
}

function _photos_down_label($label){
  $t = photos_upload_info();
  foreach($t['size'] as $v){
    if($v['name'] == $label){
      return $label;
    }
  }
}

function _photos_down_vote($fid){
	$header = array(
		array('data' => t('Vote user'), 'field'=> 'v.uid'),
		array('data' => t('Vote result'), 'field'=> 'v.value'),
		array('data' => t('Vote time'), 'field'=> 'v.timestamp', 'sort' => 'desc'),
	);
	$rows = array();
	$result = pager_query("SELECT v.uid, u.name, v.value, v.timestamp, v.vote_source FROM {votingapi_vote} v LEFT JOIN {users} u ON u.uid = v.uid WHERE content_type = '%s' AND content_id = %d" .tablesort_sql($header), 30, 0, NULL, 'image', $fid);
	while ($a = db_fetch_object($result)){
		if($a->uid != 0){
			$name = theme('username', $a);
		}else{
			$name = $a->vote_source;
		}
		$rows[] = array($name, $a->value, format_date($a->timestamp,'small'));
	}
	if (empty($rows)) {
		$rows[] = array(array('data' => t('No vote available.'), 'colspan' => 3));
	}
	$output .= theme('table', $header, $rows);
	$output .= theme('pager', NULL, 30, 0);
	drupal_set_title(t('See a image vote'));
  return $output;
}
//Specify you want to save the exif field
/*
  $tag = array(
    'FILE' => array(
      'FileName',
      'FileDateTime',
      'FileSize',
      'FileType',
      'MimeType',
      'SectionsFound',
    ),
    'COMPUTED' => array(
      'html',
      'Height',
      'Width',
      'IsColor',
      'ByteOrderMotorola',
      'ApertureFNumber',
      'Thumbnail.FileType',
      'Thumbnail.MimeType',
    ),
    'IFD0' => array(
      'Make',
      'Model',
      'Orientation',
      'XResolution',
      'YResolution',
      'ResolutionUnit',
      'YCbCrPositioning',
      'Exif_IFD_Pointer',
      'GPS_IFD_Pointer',
      'UndefinedTag:0xEA1C',
    ),
    'THUMBNAIL' => array(
      'Compression',
      'XResolution',
      'YResolution',
      'ResolutionUnit',
      'JPEGInterchangeFormat',
      'JPEGInterchangeFormatLength',
    ),
    'EXIF' => array(
      'FNumber',
      'ExifVersion',
      'DateTimeOriginal',
      'DateTimeDigitized',
      'ComponentsConfiguration',
      'ApertureValue',
      'LightSource',
      'Flash',
      'FocalLength',
      'FlashPixVersion',
      'ColorSpace',
      'ExifImageWidth',
      'ExifImageLength',
      'CustomRendered',
      'ExposureMode',
      'WhiteBalance',
      'DigitalZoomRatio',
      'SceneCaptureType',
      'UndefinedTag:0xEA1C',
    ),
    'GPS' => array(
      'GPSVersion',
      'GPSLatitudeRef',
      'GPSLatitude',
      'GPSLongitudeRef',
      'GPSLongitude',
      'GPSAltitudeRef',
      'GPSAltitude',
      'GPSTimeStamp',
      'GPSSatellites',
      'GPSMeasureMode',
      'GPSDOP',
      'GPSSpeedRef',
      'GPSSpeed',
      'GPSTrackRef',
      'GPSTrack',
      'GPSMapDatum',
      'GPSDateStamp',
    ),
    'WINXP' => array(
      'Title',
      'Comment',
      'Author',
      'Keywords',
      'Subject'
    ),
  );
*/
function _photos_exif_tag(){
  $tags = array(
    'IFD0' => array(
      'Make',
      'Model',
      'Orientation',
      'XResolution',
      'YResolution',
      'ResolutionUnit',
      'YCbCrPositioning',
      'Exif_IFD_Pointer',
      'GPS_IFD_Pointer',
    ),
    'EXIF' => array(
      'FNumber',
      'ExifVersion',
      'DateTimeOriginal',
      'DateTimeDigitized',
      'ComponentsConfiguration',
      'ApertureValue',
      'LightSource',
      'Flash',
      'FocalLength',
      'FlashPixVersion',
      'ColorSpace',
      'ExifImageWidth',
      'ExifImageLength',
      'CustomRendered',
      'ExposureMode',
      'WhiteBalance',
      'DigitalZoomRatio',
      'SceneCaptureType',
    ),
    'GPS' => array(
      'GPSVersion',
      'GPSLatitudeRef',
      'GPSLatitude',
      'GPSLongitudeRef',
      'GPSLongitude',
      'GPSAltitudeRef',
      'GPSAltitude',
      'GPSTimeStamp',
      'GPSSatellites',
      'GPSMeasureMode',
      'GPSDOP',
      'GPSSpeedRef',
      'GPSSpeed',
      'GPSTrackRef',
      'GPSTrack',
      'GPSMapDatum',
      'GPSDateStamp',
    ),
  );
  return $tags;
}