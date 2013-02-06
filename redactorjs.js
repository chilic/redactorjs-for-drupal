
(function ($) {
  if (Drupal.jsEnabled){
    $(document).ready(function(){

      if (typeof Drupal.settings.redactorjs.imageUpload !== 'undefined') {

        // Image insert Callback
        var redactorjs_imagePreUploadCallback = function(obj, json) {
          var html = '', data = '';
          console.log(obj);
          data = $.parseJSON(json);

          if (typeof data.error === 'undefined') {
            html = '<p><img src="' + data.filelink + '" /></p>';
          }

          return html;
        }
      }

      $(".redactorjs-processed").each(function (i) {
        var settings = Drupal.settings.redactorjs;
        settings.uploadFields = {"field_name": $(this).attr("name"), "form_build_id": $(this).closest("form").find('input[name="form_build_id"]').attr("id")};
        $(this).redactor(settings);
      });
    });
  }
})(jQuery);