
var FormKeeper = (function() {
    function deparam(query, elements) {
        // remove leading question mark if its there
        if (query !== '') {
            var pairs = query.split('&');
            for (var i = 0; i < pairs.length; i += 1) {
                var cur = pairs[i];
                var eq_index = cur.indexOf('=');
                key = (eq_index == -1) ? cur : decodeURIComponent(cur.substring(0, eq_index));
                value = (eq_index == -1) ? null : decodeURIComponent(cur.substring(eq_index+1));
                if(elements[key]) elements[key].value = value;
                else elements[key] = value
            }
        }
    }



    // Get variables after #
    function GetURLVariables(out) {
        var ind = window.location.href.indexOf('#');
        if(ind != -1) {
            deparam(window.location.href.substring(ind+1), out);
        }
    }

    function Serialize(form) {
        return window.location.href.split("#")[0] + "#" +$(form).serialize();
    }

    return {
        GetURLVariables,
        Serialize
    };
})();
