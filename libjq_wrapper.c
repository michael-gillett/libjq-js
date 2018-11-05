#include "jq/src/jv.h"
#include "jq/src/jq.h"
#include <string.h>
#include <stdlib.h>

// Given a json string and a JQ filter
// Returns the result
char *execute(char *input_json_string, char *filter) {
	jq_state *state;

	state = jq_init();
	if (state == NULL) {
		perror("malloc");
	}

	// TODO jq_compile_args -c
	jq_compile(state, filter);

	jv value = jv_parse(input_json_string);
	jq_start(state, value, 0);

	// Parse
  jv results = jv_array();
	jv result;
  int num_results = 0;
  while (jv_is_valid(result = jq_next(state))) {
    results = jv_array_append(results, result);
    num_results++;
  }

  // If JQ returned only one result, remove the array wrapper
  if (num_results == 1) {
    results = jv_array_get(jv_copy(results), 0);
  }

  // Convert the jv to a json string
	jv jv_as_string = jv_dump_string(results, 0); // Frees results inside
	const char* output_json_string = jv_string_value(jv_as_string);

  // Copy the json string to a buffer than can be returned
  // This buffer must be freed by the calling javascript code
  char *result_buffer = malloc(strlen(output_json_string) + 1);
	strcpy(result_buffer, output_json_string);

	// Cleans up
	jv_free(jv_as_string);
	jq_teardown(&state);
	return result_buffer;
}

int main(int argc, char **argv) {
  const char *res = execute("{\"foo\": [1,2,3]}", ".foo");
	return 0;
}
