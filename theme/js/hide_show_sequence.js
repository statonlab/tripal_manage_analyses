(function ($) {

        Drupal.behaviors.tripal_manage_analyses = {
            attach: function (context, settings) {


                $(document).on('click', '.sequence-expand-trigger', function (e) {

                    var target = $(this).attr('data-target')
                    if (!target) {
                        return
                    }

                    console.log(target)
                    var target_obj = $("#" + target)

                    console.log(target_obj)

                    if (target_obj.is(':visible') === true) {
                        $(this).text('Sequence');

                        target_obj.hide();
                    } else {
                        target_obj.show()
                        $(this).text('Hide sequence');
                    }
                });

            }
        }
    }
)(jQuery);

