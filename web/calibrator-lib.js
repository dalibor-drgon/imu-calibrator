var Module = (function() {
    var callbacks = [];
    var initialized = false;
    return {
        onRuntimeInitialized: function() {
            initialized = true;
            for(var i = 0; i < callbacks.length; i++) {
                callbacks[i](Module);
            }
            callbacks = null;
        },
        registerAfterInitHandler: function(callback) {
            if(initialized) callback(Module);
            else callbacks.push(callback);
        }
    }
})();


var CalibratorLib = (function(Module) {

    function ConvertToEntryArray(out, entries) {
        //var out = new Float32Array(entries.length * 4);
        for(var i = 0; i < entries.length; i++) {
            out[4*i+0] = entries[i][0];
            out[4*i+1] = entries[i][1];
            out[4*i+2] = entries[i][2];
        }
        //return out;
    }

    function ConvertToMatrix(array, cols) {
        var out = [];
        var cur;
        for(var i = 0; i < array.length; i++) {
            if((i % cols) == 0) {
                cur = [];
                out.push(cur);
            }
            cur.push(array[i]);
        }
        return out;
    }

    function FindSensitivityOffsetsLowLevel(magnitude, measurements) {
        var stackStart = Module.stackSave();
        var entries_ptr = Module.stackAlloc(4*4*measurements.length);
        ConvertToEntryArray(new Float32Array(Module.HEAPF32.buffer, entries_ptr, 4*measurements.length), measurements);
        var sensitivity_ptr = Module.stackAlloc(9*4);
        var sensitivity_array = new Float32Array(Module.HEAPF32.buffer, sensitivity_ptr, 9);
        var offset_ptr = Module.stackAlloc(3*4);
        var offset_array = new Float32Array(Module.HEAPF32.buffer, offset_ptr, 3);
        var cost = Module._acc_find_sensitivity_offsets(magnitude, 
            measurements.length, entries_ptr, 
            sensitivity_ptr, offset_ptr); 
        Module.stackRestore(stackStart);
        return {
            cost: cost,
            sensitivity: new Float32Array(sensitivity_array),
            offset: new Float32Array(offset_array)
        };
    }

    function FindSensitivityOffsets(magnitude, measurements) {
        var datall = FindSensitivityOffsetsLowLevel(magnitude, measurements);
        return {
            cost: datall.cost,
            sensitivity: ConvertToMatrix(datall.sensitivity, 3),
            offset: ConvertToMatrix(datall.offset, 1)
        };
    }

    return {
        FindSensitivityOffsetsLowLevel,
        FindSensitivityOffsets,
        ConvertToMatrix,
        ConvertToEntryArray
    };
})(Module);
