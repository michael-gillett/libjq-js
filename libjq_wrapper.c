#include "jq/src/jv.h"
#include "jq/src/jq.h"
#include <string.h>
#include <stdlib.h>

void set_error(char *err_buf, jv err) {
	if (jv_get_kind(err) != JV_KIND_STRING) {
		err = jv_dump_string(err, JV_PRINT_INVALID);
	}

	if (strncmp(jv_string_value(err), "jq: error", strlen("jq: error"))) {
		const char *error_message = jv_string_value(err);
		strcpy(err_buf, error_message);
	}

	jv_free(err);
}

static void err_cb(void *err_buf, jv err) {
	err_buf = (char*) err_buf;
	set_error(err_buf, err);
}

// Given a json string and a JQ filter
// Returns the result
char **execute(char *input_json_string, char *filter) {
	jq_state *state;

	// Setup buffers for passing data back to javascript
	char **outputs = malloc(sizeof(char *) * 2);
	outputs[0] = NULL;	// Output buffer
	// Error buffer, initialize with zeros since we might not write to it
	outputs[1] = calloc(512, sizeof(char));

	state = jq_init();
	if (state == NULL) {
		strcpy(outputs[1], "Failed to initialize jq");
		return outputs;
	}

	// Set the error callback for handling issues
	jq_set_error_cb(state, err_cb, outputs[1]);

	// Compile the JQ filter
	if (jq_compile(state, filter) == 0) {
		jq_teardown(&state);
		// Error message will be set in err_cb
		return outputs;
	}

	jv value = jv_parse(input_json_string);
	jq_start(state, value, 0);

	// Execute JQ filter over json
  jv results = jv_array();
	jv result;
  int num_results = 0;
  while (jv_is_valid(result = jq_next(state))) {
    results = jv_array_append(results, result);
    num_results++;
  }

	if (jq_halted(state)) {
		jv err = jq_get_error_message(state);
		set_error(outputs[1], err);
		jq_teardown(&state);
		return outputs;
	} else if (jv_invalid_has_msg(jv_copy(result))) {
		jv err = jv_invalid_get_msg(jv_copy(result));
		set_error(outputs[1], err);
		jq_teardown(&state);
		return outputs;
	}

	// If JQ returned only one result, remove the array wrapper
	if (num_results == 1) {
		results = jv_array_get(jv_copy(results), 0);
	} else if (num_results == 0) {
		// If there were no results we should return null instead of []
		results = jv_null();
	}

	// Convert the jv to a json string
	jv jv_as_string = jv_dump_string(results, 0); // Frees results inside
	const char* output_json_string = jv_string_value(jv_as_string);

	// Copy the json string to a buffer than can be returned
	outputs[0] = malloc(strlen(output_json_string) + 1);
	strcpy(outputs[0], output_json_string);

	// Cleans up state
	jv_free(jv_as_string);
	jq_teardown(&state);
	return outputs;
}
