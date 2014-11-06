<?php print $doctype; ?>
<html lang="<?php print $language->language; ?>" dir="<?php print $language->dir; ?>"<?php print $rdf->version . $rdf->namespaces; ?>>
<head<?php print $rdf->profile; ?>>
  <?php print $head; ?>
  <title><?php print $head_title; ?></title>  
  <?php print $styles; ?>
  <?php print $scripts; ?>
  <!--[if lt IE 9]><script src="//html5shiv.googlecode.com/svn/trunk/html5.js"></script><![endif]-->
</head>
<body<?php print $attributes;?>>
  <div id="skip-link">
    <a href="#main-content" class="element-invisible element-focusable"><?php print t('Skip to main content'); ?></a>
  </div>
  <?php print $page_top; ?>
  <?php print $page; ?>
  <?php print $page_bottom; ?>

<script src="<?php print path_to_theme(); ?>/js/equal-heights.js"></script>
<script type="text/javascript">
          jQuery(function($) {
                   $('.views-col-hair-tutorial').responsiveEqualHeightGrid(); 
                   $('.hair-tutorial-category-col').responsiveEqualHeightGrid(); 
		     $('.hair-care-responsive-grid-column').responsiveEqualHeightGrid();
		     $('.hair-view-responsive-grid-column').responsiveEqualHeightGrid();	
		     $('.front-videos-interested-in-column').responsiveEqualHeightGrid();
		     $('.front-articles-interested-in-column').responsiveEqualHeightGrid();
		     $('.products-interested-in-column').responsiveEqualHeightGrid();
          });
</script>


</body>
</html>

