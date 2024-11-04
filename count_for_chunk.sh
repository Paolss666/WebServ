#!/bin/bash

# Check if a file is provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

# Loop through each line of the file
while IFS= read -r line; do
    # Count the number of characters and convert to hex
    count=$(echo -n "$line" | wc -c)
    printf "%s: %x\n" "$line" "$count"
done < "$1"
