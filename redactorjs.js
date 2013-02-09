(function ($) {
  if (Drupal.jsEnabled){
    $(document).ready(function(){

      Drupal.settings.redactorjs.buttonsCustom = {
        teaser: {
          title: 'Teaser break',
          callback: function(obj, event, key) {
            var b = $(obj.$editor.find("#drupal-break"));
            if (b.length == 0) {
              b = '<img id="drupal-break" src="/sites/all/modules/redactorjs/spacer.gif"/>';
              obj.insertHtml(b);
            }
            else {
              $(b).remove();
            }
          }
        }
      }

      $(".redactorjs-processed").each(function (i) {
        var settings = Drupal.settings.redactorjs;
        settings.uploadFields = {
          "field_name": $(this).attr("name"),
          "form_build_id": $(this).closest("form").find('input[name="form_build_id"]').attr("id")
        };
        $(this).redactor(settings);
        $(this).parent().next(".grippie").css("display", "none");
      });
    });
  }
})(jQuery);