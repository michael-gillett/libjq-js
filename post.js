function json(inputJson, filter) {
  var stringPtr = Module.ccall(
    "execute",
    "number",
    ["string", "string"],
    [JSON.stringify(inputJson), filter]
  );
  var resultJson = parseResult(Pointer_stringify(stringPtr), inputJson, filter);
  Module._free(stringPtr);
  return result;
}

function parseResult(jsonString, inputJson, filter) {
  try {
    return JSON.parse(jsonString);
  } catch (e) {
    var errorMsg = "JQ library error: " + e.message;
    window.Rollbar &&
      window.Rollbar.error(errorMsg, {
        json: inputJson,
        filter: filter,
        jqResultString: jsonString,
        stacktrace: e.stacktrace,
        memoryInfo: window.performance.memory
      });
    return null;
  }
}
