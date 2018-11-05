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
