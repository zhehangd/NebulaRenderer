# Compile the project.
make
# Generate a nebula volume (material.vbf).
./ngen config-generation
# Precompute the lighting volume (lighting.vbf).
./nren config-lighting
# Render.
./nren config-rendering
