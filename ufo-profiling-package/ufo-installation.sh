#! /usr/bin/bash

GPU_NAME="$1"
FOLDER_NAME="$2"
USE_GOOGLE_DRIVE="$3"
USE_DROPBOX="$4"
GPU_PREFIX="$5"

UFO_DIR_PATH="${HOME}/ufo-profiling-package"
UFO_FRAMEWORK_PATH="$UFO_DIR_PATH"/"ufo-framework"
UFO_PROFILING_PATH="${HOME}"/"ufo-profilig-data"
PROFILING_RAW_DATA_PATH="$UFO_PROFILING_PATH"/"profiling-raw-data"
PROFILING_DATA_PATH="$UFO_PROFILING_PATH"/"profiling-processed-data"
PROFILING_LOCAL_FOLDER="profiling-data"
UFO_LIB_PATH="${HOME}/ufo-local-profiling"
#PY_PATH="$UFO_DIR_PATH"/"python"
#PY_PATH="$HOME/.local"
PY_PATH="${HOME}/.local/lib/python2.7/site-packages:${HOME}/.local/lib64/python2.7/site-packages:/usr/lib/python2.7/site-packages"
UPLOADER_SCRIPT_PATH="$UFO_DIR_PATH"/"uploaders"
PROCESSING_SCRIPT_PATH="$UFO_DIR_PATH"/"processing"
METHOD_PATH="$UFO_DIR_PATH"/"methods"
METHOD_TITLES=( "ufo-test-scheduler-bp" "ufo-test-scheduler-dfi-os1" "ufo-test-scheduler-dfi-os2" )
UFO_PACKAGES=( "ufo-core" "ufo-art" "ufo-filters" )
#GRAPHS_PATH="graphics"
#SPREADSHEETS_PATH="spreadsheets"
TIMINGS_PATH="timings"
TEMP_FOLDER_NAME="tmp"
#GRAPH_FILE_NAME="graph.svg"
#SPREADSHEET_FILE_NAME="result.csv"

#removing subsystem and pull last versions
for UFO_PACKAGE in "${UFO_PACKAGES[@]}"; do
  if [ -d "$UFO_FRAMEWORK_PATH"/"$UFO_PACKAGE"/"$TEMP_FOLDER_NAME" ]; then
    rm -r "$UFO_FRAMEWORK_PATH"/"$UFO_PACKAGE"/"$TEMP_FOLDER_NAME"
  fi
  
  cd "$UFO_FRAMEWORK_PATH"/"$UFO_PACKAGE"
  git pull
done

#set evn variables
mkdir -p "$UFO_LIB_PATH"
export LD_LIBRARY_PATH="$UFO_LIB_PATH"/lib
export PKG_CONFIG_PATH="$UFO_LIB_PATH"/lib/pkgconfig
export PYTHONPATH="$PY_PATH"

#subsystems installation
for UFO_PACKAGE in "${UFO_PACKAGES[@]}"; do
  mkdir -p "$UFO_FRAMEWORK_PATH"/"$UFO_PACKAGE"/"$TEMP_FOLDER_NAME"
  cd "$UFO_FRAMEWORK_PATH"/"$UFO_PACKAGE"/"$TEMP_FOLDER_NAME"

  if [ "$UFO_PACKAGE" == "ufo-filters" ]; then
    cmake -D ENABLE_ART=OFF WITH_PROFILING=ON .. -DPREFIX="$UFO_LIB_PATH"
  elif [ "$UFO_PACKAGE" == "ufo-core" ]; then
    cmake -D WITH_GTK_DOC=OFF WITH_PROFILING=ON .. -DPREFIX="$UFO_LIB_PATH"
  else
    cmake .. -DPREFIX="$UFO_LIB_PATH"
  fi
  make && make install
done

#methods building and invocation
for METHOD_TITLE in "${METHOD_TITLES[@]}"; do
  mkdir -p "$METHOD_PATH"/"$METHOD_TITLE"/"$TEMP_FOLDER_NAME"
  cd "$METHOD_PATH"/"$METHOD_TITLE"/"$TEMP_FOLDER_NAME"
  cmake ..
  make
  cd "$METHOD_PATH"/"$METHOD_TITLE"/build
  eval "$GPU_PREFIX ./test_profiling"
done

#create folders for data
if [ ! -d "$PROFILING_RAW_DATA_PATH" ]; then
  mkdir -p "$PROFILING_RAW_DATA_PATH" 
fi

if [ ! -d "$PROFILING_DATA_PATH" ]; then
  mkdir -p "$PROFILING_DATA_PATH"
fi

mkdir -p "$PROFILING_RAW_DATA_PATH"/"$FOLDER_NAME"/"$TIMINGS_PATH"/"$GPU_NAME"
mkdir -p "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$TIMINGS_PATH"/"$GPU_NAME"

#if [ ! -d "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$GRAPHS_PATH" ]; then
#  mkdir -p "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$GRAPHS_PATH"
#fi

#if [ ! -d "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$SPREADSHEETS_PATH" ]; then
#  mkdir -p "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$SPREADSHEETS_PATH"
#fi

#copy profiling result of each method to "$PROFILING_RAW_DATA_PATH"
for METHOD_TITLE in "${METHOD_TITLES[@]}"; do
  RECENT_FILE=$(ls -A -t "$METHOD_PATH"/"$METHOD_TITLE"/build | grep '.opencl' | head -n 1)
  cp "$METHOD_PATH"/"$METHOD_TITLE"/build/"$RECENT_FILE" "$PROFILING_RAW_DATA_PATH"/"$FOLDER_NAME"/"$TIMINGS_PATH"/"$GPU_NAME"/"$METHOD_TITLE".txt
done

#process profiling raw data
RAW_DATA_FILES=( $(ls "$PROFILING_RAW_DATA_PATH"/"$FOLDER_NAME"/"$TIMINGS_PATH"/"$GPU_NAME" | grep '.txt') )
for FILE in "${RAW_DATA_FILES[@]}"; do
  python "$PROCESSING_SCRIPT_PATH"/measurer.py -i "$PROFILING_RAW_DATA_PATH"/"$FOLDER_NAME"/"$TIMINGS_PATH"/"$GPU_NAME"/"$FILE" -o "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$TIMINGS_PATH"/"$GPU_NAME"/"$FILE" -d "$GPU_NAME" &
done

wait

#draw plot
#python "$PROCESSING_SCRIPT_PATH"/graph_plot.py -i "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$TIMINGS_PATH" -o "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$GRAPHS_PATH"/"$GRAPH_FILE_NAME"

#create spreadsheet
#python "$PROCESSING_SCRIPT_PATH"/create_spreadsheet.py -i "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$TIMINGS_PATH" -o "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$SPREADSHEETS_PATH"/"$SPREADSHEET_FILE_NAME"

DATA_FILES=( $(ls "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$TIMINGS_PATH"/"$GPU_NAME" | grep '.txt') )

#upload files to local storage
for FILE in "${DATA_FILES[@]}"; do
  bash "$UPLOADER_SCRIPT_PATH"/local-uploader.sh "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$TIMINGS_PATH"/"$GPU_NAME"/"$FILE" "$PROFILING_LOCAL_FOLDER"/"$FOLDER_NAME"/"$TIMINGS_PATH"/"$GPU_NAME"
done

#bash "$UPLOADER_SCRIPT_PATH"/local-uploader.sh "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$GRAPHS_PATH"/"$GRAPH_FILE_NAME" "$PROFILING_LOCAL_FOLDER"/"$FOLDER_NAME"/"$GRAPHS_PATH"
#bash "$UPLOADER_SCRIPT_PATH"/local-uploader.sh "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$SPREADSHEETS_PATH"/"$SPREADSHEET_FILE_NAME" "$PROFILING_LOCAL_FOLDER"/"$FOLDER_NAME"/"$SPREADSHEETS_PATH"

#upload files to goole drive
#if [ "$USE_GOOGLE_DRIVE" == "YES" ]; then
#  for FILE in "${DATA_FILES[@]}"; do
#    python "$UPLOADER_SCRIPT_PATH"/"google-drive"/uploader.py -i "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$GPU_NAME"/"$FILE" -o "$PROFILING_LOCAL_FOLDER"/"$FOLDER_NAME"/"$GPU_NAME"
#  done
#
#  python "$UPLOADER_SCRIPT_PATH"/"google-drive"/uploader.py -i "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$GRAPHS_PATH"/"$GRAPH_FILE_NAME" -o "$PROFILING_LOCAL_FOLDER"/"$FOLDER_NAME"/"$GRAPHS_PATH"
#  python "$UPLOADER_SCRIPT_PATH"/"google-drive"/uploader.py -i "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$SPREADSHEETS_PATH"/"$SPREADSHEET_FILE_NAME" -o "$PROFILING_LOCAL_FOLDER"/"$FOLDER_NAME"/"$SPREADSHEETS_PATH"
#fi

#upload files to dropbox
#if [ "$USE_DROPBOX" == "YES" ]; then
#  for FILE in "${DATA_FILES[@]}"; do
#    python "$UPLOADER_SCRIPT_PATH"/"dropbox"/uploader.py -i "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$GPU_NAME"/"$FILE" -o "$PROFILING_LOCAL_FOLDER"/"$FOLDER_NAME"/"$GPU_NAME"
#  done
#
#  python "$UPLOADER_SCRIPT_PATH"/"dropbox"/uploader.py -i "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$GRAPHS_PATH"/"$GRAPH_FILE_NAME" -o "$PROFILING_LOCAL_FOLDER"/"$FOLDER_NAME"/"$GRAPHS_PATH"
#  python "$UPLOADER_SCRIPT_PATH"/"dropbox"/uploader.py -i "$PROFILING_DATA_PATH"/"$FOLDER_NAME"/"$SPREADSHEETS_PATH"/"$SPREADSHEET_FILE_NAME" -o "$PROFILING_LOCAL_FOLDER"/"$FOLDER_NAME"/"$SPREADSHEETS_PATH"
#fi
