#!/bin/bash

# Clean old test.
rm -rf test/output_error > /dev/null || true
# Test serially.
bats --jobs 4 test/*/*.bats
