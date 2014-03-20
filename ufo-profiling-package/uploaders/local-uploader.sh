#! /usr/bin/bash

input_file_path="$1"
storage_folder="/pdv/home/rshkarin"
output_path="$storage_folder"/"$2"


#create path if necessary
if [ ! -d "$output_path" ]; then
  mkdir -p "$output_path"
fi

#if mounted folder is absent
if [ ! -d "$storage_folder" ]; then
  echo "The folder of mounted disk has not been set"
  return
fi

#perform copy
cp "$input_file_path" "$output_path"
