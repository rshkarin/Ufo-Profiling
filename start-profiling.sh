#!/bin/bash

#MACHINES=( "ipepdvcompute1" "ipepdvcompute2" "ipepdvcompute3" )
#GPUS=( "NVIDIA GeForce GTX 680" "NVIDIA GeForce GTX TITAN" "AMD Radeon R9 290 Series" )
#PREFIXES=( "CUDA_VISIBLE_DEVICES=0" "CUDA_VISIBLE_DEVICES=2" "COMPUTE=:0" )

MACHINES=( "ipecluster3" )
GPUS=( "NVIDIA TITAN" )
PREFIXES=( ""  )


UFO_PACKAGE_NAME="ufo-profiling-package"
UFO_PROFILING_SCRIPT="ufo-installation.sh"
UFO_GENERATING_SCRIPT="generate-stuff.sh"

#declarations
get_remote_access () {
   # ssh -X "$1"
   # rm -rf "${HOME}"/"$2"
   # exit
	sshpass -p rfybreks scp -r "$2" "$1":"${HOME}"
	ssh -X "$1"
	cd "${HOME}"/"$2"
	bash "$3"
}

get_local_access() {
    rm -rf "${HOME}"/"$1"
 	cp -r "$1" "${HOME}"
	bash "${HOME}"/"$1"/"$2" "$3" "$4" "$5" "$6"
}

#copy ufo-profiliqng-package and install
CURRENT_TIME_STAMP=$(date +%Y%m%d_%H%M%S)
for ((i=0;i<${#MACHINES[@]};++i)); do
 #get_remote_access "${MACHINES[i]}" "$UFO_PACKAGE_NAME" "$UFO_PROFILING_SCRIPT" "${GPUS[i]// /_}" "$CURRENT_TIME_STAMP" "YES" "YES" "${PREFIXES[i]}" &
 get_local_access "$UFO_PACKAGE_NAME" "$UFO_PROFILING_SCRIPT" "${GPUS[i]// /_}" "$CURRENT_TIME_STAMP" "YES" "YES" "${PREFIXES[i]}" &
done

wait

bash "$UFO_GENERATING_SCRIPT" "$CURRENT_TIME_STAMP"

echo "Profiling is finished!"

