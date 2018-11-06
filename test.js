var tape = require("tape");
var jq = require("./jq.js");

window = {};

tape("jq valid", function(t) {
  t.plan(3);

  // Test single result case
  jq.json({ foo: "bar", biz: 5 }, "[.foo, .biz] | {res: .}")
    .then(res => {
      t.deepEquals(res, { res: ["bar", 5] }, "Passes single result case");
    })
    .catch(e => t.fail("Should not get here"));

  // Test multi-result case
  jq.json({ foo: [1, 2, 3] }, ".foo | .[]")
    .then(res => {
      t.deepEquals(res, [1, 2, 3], "Passes multi-result case");
    })
    .catch(e => t.fail("Should not get here"));

  // Test nulls with empty json
  jq.json({}, ".foo")
    .then(res => t.equal(res, null, "Nulls with empty json"))
    .catch(e => t.fail("Shouldn't get here"));
});

tape("jq errors", function(t) {
  t.plan(3);

  // Test compile errors with invalid filter
  jq.json({ foo: "bar", biz: 5 }, "[.foo, .biz] | ")
    .then(res => t.fail("Should not get here"))
    .catch(e =>
      t.equals(e.message, "jq: 1 compile error", "Errors with invalid filter")
    );

  // Test errors cannot iterate over number
  jq.json({ foo: 1 }, ".foo | .[]")
    .then(res => t.fail("Shouldn't get here"))
    .catch(e =>
      t.equal(
        e.message,
        "Cannot iterate over number (1)",
        "Cannot iterate over number"
      )
    );

  // Test errors cannot index number with string
  jq.json({ foo: 1 }, ".foo.bar")
    .then(res => t.fail("Shouldn't get here"))
    .catch(e =>
      t.equal(
        e.message,
        'Cannot index number with string "bar"',
        "Cannot index number with string"
      )
    );
});
