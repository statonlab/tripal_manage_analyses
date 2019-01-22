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

            // Add the anchor to the pager links so that when the user clicks a pager
            // link and the page refreshes they are taken back to the location
            // on the page that they were viewing.
            $("div.tripal_manage_analyses-info-box-desc ul.pager a").each(function () {
                pager_link = $(this);
                parent = pager_link.parents('div.tripal_manage_analyses-info-box-desc');
                pager_link.attr('href', pager_link.attr('href') + '#' + parent.attr('id'));
            })
        }
    };

    /**
     * Initializes the feature viewers on the page.
     */
    function tripal_manage_analyses_feature_viewers(features) {


        var residues = features.residues

        children = features.children

        console.log(children)


        Object.keys(children).forEach(function (key, index) {
            console.log(key)
            console.log(index)

            //Each child gets its own feature viewer
            var options = {
                showAxis: true,
                showSequence: true,
                brushActive: true,
                toolbar: true,
                bubbleHelp: true,
                zoomMax: 3
            }

            var fv = new FeatureViewer(residues, '#tripal_manage_expression_featureloc_viewer_' + index, options);
            subFeatures = children[key]

            Object.keys(subFeatures).forEach(function (sfKey, sfIndex) {
                subFeature = subFeatures[sfKey]
                fv.addFeature(subFeature)

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

        // Trigger a window resize event to notify charting modules that
        // the container dimensions has changed

        $(document).on('collapsed', function (e) {



            setTimeout(function() {

                if (typeof Event !== 'undefined') {
                    window.dispatchEvent(new Event('resize'));
                }
                else {
                    // Support IE
                    var event = window.document.createEvent('UIEvents');
                    event.initUIEvent('resize', true, false, window, 0);
                    window.dispatchEvent(event);
                }

            }, 501)


        });




    }
})(jQuery);

