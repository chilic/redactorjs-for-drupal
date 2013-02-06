<?php

function redactorjs_upload($type) {
  global $user;
  global $base_path;

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
    //if (file_copy($file, $destination, FILE_EXISTS_REPLACE)) {
      file_set_status($file, FILE_STATUS_PERMANENT);

      // Output relative path, for absolute use $base_url
      $output['filelink'] = $base_path . $file->filepath;
    //}
  }
  else {
    $messages = drupal_get_messages('error');
    $output['error'] = $messages['error'][0];
  }

  // @todo сделать привзяку к материалам

  drupal_json($output);
  //echo stripslashes(json_encode($output));
  exit();
}