<?php

/**
 * Admin main page.
 */
function redactorjs_admin() {

  $editors = redactorjs_editors('all');
  $header = array(t('Editor name'), t('Operations'));
  $rows = array();
  foreach ($editors as $eid => $editor) {
    $rows[] = array($editor['name'], l(t('Edit'), 'admin/settings/redactorjs/' . $eid) . ' | ' . l(t('Delete'), 'admin/settings/redactorjs/' . $eid . '/delete', array('attributes' => array('class' => 'eop-delete'))) . ' | ' . l(t('Copy'), 'admin/settings/redactorjs', array('attributes' => array('class' => 'eop-copy', 'name' => 'Copy of ' . $editor->name), 'query' => array('eop' => 'copy', 'eid' => $eid, 'token' => $token))) . ' | ' . l(t('Export'), 'admin/settings/redactorjs', array('attributes' => array('class' => 'eop-export'), 'query' => array('eop' => 'export', 'eid' => $eid))));
  }
  $rows[] = array(array('data' => l(t('Add new editor'), 'admin/settings/redactorjs/new') . ' | ' . l(t('Add the default editor'), 'admin/settings/redactorjs', array('attributes' => array('class' => 'eop-add-default', 'name' => 'default editor'), 'query' => array('eop' => 'add_default', 'token' => $token))), 'colspan' => 2));

  $output = '<h2 class="title">' . t('Available editors') . '</h2>';

  $output .= theme('table', $header, $rows, array('id' => 'available-editors-list'));
  $output .= drupal_get_form('redactorjs_admin_form');
  return $output;
}

function redactorjs_admin_form() {
  $form = array();

  $form['roles'] = array('#tree' => TRUE);
  $roles = redactorjs_sorted_roles();
  $form['#weighted'] = count($roles) > 3;
  //user#1
  if ($GLOBALS['user']->uid == 1) {
    $u1 = array('name' => t('user #1'), 'weight' => t('n/a'), 'editor' => variable_get('redactorjs_user1', 1), 'alt' => variable_get('redactorjs_user1_alt', 0));
    $form['roles']['u1'] = redactorjs_role_form($u1, $form['#weighted'], 1);
  }
  //other roles
  foreach ($roles as $rid => $role) {
    $core = $rid == DRUPAL_ANONYMOUS_RID || $rid == DRUPAL_AUTHENTICATED_RID;
    $form['roles'][$rid] = redactorjs_role_form($role, $form['#weighted'], $core);
  }

  $form['base'] = array(
    '#type' => 'fieldset',
    '#title' => t('Base settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  // Base settings
  $form['base']['min_js'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use redactor.min.js'),
  );

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save configuration'));
  $form['#submit'][] = 'redactorjs_admin_submit';
  $form['#theme'] = 'redactorjs_admin';
  return $form;
}

/**
 * Admin form theming.
 */
function redactorjs_admin_theme($form = array()) {
  $header = array(t('User role'), t('Assigned editor'), t('Alternative editor'));
  $keys = array('name', 'editor', 'alt');
  $info = '';
  if ($form['#weighted']) {
    $header[] = t('Weight');
    $keys[] = 'weight';
    $info = '<br />' . t('For users who have <strong>multiple roles</strong>, the <strong>weight</strong> property will determine the assigned editor. Lighter roles that are placed upper will take the precedence. So, an administrator role should be placed over other roles by having a smaller weight, ie. -10.');
  }

  $rows = array();
  foreach (element_children($form['roles']) as $rid) {
    $cells = array();
    foreach ($keys as $key) {
      $cells[] = drupal_render($form['roles'][$rid][$key]);
    }
    $rows[] = $cells;
  }

  $output = '<h2 class="title">' . t('Role-editor assignments') . '</h2>';
  $output .= theme('table', $header, $rows, array('id' => 'roles-editors-list'));
  $output .= '<div class="form-item"><div class="description">' . t('Assign editors to user roles.') . '<br />' . t('Alternative editor makes it possible to use different editors for different textareas or different editors on diffrent pages. You just have to configure visibility settings for each editor.') . $info . '</div></div>';
  $output .= drupal_render($form);
  return $output;
}

/**
 * Role-editor form
 */
function redactorjs_role_form($role, $weight = TRUE, $core = TRUE) {
  $form['name'] = array(
    '#type' => 'markup',
    '#value' => $role['name'],
  );
  if ($weight) {
    $form['weight'] = $core ? array(
      '#type' => 'markup',
      '#value' => $role['weight'],
      ) : array(
      '#type' => 'weight',
      '#default_value' => $role['weight'],
      );
  }
  $form['editor'] = array(
    '#type' => 'select',
    '#options' => redactorjs_editor_options(),
    '#default_value' => $role['editor'],
  );
  $form['alt'] = array(
    '#type' => 'select',
    '#options' => redactorjs_editor_options(),
    '#default_value' => $role['alt'],
  );
  return $form;
}

/**
 * Editor options.
 */
function redactorjs_editor_options() {
  static $options;
  if (!isset($options)) {
    $options = array(0 => t('none'));
    foreach (redactorjs_editors('all') as $eid => $editor) {
      $options[$eid] = $editor['name'];
    }
  }
  return $options;
}

/**
 * Sort roles according to their weights.
 */
function redactorjs_sorted_roles() {
  static $sorted;
  if (!isset($sorted)) {
    $sorted = array();
    $roles = user_roles();
    $broles = variable_get('redactorjs_roles', array());
    $broles[DRUPAL_ANONYMOUS_RID]['weight'] = 12;
    $broles[DRUPAL_AUTHENTICATED_RID]['weight'] = 11;
    $keys = array('weight', 'editor', 'alt');
    foreach ($roles as $rid => $name) {
      $sorted[$rid]['name'] = $name;
      foreach ($keys as $key) {
        $sorted[$rid][$key] = empty($broles[$rid][$key]) ? 0 : (int) $broles[$rid][$key];
      }
    }
    uasort($sorted, 'redactorjs_rolesort');
  }
  return $sorted;
}

/**
 * user sorting function for roles.
 */
function redactorjs_rolesort($r1, $r2) {
  return $r1['weight'] - $r2['weight'];
}

function redactorjs_editor_form($form_state, $eid = NULL) {
  $default_editor = redactorjs_editor_defaults();

  if ($eid == 'new' || $eid === NULL) {
    $editor = $default_editor;
  }
  else {
    $editor = redactorjs_editors($eid);
    $editor += unserialize($editor['options']);

    unset($editor['options']);

    $editor = $editor + $default_editor;
  }

  $form = array(
    '#cache' => TRUE,
  );

  $form['editor'] = array(
    '#type' => 'fieldset',
    '#title' => t('Editor'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['editor']['eid'] = array(
    '#type' => 'hidden',
    '#value' => $editor['eid'],
  );

  $form['visibility'] = array(
    '#type' => 'fieldset',
    '#title' => t('Visibility settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['editor']['name'] = array('#type' => 'textfield',
    '#title' => t('Editor name'),
    '#maxlength' => 255,
    '#default_value' => $editor['name'],
    '#required' => TRUE,
  );

  $form['visibility']['pages'] = array('#type' => 'textarea',
    '#title' => t('Show the editor on specific pages'),
    '#default_value' => $editor['pages'],
    '#description' => t('Enter one page per line as Drupal paths. The * character is a wildcard.'),
  );

  $form['visibility']['excludes'] = array('#type' => 'textarea',
    '#title' => t('Hide the editor for specific textareas'),
    '#default_value' => $editor['excludes'],
    '#description' => t('Enter one textarea ID per line. The * character is a wildcard.'),
  );

  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['language'] = array(
    '#type' => 'fieldset',
    '#title' => t('Language options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['toolbar'] = array(
    '#type' => 'fieldset',
    '#title' => t('Toolbar settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['toolbar']['buttons'] = array(
    '#type' => 'textarea',
    '#title' => t('Buttons'),
    '#default_value' => $editor['buttons'],
    '#description' => t("By default, this setting contains the following array of toolbar's buttons:
      <pre>
      ['html', '|', 'formatting', '|', 'bold', 'italic', 'deleted', '|',
'unorderedlist', 'orderedlist', 'outdent', 'indent', '|',
'image', 'video', 'file', 'table', 'link', '|',
'fontcolor', 'backcolor', '|', 'alignment', '|', 'horizontalrule']

// additional buttons
// 'underline', 'alignleft', 'aligncenter', 'alignright', 'justify'
      </pre>"),
  );

  $form['toolbar']['formattingTags'] = array(
    '#type' => 'textarea',
    '#title' => t('Formatting Tags'),
    '#default_value' => $editor['formattingTags'],
    //'#description' => t('By default, this setting contains the following array of toolbar's buttons:'),
  );

  $form['toolbar']['colors'] = array(
    '#type' => 'textarea',
    '#title' => t('Colors'),
    '#default_value' => $editor['colors'],
    //'#description' => t('By default, this setting contains the following array of toolbar's buttons:'),
  );

  $form['image'] = array(
    '#type' => 'fieldset',
    '#title' => t('Image settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['file'] = array(
    '#type' => 'fieldset',
    '#title' => t('File settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['language']['lang'] = array(
    '#type' => 'select',
    '#title' => t('Language'),
    '#options' => redactorjs_lang_options(),
    '#default_value' => $editor['lang'],
    '#description' => 'To setup a new language in Redactor you should do two things: <ul><li><a href="http://imperavi.com/redactor/docs/languages/">Download your language</a>.</li> <li>Put language file into /sites/all/modules/redactorjs/lang/</li></ul>',
  );

  $form['language']['direction'] = array(
    '#type' => 'select',
    '#title' => t('Direction'),
    '#options' => array(
      'auto' => t('Auto detect'),
      'ltr' => 'ltr',
      'rtl' => 'rtl',
    ),
    '#default_value' => $editor['direction'],
    '#description' => '',
  );

  $form['options']['source'] = array(
    '#type' => 'checkbox',
    '#title' => t('Source'),
    '#description' => 'Show/hide the HTML source button on the toolbar.',
    '#default_value' => $editor['source'],
  );

  $form['options']['focus'] = array(
    '#type' => 'checkbox',
    '#title' => t('Focus'),
    '#description' => 'This option allows you to set whether Redactor gets cursor focus on load or not.',
    '#default_value' => $editor['focus'],
  );

  $form['options']['shortcuts'] = array(
    '#type' => 'checkbox',
    '#title' => t('Shortcuts'),
    '#description' => 'Turns on/off keydown / keyup shortcuts functionality.',
    '#default_value' => $editor['shortcuts'],
  );

  $form['options']['autoresize'] = array(
    '#type' => 'checkbox',
    '#title' => t('Autoresize'),
    '#description' => 'This option turns on height autoresizing, which depends on the amount of text inputted into the text layer.',
    '#default_value' => $editor['autoresize'],
  );

  $form['options']['cleanup'] = array(
    '#type' => 'checkbox',
    '#title' => t('Cleanup'),
    '#description' => 'Turns on/off a text\'s cleanup on paste.',
    '#default_value' => $editor['cleanup'],
  );

  $form['options']['fixed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Fixed'),
    '#description' => 'If this option is turned on, Redactor\'s toolbar will remain at the top of the browser\'s window at all times.',
    '#default_value' => $editor['fixed'],
  );

  $form['options']['convertLinks'] = array(
    '#type' => 'checkbox',
    '#title' => t('Convert Links'),
    '#description' => 'With this option turned on, Redactor will automatically replace URLs with hyperlinks.',
    '#default_value' => $editor['convertLinks'],
  );

  $form['options']['convertDivs'] = array(
    '#type' => 'checkbox',
    '#title' => t('Convert Divs'),
    '#description' => 'With this option turned on, Redactor will automatically replace divs to paragraphs.',
    '#default_value' => $editor['convertDivs'],
  );

  $form['options']['overlay'] = array(
    '#type' => 'checkbox',
    '#title' => t('Overlay'),
    '#description' => 'Turns on and off overlay for modal Redactor\'s windows.',
    '#default_value' => $editor['overlay'],
  );

  $form['options']['air'] = array(
    '#type' => 'checkbox',
    '#title' => t('Air mode'),
    '#description' => 'Use air mode.',
    '#default_value' => $editor['air'],
  );

  $form['options']['wym'] = array(
    '#type' => 'checkbox',
    '#title' => t('Visual structure'),
    '#description' => 'Turns on a visual structure mode.',
    '#default_value' => $editor['wym'],
  );

  $form['options']['mobile'] = array(
    '#type' => 'checkbox',
    '#title' => t('Mobile'),
    '#description' => 'This setting turns on/off the editor for mobile.',
    '#default_value' => $editor['mobile'],
  );

  $form['options']['drupal_break'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add drupal teaser break button'),
    '#default_value' => 1,
    '#default_value' => $editor['drupal_break'],
  );

  $form['image']['observeImages'] = array(
    '#type' => 'checkbox',
    '#title' => t('Observe Images'),
    '#description' => 'Enable or disable by clicking on the image to display a modal window image settings.',
    '#default_value' => $editor['observeImages'],
  );

  $form['image']['imageUpload'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable image upload'),
    '#default_value' => $editor['imageUpload'],
    '#return_value' => '/redactorjs/upload/image',
  );

  $form['image']['imageGetJson'] = array(
    '#type' => 'checkbox',
    '#title' => t('View uploaded files'),
    '#default_value' => $editor['imageGetJson'],
    '#return_value' => '/redactorjs/upload/image/json',
  );

  $form['file']['fileUpload'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable file upload'),
    '#default_value' => $editor['fileUpload'],
    '#return_value' => '/redactorjs/upload/file',
  );

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save configuration'));

  return $form;
}

function redactorjs_lang_options() {
  $base_path = drupal_get_path('module', 'redactorjs') . '/lang/';

  $handle = opendir($base_path);
  $langs['auto'] = t('Auto detect');
  $langs['en'] = 'en';
  while ($file = readdir($handle)) {
    if (is_file($base_path . '/' . $file)) {
      $lang_name = basename($file, '.js');
      $langs[$lang_name] = $lang_name;
    }
  }
  natsort($langs);
  closedir($handle);

  variable_set('redactorjs_langs', $langs);

  return $langs;
}

function redactorjs_editor_defaults() {
  $editor['eid'] = NULL;
  $editor['name'] = "";
  $editor['pages'] = "node/*\ncomment/*";
  $editor['excludes'] = "edit-log";
  $editor['source'] = 1;
  $editor['focus'] = 0;
  $editor['shortcuts'] = 1;
  $editor['autoresize'] = 1;
  $editor['cleanup'] = 1;
  $editor['fixed'] = 0;
  $editor['convertLinks'] = 1;
  $editor['convertDivs'] = 1;
  $editor['overlay'] = 1;
  $editor['air'] = 0;
  $editor['wym'] = 0;
  $editor['mobile'] = 1;
  $editor['drupal_break'] = 1;
  $editor['observeImages'] = 1;
  $editor['imageUpload'] = 0;
  $editor['imageGetJson'] = 0;
  $editor['fileUpload'] = 0;
  $editor['lang'] = 'auto';
  $editor['direction'] = 'auto';
  $editor['buttons'] = 'html,|,formatting,|,bold,italic,deleted,|,unorderedlist,orderedlist,outdent,indent,|,image,video,file,table,link,|,fontcolor,backcolor,|,alignment,|,horizontalrule';
  $editor['formattingTags'] = 'p,blockquote,pre,h1,h2,h3,h4';
  $editor['colors'] = '#ffffff,#000000,#eeece1,#1f497d,#4f81bd,#c0504d,#9bbb59,#8064a2,#4bacc6,#f79646,#ffff00,#f2f2f2,#7f7f7f,#ddd9c3,#c6d9f0,#dbe5f1,#f2dcdb,#ebf1dd,#e5e0ec,#dbeef3,#fdeada,#fff2ca,#d8d8d8,#595959,#c4bd97,#8db3e2,#b8cce4,#e5b9b7,#d7e3bc,#ccc1d9,#b7dde8,#fbd5b5,#ffe694,#bfbfbf,#3f3f3f,#938953,#548dd4,#95b3d7,#d99694,#c3d69b,#b2a2c7,#b7dde8,#fac08f,#f2c314,#a5a5a5,#262626,#494429,#17365d,#366092,#953734,#76923c,#5f497a,#92cddc,#e36c09,#c09100,#7f7f7f,#0c0c0c,#1d1b10,#0f243e,#244061,#632423,#4f6128,#3f3151,#31859b,#974806,#7f6000';
  return $editor;
}

function redactorjs_editor_form_submit($form, &$form_state) {
  $default_editor = redactorjs_editor_defaults();
  $editor = $form_state['values'];

  $redactor = array(
    'eid' => isset($editor['eid']) ? $editor['eid'] : NULL,
    'name' => $editor['name'],
    'pages' => $editor['pages'],
    'excludes' => $editor['excludes'],
  );

  unset($editor['op'], $editor['submit'], $editor['form_build_id'], $editor['form_token'], $editor['form_id'], $editor['eid'], $editor['name'], $editor['pages'], $editor['excludes']);
  $diff_settings = array_diff_assoc($editor, $default_editor);
  $redactor['options'] = serialize(array_map('_redactorjs_to_settings', $diff_settings));

  if ($redactor['eid']) {
    drupal_write_record('redactorjs_editors', $redactor, array('eid'));
  }
  else {
    drupal_write_record('redactorjs_editors', $redactor);
  }
}

function _redactorjs_to_settings($item) {
  if (is_string($item)) {
    return $item;
  }
  return $item == 1 ? true : false;
}

function redactorjs_admin_submit($form, &$form_state) {
  $roles = $form_state['values']['roles'];
  $roles[DRUPAL_ANONYMOUS_RID]['weight'] = 12;
  $roles[DRUPAL_AUTHENTICATED_RID]['weight'] = 11;
  if ($GLOBALS['user']->uid == 1 && isset($roles['u1'])) {
    variable_set('redactorjs_user1', $roles['u1']['editor']);
    variable_set('redactorjs_user1_alt', $roles['u1']['alt']);
    unset($roles['u1']);
  }
  if (count($roles) > 3) {
    uasort($roles, 'redactorjs_rolesort');
  }
  variable_set('redactorjs_roles', $roles);

  drupal_set_message(t('The changes have been saved.'));
}