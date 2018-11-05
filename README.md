# Getting setup for development
```
# This can take 5+ minutes,but only needs to be run once
git submodule update --init --recursive
./configure

# Run this everytime you change libjq_wrapper.c, pre.js, or post.js
make jq.js
```
