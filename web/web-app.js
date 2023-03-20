var WebApp = (function(CalibratorLib, FormKeeper) {
    var form = null; 

    function OnReady() {
        form = $("#main-form")[0];
        //FormKeeper.GetURLVariables(form.elements);
    }

    function OnChange() {
        if(!form)
            return;
        //var href = FormKeeper.Serialize(form);
        //window.history.replaceState({}, "", href);
    }

    function ParseData(text) {
        var out = [];
        var lines = text.split("\n");
        var splitChars = "[\s,]*";
        for(var i = 0; i < lines.length; i++) {
            var trimmed = lines[i].replace(/^[\s,]+/, "").replace(/[\s,]+$/, "");
            var split = trimmed.split(/[\s,]+/);
            if(split.length == 0 || (split.length == 1 && split[0] == ""))
                continue;
            if(split.length != 3) {
                throw "Error at line " + i + ": Must be exactly three values (XYZ) separated by comma or any space";
            }
            var cur = [];
            for(var j = 0; j < 3; j++) {
                cur[j] = Number.parseFloat(split[j]);
                if(Number.isNaN(cur[j])) {
                    throw "Error at line " + i + ": Invalid number at " + j + "th coordinate";
                }
            }
            out.push(cur);
        }
        /*
        var str = "";
        for(var i = 0; i < out.length; i++) {
            for(var j = 0; j < 3; j++) {
                str += out[i][j].toFixed(6);
                if (j != 2)
                    str += "\t";
            }
            str += "\n";
        }
        console.log(str);
        */
        return out;
    }

    function SetError(success, msg) {
        var elem = $("#main-form #status");
        if(success) {
            elem.addClass("success");
            elem.removeClass("fatal");
        } else {
            elem.removeClass("success");
            elem.addClass("fatal");
        }
        elem.html(msg);
    }

    function MatrixToString(matrix) {
        if (matrix.length == 0)
            return "[]";
        var str = "";
        for(var i = 0; i < matrix.length; i++) {
            var row = matrix[i];
            if(i == 0)
                str += "[[";
            else
                str += " [";
            for(var j = 0; j < row.length; j++) {
                str += (0.0 + row[j]).toFixed(8);
                if (j != row.length-1)
                    str += ", ";
            }
            str += "]";
            if (i == matrix.length-1)
                str += "]";
            else
                str += ",";
            str += "\n";
        }
        return str;
    }

    function DisplayResults(cost, sensitivity, offset) {
        var elem = $("#results");
        var str = "Mean-Of-Squared-Errors (normalized - divided by magnitude^2):\n " + (0.0 + cost).toFixed(8) + "\n";
        str += "\nSensitivity Matrix (S):\n" + MatrixToString(sensitivity);
        str += "\nOffset Vector (e):\n" + MatrixToString(offset);
        str += "";
        elem.text(str);
    }

    function OnSubmit() {
        if(!form)
            return;
        var mag = form.elements.magnitude.value;
        var data = form.elements.data.value;
        try {
            data = ParseData(data);
        } catch(err) {
            SetError(false, err);
            return false;
        }

        var result = CalibratorLib.FindSensitivityOffsets(mag, data);
        if (Number.isNaN(result.cost) || !Number.isFinite(result.cost)) {
            SetError(false, "Calculation error occured. Make sure you have entered at least 9 distinct data points, ideally at least 18");
            return false;
        }

        DisplayResults(result.cost, result.sensitivity, result.offset);
        SetError(true, "Matrices have been successfully recalculated!");

        return false; // This is called on submit event, so return 
        // false to avoid submitting the form.
    }

    return {
        OnReady,
        OnChange,
        OnSubmit,
        SetError
    };
})(CalibratorLib, FormKeeper);
