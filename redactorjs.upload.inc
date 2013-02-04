<?php

function redactorjs_upload_image() {
  global $user;
  $errors = array();
  $validators = array();

  $path = file_directory_path() . '/userfiles/';
  file_check_directory($path, FILE_CREATE_DIRECTORY);
  $path = file_directory_path() . '/userfiles/' . $user->uid;
  file_check_directory($path, FILE_CREATE_DIRECTORY);

  $_FILES['files']['name']['file'] = $_FILES['file']['name'];
  $_FILES['files']['tmp_name']['file'] = $_FILES['file']['tmp_name'];
  $_FILES['files']['type']['file'] = $_FILES['file']['type'];
  $_FILES['files']['error']['file'] = $_FILES['file']['error'];
  $_FILES['files']['error']['size'] = $_FILES['file']['size'];

  $file = file_save_upload('file', $validators, $path);
  $output['file'] = $_FILES['file'];
/*
  if (!$file) {
    $output['error'][] = $_FILES['files'];
  }
  else {
    $output['error'][] = file_validate_is_image($file);
    $output['error'][] = file_validate_size($file, 2097000);
  }
*/
  if (count($errors) == 0) {
    file_set_status($file, FILE_STATUS_PERMANENT);
    $output['filelink'] = '/'.$file->filepath;
    $output['link'] = TRUE;
    $output['status'] = TRUE;
  }

  print drupal_to_js($output);
  exit;
}