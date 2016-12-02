#!/bin/bash
# Based on the frame instruction, return the camera position and orientation.
# Users should modified the content to satisfy their design.

# Takes two arguments:
# 1. Current frame index starting from 0.
iFrames=$1
# 2. Total number of frames.
nFrames=$2


# Currently the most simple motion is implemented.
# The camera position is linearly interpolated between the first and the last frames.
# The horizontal orientation of the camera is similarly interpolated, 
# but the start and end time can be specified.

# Position
#posValSrt=(150 -30 10)
#posValEnd=(-100 -30 10)
posValSrt=(200 -30  -10)
posValEnd=(-160 -30 -10)
posTimSrt=0
posTimEnd=$nFrames

# Orientation
oriValSrt=190
oriValEnd=170
oriTimSrt=0
oriTimEnd=$(($nFrames))

# Interpolate the position.
if [ $iFrames -lt $posTimSrt ]; then
  for i in $(seq 0 2); do
      posInterp[$i]=${posValSrt[$i]}
  done
else
  if [ $iFrames -ge $posTimEnd ]; then
    for i in $(seq 0 2); do
      posInterp[$i]=${posValEnd[$i]}
    done
  else
    for i in $(seq 0 2); do
      posInterp[$i]=$(bc <<< "scale=4;(${posValEnd[$i]}-(${posValSrt[$i]}))*($iFrames-$posTimSrt)/($posTimEnd-$posTimSrt-1)+(${posValSrt[$i]})")
    done
  fi
fi 

# Interpolate the orientation.
if [ $iFrames -lt $oriTimSrt ]; then
  oriInterp[$i]=${oriValSrt[$i]}
else
  if [ $iFrames -ge $oriTimEnd ]; then
    oriInterp[$i]=${oriValEnd[$i]}
  else
    oriInterp=$(bc <<< "scale=4;(${oriValEnd}-(${oriValSrt}))*($iFrames-($oriTimSrt))/(($oriTimEnd)-($oriTimSrt)-1)+(${oriValSrt})")
  fi
fi

# Output the position and the orientation.
echo ${posInterp[*]} $oriInterp