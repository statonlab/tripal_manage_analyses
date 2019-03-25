(function ($) {

    Drupal.behaviors.tripal_manage_analyses = {
        attach: function (context, settings) {

            // Remove the jquery.ui override of our link theme:
            $(".ui-widget-content").removeClass('ui-widget-content')

            /**
             * JS for hiding/showing the Sequences.
             */

            // Hide all the results tables.
            $(".tripal-manage-analyses-sequence").hide();

            $(".tripal-analysis-blast-table-sequence").click(function(e) {
                var my_id = e.target.id;

                //get feature_id from the target
                feature_id = my_id

                var sequence = $("#tripal-analysis-blast-table-sequence" + feature_id)
                if (sequence.is(':visible') === true) {
                    sequence.hide();
                }
                else {
                    sequence.show();
                    $(this).text('Hide Results Table');
                }
            });
        }
    };

})(jQuery);

