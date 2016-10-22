ID=1
PREFIX=output$ID
mkdir $PREFIX
for i in $(seq 0 10 360)
do
  FILENAME=$(printf "%s/frame%03d.ppm" $PREFIX $i)
  echo $FILENAME
  ./nren material.vbf lighting.vbf $i $FILENAME
done

