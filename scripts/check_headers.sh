#!/bin/bash
for header in include/*.h; do
    echo "Checking $header..."
    gcc -fsyntax-only -c "$header"
done 