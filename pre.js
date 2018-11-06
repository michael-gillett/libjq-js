var isInitialized = false;
var initListeners = [];

var onInitialized = {
  addListener: function(cb) {
    if (isInitialized) {
      cb();
    }
    initListeners.push(cb);
  }
};

// Note about Emscripten, even though the module is now named 'jq', pre.js still uses Module, but post.js uses 'jq'
Module = Object.assign({}, Module, {
  "noInitialRun": true,
  "noExitRuntime": false,
  "onRuntimeInitialized": function() {
    isInitialized = true;
    initListeners.forEach(function(cb) {
      cb();
    });
  },
  "json": function() {
    var args = arguments;
    return new Promise(function(resolve, reject) {
      onInitialized.addListener(function() {
        try {
          resolve(json.apply(Module, args));
        } catch (e) {
          reject(e);
        }
      });
    });
  }
});

function json(inputJson, filter) {
  // Returns pointer to output and error strings
  var buffersPtr = Module.ccall(
    "execute",
    "number",
    ["string", "string"],
    [JSON.stringify(inputJson), filter]
  );

  // Get pointers to output and error
  var type = "i32";
  var outputPtr = getValue(buffersPtr, type);
  var errorPtr = getValue(buffersPtr + getNativeTypeSize(type), type);

  // Get the output and error strings
  var output = Pointer_stringify(outputPtr);
  var err = Pointer_stringify(errorPtr);

  // Free the malloc'd pointers
  Module._free(outputPtr);
  Module._free(errorPtr);
  Module._free(buffersPtr);

  if (err) {
    throw new Error(err);
  } else {
    return JSON.parse(output);
  }
}
