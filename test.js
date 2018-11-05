var tape = require("tape");
var jq = require("./jq.js");

window = {};
tape("jq", function(t) {
  t.plan(1);

  jq.json(
    { foo: "bar", biz: 5 },
    '[.foo, .biz] | {res: .}'
  )
    .then(res => {
      t.deepEquals(res, { res: ["bar", 5] });
    })
    .catch(e => console.log(e));
});
