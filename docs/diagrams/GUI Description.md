# Video (each video has this info I can give you)

-   **created_at** (timestamp it was added)
    -   can be any date time representation you want, I can convert
        the data to that representation
-   **progress** (download progress)
    -   is be a null/unset value for videos that haven't started downloading
        or were from history
    -   currently stored as a string that is "0%" to "100%" but I can change it to
        a numeric value for other representations
-   **info** (info on the video)
    -   It's possible the underlying CLI program couldn't provide some of these infos
        -   Suggestion: a placeholder value could be used or that element is hidden
            when it doesn't exist
        -   There is an explicit guarantee:
            -   If **audio_available** is false, it's guaranteed **formats** isn't empty
            -   If **formats** is empty, it's guaranteed **audio_available** is true
            -   Reasoning: Program logic has validated that there is at least
                _something_ downloadable from this "video" that was added.
    -   **video_id**
        -   e.g.:
            -   "Sv3LXGWKw6Q" from YT
            -   "jNQXAC9IVRw" from YT
            -   "652eccdcf4d64600015fd610" from America's Test Kitchen
    -   **title**
        -   e.g.:
            -   "Should this be the future of Angular applications?"
            -   "Me at the zoo"
            -   "Sausages and Salad"
    -   **author**
        -   e.g.:
            -   "Joshua Morony"
            -   "jawed"
            -   ""
    -   **seconds** (length of vid in secs)
        -   of course, can be converted to any representation you want
        -   e.g.:
            -   341
            -   19
            -   1438
    -   **thumbnail** (url to a thumbnail)
        -   e.g.:
            -   "https://i.ytimg.com/vi/Sv3LXGWKw6Q/maxresdefault.jpg"
            -   "https://i.ytimg.com/vi/jNQXAC9IVRw/"
            -   "https://gvimage.zype.com/5b0820fbdc4390132f0001ca/652eccdcf4d64600015fd610/custom_thumbnail/1080.jpg?1701815955"
    -   **url** (original url to the video)
        -   e.g.:
            -   "https://youtu.be/Sv3LXGWKw6Q"
            -   "https://youtu.be/jNQXAC9IVRw"
            -   "https://www.americastestkitchen.com/cookscountry/episode/918-sausages-and-salad"
    -   **audio_available**
        -   e.g.:
            -   true
            -   true
            -   true
    -   **formats** (list of formats)
        -   for each format:
            -   only format_id is guaranteed to exist
            -   suggestion: again, placeholder/hide value
            -   **format_id** (unique relative to other formats in the the current
                list)
                -   e.g.:
                    -   "hls-360"
                    -   "602"
                    -   "394"
            -   **container** (file ext)
                -   e.g.:
                    -   "mp4"
                    -   "webm"
                    -   "avi"
            -   **width** (video dimension width)
                -   e.g.:
                    -   426
                    -   1280
                    -   1920
            -   **height** (video dimension height)
                -   e.g.:
                    -   240
                    -   720
                    -   1080
            -   **fps**
                -   e.g.:
                    -   0
                    -   15
                    -   30.0
                    -   60
-   **selected_format** (will be a format_id from **formats**)
    -   this is the format that will be selected when downloading the video
    -   currently, by default is the format_id of the last format in the formats
        list. if formats is empty, this is an empty string, "".
    -   e.g.:
        -   "hls-360"
        -   "602"
        -   "394"

# Necessary components

-   text box to input video urls
-   console pane to show CLI error messages and our own error messages
    - suggestion: collapsible
-   button to open output folder
-   button to remove all videos from history (implicitly cancels download)
-   button download all videos
    -   only those added in the current session that haven't been downloaded yet.
        therefore, excludes those from history
-   button to cancel all videos currently downloading
-   for each video pane:
    -   dropdown to select format from **formats**
    -   button to start download
    -   button to cancel download
        -   suggestion: takes the position of the start download button
            when downloading, hiding the start dl button
    -   button to delete this video from history (implicitly cancels download)
-   settings:
    -   none that I can think of! the best configuration is ours :handshake:
        :pop_cat_gun:
