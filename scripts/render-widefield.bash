#!/bin/bash

# ------------------------------------
# UNDER CONSTRUCTION, NOT WORKING YET
# ------------------------------------


Filename=$1
FOV=120
CameraMotion=($(./scripts/camera-motion.bash $2 $3))
CameraPos=(${CameraMotion[0]} ${CameraMotion[1]} ${CameraMotion[2]})
CameraYaw=${CameraMotion[3]}

Scale="scale=7"
# deg-to-rad coefficient for later computation.
Deg2Rad=$(bc <<< "$Scale;3.1416/180")
# Compute the focus.
Focus=$(bc -l <<< "$Scale;v=$FOV/6*$Deg2Rad;c(v)/s(v)")

Width=854
Height=480

echo "[script-widefield] ${CameraPos[*]} $CameraYaw"

for k in $(seq 0 2)
do
  ViewIdx=$((1-k))
  SubYaw=$(bc <<< "$Scale;($CameraYaw + $ViewIdx * $FOV / 3) * $Deg2Rad;")
  SubDir=($(bc -l <<< "$Scale;c($SubYaw);") 0 $(bc -l <<< "$Scale;-s($SubYaw);"))
  for i in 0 1 2; do
    SubPos[$i]=$(bc <<< "${CameraPos[$i]} + ${SubDir[$i]}*$Focus")
    SubAt[$i]=$( bc <<< "${SubPos[$i]}    + ${SubDir[$i]}")
  done
  # Replace space by comma.
  SubPos=$(echo ${SubPos[*]} | tr " "  ",")
  SubAt=$( echo ${SubAt[*]}  | tr " "  ",")
  # Resolution.
  if [ $k -eq 1 ];then
    SubWidth=$(($Width - $Width / 3 * 2))
  else
    SubWidth=$(($Width / 3))
  fi
  SubHeight=$Height
  ./nren \
  -e "renderer_open_material material.vbf" \
  -e "renderer_open_lighting lighting.vbf" \
  -e "renderer_setup_extinct Emission   0.1,0.1,1,0,0" \
  -e "renderer_setup_extinct Reflection 0.1,0.1,0,1,0" \
  -e "renderer_setup_canvas  $SubWidth $SubHeight" \
  -e "renderer_camera_intrinsic $Focus 600" \
  -e "renderer_camera_extrinsic Rect $SubPos $SubAt 0,1,0" \
  -e "renderer_set_spectrum 1.0,0.3,0.6 0.2,0.3,0.5" \
  -e "renderer_draw_skybox skybox.ppm" \
  -e "renderer_invert_tonemapping" \
  -e "renderer_draw_volume 1" \
  -e "renderer_tonemapping" \
  -e "renderer_save_canvas view-$$-$k.ppm" \
  
done

convert view-$$-0.ppm view-$$-1.ppm view-$$-2.ppm +append $Filename
rm view-$$-0.ppm view-$$-1.ppm view-$$-2.ppm
