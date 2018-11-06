## Disclaimer

This package is still very new and doesn't support all of the JQ functionality that [`jq-web`](https://github.com/fiatjaf/jq-web) provides, but aims to be a light-weight alternative. Instead of wrapping the entire JQ cli like `jq-web`, we have a custom C wrapper that make calls to the libjq C API directly.

## Installation

```
yarn add libjq-js
or
npm install libjq-js
```

## API

```javascript
jq.json(json: object, filter: string): Promise<any>
```

### Example

```javascript
import jq from "libjq-js";
jq.json({ foo: 5 }, ".foo").then(res => console.log(res));
```

Outputs

```
5
```

## Getting setup for development

```
# This can take 5+ minutes,but only needs to be run once
git submodule update --init --recursive
./configure

# Run this everytime you change libjq_wrapper.c, pre.js, or post.js
make jq.js
```
