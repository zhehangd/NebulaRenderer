#!/bin/bash

# ------------------------------------
# UNDER CONSTRUCTION, NOT WORKING YET
# ------------------------------------

# A bash script to be called.
# Four arguments will be given.
# 1. Current frame index.
# 2. Total number of frames.
# 3. Current task index.
# 4. Total number of tasks.
runFile=$1
# The number of frames to produce.
nFrames=$2
# The number of tasks are work parallelly.
nTasks=$3

echo "[make-sequence] runFile = $runFile"
echo "[make-sequence] nFrames = $nFrames"
echo "[make-sequence] nTasks  = $nTasks"

function task()
{
  # Render the frames.
  for frameIdx in $(seq $taskIdx $nTasks $(($nFrames-1)) )
  do
    filename=$(printf "output/frame%04d.ppm" $frameIdx)
    echo "[make-sequence] frame $frameIdx/$(($nFrames-1)) @ $taskIdx/$nTasks"
    ./$runFile $filename $frameIdx $nFrames
  done
}

# Execute the tasks in background.
for taskIdx in $(seq 0 $(($nTasks-1)))
do
  task $taskIdx &
done