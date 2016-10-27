mkdir output1
for i in $(seq 401 599 )
do
  angle=$((i))
  dist=$((300-i/2))
  filename=$(printf output1/frame%03d.ppm $i)
  ./nren \
  -e "renderer_open_material material.vbf" \
  -e "renderer_open_lighting lighting.vbf" \
  -e "renderer_setup_canvas  160 120" \
  -e "renderer_setup_extinct Emission   0.1,0.1,1,0,0" \
  -e "renderer_setup_extinct Reflection 0.1,0.1,0,1,0" \
  -e "renderer_camera_extrinsic Polar $angle 20 $dist" \
  -e "renderer_camera_intrinsic 1.2 600" \
  -e "renderer_draw_cube 100" \
  -e "renderer_draw_volume 1" \
  -e "renderer_save_canvas $filename"
done