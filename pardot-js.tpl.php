<?php
/**
 * @file
 * Render Pardot JavaScript and parameters
 */
?>
piAId = "<?php print check_plain($pardot_a_id); ?>";
piCId = "<?php print check_plain($pardot_c_id); ?>";
<?php if ($score): ?>
  piPoints = "<?php print check_plain($score); ?>";
<?php endif; ?>

(function() {
  function async_load(){
    var s = document.createElement('script'); s.type = 'text/javascript';
    s.src = ('https:' == document.location.protocol ? 'https://pi' : 'http://cdn') + '.pardot.com/pd.js';
    var c = document.getElementsByTagName('script')[0]; c.parentNode.insertBefore(s, c);
  }
  if(window.attachEvent) { window.attachEvent('onload', async_load); }
  else { window.addEventListener('load', async_load, false); }
})();
