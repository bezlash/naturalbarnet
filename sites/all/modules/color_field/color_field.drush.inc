<?php

/**
 * @file
 * Drush integration for color fields plugin.
 */

/**
 * The color plugin URI.
 */
define('COLOR_FIELD_SIMPLE_COLOR_PICKER_DOWNLOAD_URI', 'http://recursive-design.com/projects/jquery-simple-color/jquery-simple-color.1.0.1.zip');
define('COLOR_FIELD_EYECON_COLOR_PICKER_DOWNLOAD_URI', 'http://www.eyecon.ro/colorpicker/colorpicker.zip');
define('COLOR_FIELD_DEMATTE_COLOR_PICKER_DOWNLOAD_URI', 'http://www.dematte.at/colorPicker/cP_v0.91.zip');

/**
 * Implements hook_drush_command().
 */
function color_field_drush_command() {
  $items = array();

  $items['color_fied-plugin-scp'] = array(
    'description' => dt("Downloads the Simple Color Picker plugin."),
    'arguments' => array(
      'path' => dt('Optional. A path where to install the Simple Color Picker plugin. If omitted Drush will use the default location.'),
    ),
  );

  $items['color_fied-plugin-ecp'] = array(
    'description' => dt("Downloads the EyeCon Color Picker plugin."),
    'arguments' => array(
      'path' => dt('Optional. A path where to install the EyeCon Color Picker plugin. If omitted Drush will use the default location.'),
    ),
  );

  $items['color_fied-plugin-dcp'] = array(
    'description' => dt("Downloads the Dematte Color Picker plugin."),
    'arguments' => array(
      'path' => dt('Optional. A path where to install the Dematte Color Picker plugin. If omitted Drush will use the default location.'),
    ),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 */
function color_field_drush_help($section) {
  switch ($section) {
    case 'drush:color_fied-plugin-scp':
      return dt("Downloads the Simple Color Picker plugin, default location is sites/all/libraries.");
      break;
    case 'drush:color_fied-plugin-ecp':
      return dt("Downloads the EyeCon Color Picker plugin, default location is sites/all/libraries.");
      break;
    case 'drush:color_fied-plugin-dcp':
      return dt("Downloads the Dematte Color Picker plugin, default location is sites/all/libraries.");
      break;
  }
}

/**
 * Commands to download the Simple Color Picker plugin.
 */
function drush_color_field_color_fied_plugin_scp($path = 'sites/all/libraries') {
  $filename = 'jquery-simple-color.1.0.1.zip';
  $dirname = 'jquery-simple-color';

  if (!drush_shell_exec('type unzip')) {
    return drush_set_error(dt('Missing dependency: unzip. Install it before using this command.'));
  }

  // Create the path if it does not exist.
  if (!is_dir($path)) {
    drush_op('mkdir', $path);
    drush_log(dt('Directory @path was created', array('@path' => $path)), 'notice');
  }

  // Set the directory to the download location.
  $olddir = getcwd();
  chdir($path);

  // Remove any existing jQuery Simple Color plugin directory.
  if (is_dir($dirname)) {
    drush_shell_exec('rm -rf ' . $dirname);
    drush_log(dt('An existing Simple Color Picker plugin was overwritten at @path', array('@path' => $path)), 'notice');
  }

  // Remove any existing jQuery Simple Color plugin zip archive.
  if (is_file($filename)) {
    drush_op('unlink', $filename);
  }

  // Download the zip archive.
  if (!drush_shell_exec('wget ' . COLOR_FIELD_SIMPLE_COLOR_PICKER_DOWNLOAD_URI)) {
    drush_shell_exec('curl -kOL ' . COLOR_FIELD_SIMPLE_COLOR_PICKER_DOWNLOAD_URI);
  }

  if (is_file($filename)) {
    // Decompress the zip archive.
    drush_shell_exec('unzip -qq -o ' . $filename);
    // Remove the zip archive.
    drush_op('unlink', $filename);
  } else {
    drush_log(dt('Simple Color Picker plugin could not been downloaded from @url', array('@url' => COLOR_FIELD_SIMPLE_COLOR_PICKER_DOWNLOAD_URI)), 'error');
  }

  // Set working directory back to the previous working directory.
  chdir($olddir);

  if (is_dir($path . '/' . $dirname)) {
    drush_log(dt('jQuery Simple Color plugin has been downloaded to @path', array('@path' => $path)), 'success');
  }
  else {
    drush_log(dt('Drush was unable to download the jQuery Simple Color  plugin to @path', array('@path' => $path)), 'error');
  }
}

/**
 * Commands to download the EyeCon Color Picker plugin.
 */
function drush_color_field_color_fied_plugin_ecp($path = 'sites/all/libraries') {
  $filename = 'colorpicker.zip';
  $unzipfilename = 'cP_v0.91';
  $dirname = 'eyecon_colorpicker';

  if (!drush_shell_exec('type unzip')) {
    return drush_set_error(dt('Missing dependency: unzip. Install it before using this command.'));
  }

  // Create the path if it does not exist.
  if (!is_dir($path)) {
    drush_op('mkdir', $path);
    drush_log(dt('Directory @path was created', array('@path' => $path)), 'notice');
  }

  // Set the directory to the download location.
  $olddir = getcwd();
  chdir($path);

  // Remove any existing EyeCon Color Picker plugin directory.
  if (is_dir($dirname)) {
    drush_shell_exec('rm -rf ' . $dirname);
    drush_log(dt('An existing EyeCon Color Picker plugin was overwritten at @path', array('@path' => $path)), 'notice');
  }
  // Remove any existing EyeCon Color Picker plugin zip archive.
  if (is_file($filename)) {
    drush_op('unlink', $filename);
  }

  // Download the zip archive.
  if (!drush_shell_exec('wget ' . COLOR_FIELD_EYECON_COLOR_PICKER_DOWNLOAD_URI)) {
    drush_shell_exec('curl -kOL ' . COLOR_FIELD_EYECON_COLOR_PICKER_DOWNLOAD_URI);
  }

  if (is_file($filename)) {
    // Decompress the zip archive.
    drush_shell_exec('unzip -qq -o ' . $filename);
    // Remove old librairy.
    // Remove the zip archive.
    //drush_op('unlink', $filename);

  }

  // Set working directory back to the previous working directory.
  chdir($olddir);

  if (is_dir($path . '/' . $dirname)) {
    drush_log(dt('EyeCon Color Picker plugin has been downloaded to @path', array('@path' => $path)), 'success');
  }
  else {
    drush_log(dt('Drush was unable to download the EyeCon Color Picker plugin to @path', array('@path' => $path)), 'error');
  }
}

/**
 * Commands to download the Dematte Color Picker plugin.
 */
function drush_color_field_color_fied_plugin_dcp($path = 'sites/all/libraries') {
  $filename = 'cP_v0.91.zip';
  $unzipfilename = 'cP_v0.91';
  $dirname = 'dematte_colorpicker';

  if (!drush_shell_exec('type unzip')) {
    return drush_set_error(dt('Missing dependency: unzip. Install it before using this command.'));
  }

  // Create the path if it does not exist.
  if (!is_dir($path)) {
    drush_op('mkdir', $path);
    drush_log(dt('Directory @path was created', array('@path' => $path)), 'notice');
  }

  // Set the directory to the download location.
  $olddir = getcwd();
  chdir($path);

  // Remove any existing Dematte Color Picker plugin directory.
  if (is_dir($dirname)) {
    drush_log(dt('A existing Dematte Color Picker plugin was overwritten at @path', array('@path' => $path)), 'notice');
  }
  // Remove any existing Dematte Color Picker plugin zip archive.
  if (is_file($filename)) {
    drush_op('unlink', $filename);
  }

  // Download the zip archive.
  if (!drush_shell_exec('wget ' . COLOR_FIELD_DEMATTE_COLOR_PICKER_DOWNLOAD_URI)) {
    drush_shell_exec('curl -kOL ' . COLOR_FIELD_DEMATTE_COLOR_PICKER_DOWNLOAD_URI);
  }

  // If file has been downloaded.
  if (is_file($filename)) {
    // Decompress the zip archive.
    drush_shell_exec('unzip -qq -o ' . $filename);
    // Remove old library.
    if (is_dir($dirname)) {
      drush_shell_exec('rm -rf ' . $dirname);
    }
    drush_shell_exec('mv ' . $unzipfilename . ' ' . $dirname);
    // Remove the zip archive.
    drush_op('unlink', $filename);
  }

  // Set working directory back to the previous working directory.
  chdir($olddir);

  if (is_dir($path . '/' . $dirname)) {
    drush_log(dt('Dematte Color Picker plugin has been downloaded to @path', array('@path' => $path)), 'success');
  }
  else {
    drush_log(dt('Drush was unable to download the Dematte Color Picker plugin to @path', array('@path' => $path)), 'error');
  }
}
