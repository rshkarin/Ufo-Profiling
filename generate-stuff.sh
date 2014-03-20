#! /usr/bin/bash

WORKING_FOLDER="$1"
PY_PATH="${HOME}/.local/lib/python2.7/site-packages:${HOME}/.local/lib64/python2.7/site-packages:/usr/lib/python2.7/site-packages"
PROFILING_LOCAL_FOLDER="profiling-data"
LOCAL_STORAGE_PATH="/pdv/home/rshkarin"
WORKING_PATH=${LOCAL_STORAGE_PATH}/${PROFILING_LOCAL_FOLDER}/${WORKING_FOLDER}
UFO_PROFILING_PACKAGE_FOLDER="ufo-profiling-package"
UPLOADER_SCRIPT_PATH="$(pwd)"/${UFO_PROFILING_PACKAGE_FOLDER}/"uploaders"
PROCESSING_SCRIPT_PATH="$(pwd)"/${UFO_PROFILING_PACKAGE_FOLDER}/"processing"
GRAPHS_PATH="graphics"
SPREADSHEETS_PATH="spreadsheets"
TIMINGS_PATH="timings"
GRAPH_FILE_NAME="graph.svg"
SPREADSHEET_FILE_NAME="result.csv"

#set evn variables
export PYTHONPATH="$PY_PATH"

if [ ! -d "$WORKING_PATH"/"$GRAPHS_PATH" ]; then
  mkdir -p "$WORKING_PATH"/"$GRAPHS_PATH"
fi

if [ ! -d "$WORKING_PATH"/"$SPREADSHEETS_PATH" ]; then
  mkdir -p "$WORKING_PATH"/"$SPREADSHEETS_PATH"
fi

#create graph
python "$PROCESSING_SCRIPT_PATH"/graph_plot.py -i "$WORKING_PATH"/"$TIMINGS_PATH" -o "$WORKING_PATH"/"$GRAPHS_PATH"/"$GRAPH_FILE_NAME"

echo "Graph has been created."

#create spreadsheet
python "$PROCESSING_SCRIPT_PATH"/create_spreadsheet.py -i "$WORKING_PATH"/"$TIMINGS_PATH" -o "$WORKING_PATH"/"$SPREADSHEETS_PATH"/"$SPREADSHEET_FILE_NAME"

echo "Spreadsheet has been generated."

