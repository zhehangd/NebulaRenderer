ID=1
PREFIX=output$ID
mkdir $PREFIX
for i in $(seq 300 10 420)
do
  FILENAME=$(printf "%s/frame%03d.ppm" $PREFIX $i)
  echo $FILENAME
  ./nren material.vbf lighting.vbf $i $FILENAME
done

