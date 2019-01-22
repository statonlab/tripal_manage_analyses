//https://cdn.rawgit.com/calipho-sib/feature-viewer/v1.0.0/examples/index.html


(function ($) {

    Drupal.behaviors.tripal_analysis_blast = {
        attach: function (context, settings) {

            /**
             * JS to add the feature viewer.
             */
            tripal_manage_analyses_feature_viewers(settings.children_draw_info);

            // Remove the jquery.ui override of our link theme:
            $(".ui-widget-content").removeClass('ui-widget-content')

            /**
             * JS for the HSP box the appears when the [more] link is clicked.
             */

            // Hide all the HSP description boxes by default
            $(".tripal-analysis-blast-info-hsp-desc").hide();

            // When the [more] link is clicked, show the appropriate HSP description box
            $(".tripal-analysis-blast-info-hsp-link").click(function(e) {
                var my_id = e.target.id;
                var re = /hsp-link-(\d+)-(\d+)/;
                var matches = my_id.match(re);
                var analysis_id = matches[1];
                var j = matches[2];
                $(".tripal-analysis-blast-info-hsp-desc").hide();
                $("#hsp-desc-" + analysis_id + "-" +j).show();
            });

            // When the [Close] button is clicked on the HSP description close the box
            $(".hsp-desc-close").click(function(e) {
                $(".tripal-analysis-blast-info-hsp-desc").hide();
            });

            // Add the anchor to the pager links so that when the user clicks a pager
            // link and the page refreshes they are taken back to the location
            // on the page that they were viewing.
            $("div.tripal_manage_analyses-info-box-desc ul.pager a").each(function() {
                pager_link = $(this);
                parent = pager_link.parents('div.tripal_manage_analyses-info-box-desc');
                pager_link.attr('href', pager_link.attr('href') + '#' + parent.attr('id'));
            })
        }
    };

    /**
     * Initializes the feature viewers on the page.
     */
    function tripal_manage_analyses_feature_viewers(features){
        console.log(features)

        var residues = features.residues


        //Create a single feature viewer.

            var options = {
                showAxis: true,
                showSequence: true,
                brushActive: true,
                toolbar: true,
                bubbleHelp: true,
                zoomMax: 3
            }

             var fv = new FeatureViewer(residues, '#tripal_manage_expression_featureloc_viewer', options);


            //Loop through features info

        features.info.each(function() {

            let sub_name = 'something'
            let data = 'something'
            fv.addFeature({data:data,
            name: sub_name,
                color: '#888888',
                type: 'rect'
            })

        })

        //     fv.onFeatureSelected(function (d) {
        //         var id = d.detail.id;
        //         var re = /tripal-analysis-blast-hsp-(\d+)-(\d+)/;
        //         var matches = id.match(re);
        //         var analysis_id = matches[1];
        //         var j = matches[2];
        //         $(".tripal-analysis-blast-info-hsp-desc").hide();
        //         $("#hsp-desc-" + analysis_id + "-" +j).show();
        //     });
        // });
        //


    }
})(jQuery);

