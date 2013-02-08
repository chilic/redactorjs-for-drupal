(function ($) {
  if (Drupal.jsEnabled){
    $(document).ready(function(){

      Drupal.settings.redactorjs.buttonsCustom = {
        teaser: {
          title: 'Teaser break',
          callback: function(obj, event, key) {
            obj.insertHtml('<img id="drupal-break" src="/sites/all/modules/redactorjs/spacer.gif"/>');
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