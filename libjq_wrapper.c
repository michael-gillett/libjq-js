#include "jq/src/jv.h"
#include "jq/src/jq.h"
#include <string.h>
#include <stdlib.h>

#define ERR_BUFFER_SIZE 512

void set_error(char *err_buf, jv err) {
	if (jv_get_kind(err) != JV_KIND_STRING) {
		err = jv_dump_string(err, JV_PRINT_INVALID);
	}

	if (strncmp(jv_string_value(err), "jq: error", strlen("jq: error"))) {
		const char *error_message = jv_string_value(err);
		strlcpy(err_buf, error_message, ERR_BUFFER_SIZE);
	}

	jv_free(err);
}

static void err_cb(void *err_buf, jv err) {
	err_buf = (char*) err_buf;
	set_error(err_buf, err);
}

char **combined_return(char *out_buf, char *err_buf) {
	char **outputs = malloc(sizeof(char*) * 2);
	outputs[0] = out_buf;
	outputs[1] = err_buf;
	return outputs;
}

// Given a json string and a JQ filter
// Returns the result
char **execute(char *input_json_string, char *filter) {
	jq_state *state;
	char *out_buf = NULL;
	char *err_buf = calloc(ERR_BUFFER_SIZE, sizeof(char)); // Zero'd since we might not write to it

	state = jq_init();
	if (state == NULL) {
		strcpy(err_buf, "Failed to initialize jq");
		return combined_return(out_buf, err_buf);
	}

	// Set the error callback for handling issues
	jq_set_error_cb(state, err_cb, err_buf);

	// Compile the JQ filter
	if (jq_compile(state, filter) == 0) {
		jq_teardown(&state);
		// Error message will be set in err_cb
		return combined_return(out_buf, err_buf);
	}

	jv input = jv_parse(input_json_string);
	jq_start(state, input, 0);

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
		set_error(err_buf, err);
		jq_teardown(&state);
		return combined_return(out_buf, err_buf);
	} else if (jv_invalid_has_msg(jv_copy(result))) {
		jv err = jv_invalid_get_msg(result);
		set_error(err_buf, err);
		jv_free(results);
		jq_teardown(&state);
		return combined_return(out_buf, err_buf);
	}

	// If JQ returned only one result, remove the array wrapper
	if (num_results == 1) {
		results = jv_array_get(results, 0);
	}

	// Convert the jv to a json string
	jv jv_as_string = jv_dump_string(results, 0);
	const char* output_json_string = jv_string_value(jv_as_string);

	// Copy the json string to a buffer than can be returned
	out_buf = malloc(strlen(output_json_string) + 1);
	strcpy(out_buf, output_json_string);

	// Cleans up state
	jv_free(jv_as_string);
	jq_teardown(&state);
	return combined_return(out_buf, err_buf);
}
