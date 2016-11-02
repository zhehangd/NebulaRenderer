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
-e "renderer_setup_extinct Emission   0.1,0.2,1,0,0" \
-e "renderer_setup_extinct Reflection 0.1,0.1,0,1,0" \
-e "renderer_setup_canvas  854 480" \
-e "renderer_camera_intrinsic 2 800" \
-e "renderer_camera_extrinsic Polar $CameraYaw 20 300" \
-e "renderer_set_spectrum 1.0,0.3,0.6 0.2,0.3,0.5" \
-e "renderer_draw_volume 1" \
-e "renderer_save_canvas $Filename.ppm" \