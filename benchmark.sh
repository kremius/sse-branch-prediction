#! /bin/bash

for i in build/*; do echo "Processing: $i"; time $i; done
