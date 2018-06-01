compile:
	gcc ImageFilter.c -o ImageFilter.out -lm -lpthread

run:
	./ImageFilter.out 8 exampleImages/casablanca.ascii.pgm generateFilter/RAND.filter outputImage/FilteredImage.ascii.pgm

times:
	touch Times.txt
	echo "This file contains data from ImageFilter program" > Times.txt
	./ImageFilter.out 1 exampleImages/casablanca.ascii.pgm generateFilter/RAND.filter outputImage/FilteredImage.ascii.pgm >> Times.txt
	echo " " >> Times.txt
	./ImageFilter.out 2 exampleImages/casablanca.ascii.pgm generateFilter/RAND.filter outputImage/FilteredImage.ascii.pgm >> Times.txt
	echo " " >> Times.txt
	./ImageFilter.out 4 exampleImages/casablanca.ascii.pgm generateFilter/RAND.filter outputImage/FilteredImage.ascii.pgm >> Times.txt
	echo " " >> Times.txt
	./ImageFilter.out 8 exampleImages/casablanca.ascii.pgm generateFilter/RAND.filter outputImage/FilteredImage.ascii.pgm >> Times.txt
