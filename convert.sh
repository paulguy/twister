rm *.data *.gif
./main
for IMAGE in *.data; do
	BASENAME=$(basename $IMAGE .data)
	PNGNAME=${BASENAME}.gif
	FINALNAME=pal${BASENAME}.gif
	echo $BASENAME
	convert -size 100x450 -depth 8 gray:$IMAGE -flip -rotate 90 $PNGNAME
	./mapcolors -f colormap.txt $PNGNAME $PNGNAME
	convert $PNGNAME -ordered-dither o4x4,4 $FINALNAME
done
gifsicle -O3 --delay=10 --loop palout*.gif >anim.gif
#apngasm anim.png palout0000.png 1 10
