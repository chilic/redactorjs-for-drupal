<?php

function redactorjs_upload($type) {
  global $user;
  global $base_path;
  $maxWidth = 535;

  $output = array();

  $validators = array(
    'file_validate_is_image' => array(),
    'file_validate_size' => array(),
  );

  // @todo Привязать к token
  $destination = file_directory_path() . '/userfiles/';
  file_check_directory($destination, FILE_CREATE_DIRECTORY);
  $destination = file_directory_path() . '/userfiles/' . $user->uid;
  file_check_directory($destination, FILE_CREATE_DIRECTORY);

  $file = file_save_upload('file', $validators, $destination);

  if ($file) {
    //file_set_status($file, FILE_STATUS_PERMANENT);
    // Output relative path, for absolute use $base_url
    $imageInfo = image_get_info($file->filepath);

    if ($imageInfo['width'] > $maxWidth) {
      $img = theme('imagecache', 'node_image', $file->filepath, $file->name, $file->name);
      $output['filelink'] = l($img, $file->filepath, array('html' => TRUE, 'attributes' => array('rel' => 'lightbox')));
      $output['link'] = true;
    }
    else {
      $output['filelink'] = $base_path . $file->filepath;
      $output['link'] = false;
    }

    // Add file to cahce
    // 6 hours cache life time for forms should be plenty.
    $expire = 21600;

    $cid = 'redactorjs:' . check_plain($_POST['form_build_id']) . ':' . $user->uid;
    if ($cache = cache_get($cid)) {
      $files = $cache->data;
    }
    $files[check_plain($_POST['field_name'])][$file->fid] = $output['filelink'];
    cache_set($cid, $files, 'cache', time() + $expire);
  }
  else {
    $messages = drupal_get_messages('error');
    $output['error'] = $messages['error'][0];
  }

  // @todo сделать привзяку к материалам
  drupal_json($output);
  exit();
}