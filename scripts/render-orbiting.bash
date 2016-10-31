#!/bin/bash

# ------------------------------------
# UNDER CONSTRUCTION, NOT WORKING YET
# ------------------------------------


Filename=$1



CameraMotion=($(./scripts/camera-motion.bash $2 $3))
CameraPos=(${CameraMotion[0]} ${CameraMotion[1]} ${CameraMotion[2]})
CameraYaw=$(bc <<< "scale=3; ($2)/($3)*360")

echo "[render-orbiting] $CameraYaw"

#-e "renderer_setup_canvas  854 480" \

./nren \
-e "renderer_open_material material.vbf" \
-e "renderer_open_lighting lighting.vbf" \
-e "renderer_setup_extinct Emission   0.1,0.1,1,0,0" \
-e "renderer_setup_extinct Reflection 0.1,0.1,0,1,0" \
-e "renderer_setup_canvas  854 480" \
-e "renderer_camera_intrinsic 2.2 800" \
-e "renderer_camera_extrinsic Polar $CameraYaw 20 600" \
-e "renderer_draw_cube 100" \
-e "renderer_draw_volume 1" \
-e "renderer_save_canvas $Filename.ppm" \