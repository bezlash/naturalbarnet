<?php

/**
 * @ file
 * YouTube field helper functions.
 */

/**
 * Extracts the video_id from the submitted field value.
 *
 * @param $input
 *   The input submitted to the field.
 *
 * @return
 *   Returns the video_id if available, or FALSE if not.
 */
function youtube_get_video_id($input) {
  // The video URL in the format http://youtube.com/watch?v=1SqBdS0XkV4.
  if (strstr($input, 'youtube.com/watch?') && preg_match('/v=[^&]*(?=&|$)/', $input, $matches)) {
    $video_id = ltrim($matches[0], 'v=');
  }

  // The video URL in the format http://youtu.be/1SqBdS0XkV4.
  elseif (strstr($input, 'youtu.be/')) {
    $anchor = 'be/';
    $position = strpos($input, $anchor);
    $video_id = trim(substr($input, $position + strlen($anchor)));
    // Remove additional query parameters.
    if ($param = strpos($video_id, '?')) {
      $video_id = substr($video_id, 0, $param);
    }
  }

  if (!empty($video_id)) {
    return $video_id;
  }

  return FALSE;
}

/**
 * Returns a list of standard YouTube video sizes.
 */
function youtube_size_options() {
  return array(
    '420x315' => '450px by 315px',
    '480x360' => '480px by 360px',
    '640x480' => '640px by 480px',
    '960x720' => '960px by 720px',
    'responsive' => 'responsive (full-width of container)',
    'custom' => 'custom',
  );
}

/**
 * Splits height and width when given size, as from youtube_size_options.
 */
function youtube_get_dimensions($size = NULL, $width = NULL, $height = NULL) {
  $dimensions = array();
  if ($size == 'responsive') {
    $dimensions['width'] = '100%';
    $dimensions['height'] = '100%';
  }
  elseif ($size == 'custom') {
    $dimensions['width'] = (int) $width;
    $dimensions['height'] = (int) $height;
  }
  else {
    // Locate the 'x'.
    $strpos = strpos($size, 'x');
    // Width is the first dimension.
    $dimensions['width'] = substr($size, 0, $strpos);
    // Height is the second dimension.
    $dimensions['height'] = substr($size, $strpos + 1, strlen($size));
  }

  return $dimensions;
}

/**
 * Retreve youtube thumbnail image via YouTube API.
 *
 * TODO add error messaging if something goes wrong, and return FALSE.
 *
 * @param $id
 *   The video_id of the particular YouTube video.
 * @param $force_small
 *   When TRUE, this function should return the standard size image regardless
 *   of what the youtube_thumb_hires variable is set to. This is used should
 *   the high resolution image be found to not exist for a particular video.
 */
function youtube_get_remote_image($id = NULL, $force_small = FALSE) {
  // This variable is TRUE when higher resolution thumnbails should be saved.
  // The only thumbnail resolution higher than the standard 480 is
  // 'maxresdefault'. This resolution image is not guaranteed to exist. After
  // saving the file, we check to ensure that it does.
  $youtube_thumb_hires = variable_get('youtube_thumb_hires', FALSE);

  if ($youtube_thumb_hires && !$force_small) {
    // Again, the high resolution image is not available via JSON-C API.
    $src = youtube_build_remote_image_path($id, 'maxresdefault');
  }
  else {
    $path = 'http://gdata.youtube.com/feeds/api/videos/' . $id;
    $query = array(
      'v' => '2', 
      'alt' => 'jsonc'
    );
    $url = url($path, array('query' => $query));
    $result = drupal_http_request($url);
    $data = json_decode($result->data);
    // Get the high quality default thumbnail.
    if (!empty($data->data)) {
      $src = $data->data->thumbnail->hqDefault;
    }
    else {
      $src = youtube_build_remote_image_path($id);
    }
  }

  // Make the actual request to download the file.
  $image_result = drupal_http_request($src);

  // Assure the youtube thumbnail directory exists.
  $files = variable_get('file_public_path', conf_path() . '/files');
  $youtube_dir = variable_get('youtube_thumb_dir', 'youtube');
  $youtube_path = $files . '/' . $youtube_dir;
  if (!file_prepare_directory($youtube_path, FILE_CREATE_DIRECTORY) && !mkdir($youtube_path, 0775, TRUE)) {
    watchdog('youtube', 'Failed to create YouTube thumbnail directory: %dir', array('%dir' => $youtube_path), WATCHDOG_ERROR);
  }

  // Save the file.
  $dest = $files . '/' . $youtube_dir . '/' . $id . '.png';
  file_put_contents($dest, $image_result->data);

  // If the high resolution image was saved but didn't actually exist, a very
  // small placeholder image from YouTube will have been saved. By checking the
  // dimensions of this image, we can determine if we saved the placeholder.
  if ($youtube_thumb_hires && !$force_small) {
    $image_path = $files . '/' . $youtube_dir . '/' . $id . '.png';
    if (file_exists($image_path)) {
      $image_size = getimagesize($image_path);
      if (empty($image_size[0]) || $image_size[0] < 480) {
        // We saved the placeholder. Re-run this function with $force_small
        // set to TRUE. This will give us the standard, guaranteed, thumbnail.
        youtube_get_remote_image($id, TRUE);
      }
    }
  }

  return TRUE;
}

/**
 * Get YouTube image path by building correctly formed URL.
 *
 * @param $video_id
 *   The ID of the video to grab the thumbnail from.
 * @param  $version
 *   Which version of the thumbnail to grab.
 * @return string
 *   The youtube.com image path to the specified version/video.
 */
function youtube_build_remote_image_path($video_id = NULL, $version = '0') {
  // The different versions of the image made available by YouTube.
  // http://stackoverflow.com/questions/2068344/how-to-get-thumbnail-of-youtube-video-link-using-youtube-api
  $versions = array(
    '0',
    'hqdefault',
    'mqdefault',
    'maxresdefault',
    'default',
    '1',
    '2',
    '3',
  );

  if (!$video_id || !in_array($version, $versions)) {
    return;
  }

  $version_path = 'http://img.youtube.com/vi/' . $video_id . '/' . $version . '.jpg';
  return url($version_path);
}

/**
 * Implements hook_feeds_processor_targets_alter().
 *
 * Adds a target option for YouTube fields to Feeds mapping options.
 *
 * @param &$targets
 *   Array containing the targets to be offered to the user. Add to this array
 *   to expose additional options. Remove from this array to suppress options.
 *   Remove with caution.
 * @param $entity_type
 *   The entity type of the target, for instance a 'node' entity.
 * @param $bundle_name
 *   The bundle name for which to alter targets.
 */
function youtube_feeds_processor_targets_alter(&$targets, $entity_type, $bundle_name) {
  foreach (field_info_instances($entity_type, $bundle_name) as $name => $instance) {
    $info = field_info_field($name);

    if (in_array($info['type'], array('youtube'))) {
      $targets[$name] = array(
        'name' => check_plain($instance['label']),
        'callback' => 'youtube_set_target',
        'description' => t('The @label field of the node.', array('@label' => $instance['label'])),
      );
    }
  }
}

/**
 * Callback to set the Feeds target for a YouTube field.
 *
 * @param $source
 *   Field mapper source settings.
 * @param $entity
 *   An entity object, for instance a node object.
 * @param $target
 *   A string identifying the target on the node.
 * @param $value
 *   The value to populate the target with.
 */
function youtube_set_target($source, $entity, $target, $value, $mapping) {
  $video_id = youtube_get_video_id($value);
  if ($video_id) {
    $entity->{$target}[LANGUAGE_NONE][0] = array(
      'input' => $value,
      'video_id' => $video_id,
    );
  }
}

/**
 * Implements hook_token_info_alter().
 *
 * Alters and adds tokens for each youtube field.
 *
 * @param $data
 *   The associative array of token definitions from hook_token_info().
 */
function youtube_token_info_alter(&$data) {
  // Get all youtube fields. Gather entity_type and bundle information.
  $fields = field_info_fields();
  $youtube_fields = array();
  foreach ($fields as $name => $field) {
    if ($field['type'] == 'youtube') {
      foreach ($field['bundles'] as $type => $entity_type) {
        foreach ($entity_type as $bundle) {
          $youtube_fields[] = array(
            'entity_type' => $type,
            'bundle' => $bundle,
            'field_name' => $name,
          );
        }
      }
    }
  }

  foreach ($youtube_fields as $field) {
    $field_info = field_info_instance($field['entity_type'], $field['field_name'], $field['bundle']);
    $field_label = $field_info['label'];

    // Modify the default field token.
    $data['tokens'][$field['entity_type']][$field['field_name']] = array(
      'name' => $field_label . t(": Default"),
      'description' => t("The YouTube video field value's Default (or Token if exists) view mode output."),
    );

    // Add two new tokens.
    $data['tokens'][$field['entity_type']][$field['field_name'] . '__youtube_video_url'] = array(
      'name' => $field_label . t(": Video URL"),
      'description' => t("The YouTube video field value's youtube.com URL."),
    );
    $data['tokens'][$field['entity_type']][$field['field_name'] . '__youtube_image_url'] = array(
      'name' => $field_label . t(": Image URL"),
      'description' => t("The YouTube video field value's local image URL."),
    );
  }
}

/**
 * Provide replacement values for placeholder tokens.
 *
 * Replaces youtube_video_url and youtube_image_url tokens.
 *
 * @param $type
 *   The machine-readable name of the type (group) of token being replaced, such
 *   as 'node', 'user', or another type defined by a hook_token_info()
 *   implementation.
 * @param $tokens
 *   An array of tokens to be replaced. The keys are the machine-readable token
 *   names, and the values are the raw [type:token] strings that appeared in the
 *   original text.
 * @param $data
 *   (optional) An associative array of data objects to be used when generating
 *   replacement values, as supplied in the $data parameter to token_replace().
 * @param $options
 *   (optional) An associative array of options for token replacement; see
 *   token_replace() for possible values.
 *
 * @return
 *   An associative array of replacement values, keyed by the raw [type:token]
 *   strings from the original text.
 *
 * @see youtube_tokens_info_alter()
 */
function youtube_tokens($type, $tokens, array $data = array(), array $options = array()) {
  $url_options = array('absolute' => TRUE);
  if (isset($options['language'])) {
    $url_options['language'] = $options['language'];
    $language_code = $options['language']->language;
  }
  else {
    $language_code = NULL;
  }
  $sanitize = !empty($options['sanitize']);

  $replacements = array();

  if ($type == 'node' && !empty($data['node'])) {
    $node = $data['node'];

    foreach ($tokens as $name => $original) {
      if (!strpos($name, '__youtube_')) {
        // This isn't a youtube token!
        return;
      }

      $token_pieces = explode('__', $name);
      if (count($token_pieces) != 2) {
        return;
      }

      $field_name = $token_pieces[0];
      $token_name = $token_pieces[1];

      switch ($token_name) {
        case 'youtube_video_url':
          $field = $node->$field_name;
          $video_id = $field[LANGUAGE_NONE][0]['video_id'];
          $replacements[$original] = 'http://www.youtube.com/watch?v=' . $video_id;
          break;

        case 'youtube_image_url':
          global $base_url;
          global $base_path;
          $field = $node->$field_name;
          $video_id = $field[LANGUAGE_NONE][0]['video_id'];
          $file_path = variable_get('file_public_path', conf_path() . '/files') . '/';
          $file_path .= variable_get('youtube_thumb_dir', 'youtube');
          $file_path .= '/' . $video_id . '.png';
          $full_path = $base_url . $base_path . $file_path;
          if (!file_exists($full_path)) {
            youtube_get_remote_image($video_id);
          }
          $replacements[$original] = $full_path;
          break;
      }
    }
  }

  return $replacements;
}
