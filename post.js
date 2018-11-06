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
    return parseResult(output, inputJson, filter);
  }
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
