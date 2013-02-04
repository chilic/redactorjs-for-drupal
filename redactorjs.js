/*
function testButton(obj, event, key)
{
  obj.insertHtml('<!--break-->');
}
*/
function redactorjs_imageUploadCallback(obj, json) {
  console.log('ok');
}

(function ($) {
  if (Drupal.jsEnabled){
    $(document).ready(function(){
      $('.redactorjs-processed').next(".grippie").css("display", "none");
      $('.redactorjs-processed').redactor(Drupal.settings.redactorjs);
    });
  }
})(jQuery);