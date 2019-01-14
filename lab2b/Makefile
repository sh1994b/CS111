
C = gcc
CFLAGS = -g -Wall -Wextra -lpthread -lprofiler
.SILENT:

default: lab2_list
lab2_list: lab2_list.c SortedList.c
	$(CC) $(CFLAGS) -o lab2_list lab2_list.c SortedList.c

tests: default
	#tests for images 1 & 2 
	rm -f lab2b_list.csv
	#/lab2_list --threads=1 --iterations=1000 >> lab2b_list.csv
	#./lab2_list --threads=1 --iteration=1000 --sync=m >> lab2b_list.csv
	#/lab2_list --threads=2 --iteration=1000 --sync=m >> lab2b_list.csv
	#./lab2_list --threads=4 --iteration=1000 --sync=m >> lab2b_list.csv
	#./lab2_list --threads=8 --iteration=1000 --sync=m >> lab2b_list.csv
	#/lab2_list --threads=12 --iteration=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=16 --iteration=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=24 --iteration=1000 --sync=m >> lab2b_list.csv
	#./lab2_list --threads=1 --iteration=1000 --sync=s >> lab2b_list.csv
	#./lab2_list --threads=2 --iteration=1000 --sync=s >> lab2b_list.csv
	#./lab2_list --threads=4 --iteration=1000 --sync=s >> lab2b_list.csv
	#./lab2_list --threads=8 --iteration=1000 --sync=s >> lab2b_list.csv
	#./lab2_list --threads=12 --iteration=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=16 --iteration=1000 --sync=s >> lab2b_list.csv
	./lab2_list --threads=24 --iteration=1000 --sync=s >> lab2b_list.csv
	#how many iterations it  takes to reliably fail (image 3)
	./lab2_list --lists=4 --threads=1 --iterations=1 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=1 --iterations=2 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=1 --iterations=4 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=1 --iterations=8 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=1 --iterations=16 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=4 --iterations=1 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=4 --iterations=2 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=4 --iterations=4 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=4 --iterations=8 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=4 --iterations=16 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=8 --iterations=1 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=8 --iterations=2 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=8 --iterations=4 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=8 --iterations=8 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=8 --iterations=16 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=1 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=2 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=4 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=8 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=12 --iterations=16 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=1 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=2 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=4 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=8 --yield=id >> lab2b_list.csv
	-./lab2_list --lists=4 --threads=16 --iterations=16 --yield=id >> lab2b_list.csv
			
	./lab2_list --lists=4 --threads=1 --iterations=10 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=1 --iterations=20 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=1 --iterations=40 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=1 --iterations=80 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=4 --iterations=10 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=4 --iterations=20 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=4 --iterations=40 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=4 --iterations=80 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=8 --iterations=10 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=8 --iterations=20 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=8 --iterations=40 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=8 --iterations=80 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=12 --iterations=10 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=12 --iterations=20 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=12 --iterations=40 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=12 --iterations=80 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=16 --iterations=10 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=16 --iterations=20 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=16 --iterations=40 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=16 --iterations=80 --yield=id --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=1 --iterations=10 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=1 --iterations=20 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=1 --iterations=40 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=1 --iterations=80 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=4 --iterations=10 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=4 --iterations=20 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=4 --iterations=40 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=4 --iterations=80 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=8 --iterations=10 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=8 --iterations=20 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=8 --iterations=40 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=8 --iterations=80 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=12 --iterations=10 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=12 --iterations=20 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=12 --iterations=40 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=12 --iterations=80 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=16 --iterations=10 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=16 --iterations=20 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=16 --iterations=40 --yield=id --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=16 --iterations=80 --yield=id --sync=s >> lab2b_list.csv
	
	#data for image 4
	./lab2_list --lists=1 --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=8 --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=16 --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=1 --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=8 --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=16 --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv		
	./lab2_list --lists=1 --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=8 --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=16 --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=1 --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=8 --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=16 --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=1 --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=4 --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=8 --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --lists=16 --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
										
	./lab2_list --lists=1 --threads=1 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=1 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=8 --threads=1 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=16 --threads=1 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=1 --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=8 --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=16 --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv		
	./lab2_list --lists=1 --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=8 --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=16 --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=1 --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=8 --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=16 --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=1 --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=4 --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=8 --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
	./lab2_list --lists=16 --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
	
	
profile: default
	LD_PRELOAD=/u/cs/ugrad/shabnam/lib/libprofiler.so.0
	CPUPROFILE=./raw.gperf ./lab2_list --iterations=1000 --threads=12 --sync=s
	pprof --text ./lab2_list ./raw.gperf > profile.out
	pprof --list=threadFunc ./lab2_list ./raw.gperf >> profile.out
	rm -f ./raw.gperf profile.gperf lab2_list

graphs: tests
	gnuplot lab2b_list.gp

clean: 
	-rm -f lab2_list
	-rm -f *.tar.gz
	
dist: graphs profile
	tar -zcf lab2b-104950171.tar.gz  lab2_list.c \
	SortedList.h SortedList.c Makefile lab2b_list.csv \
	lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png \
	lab2b_5.png README profile.out lab2b_list.gp

