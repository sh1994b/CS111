#! /usr/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2b_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#	8. average wait-for-lock
#
# output:
#	lab2b_1.png ... thruput vs. num of threads for mutex and spin-lock operations
#	lab2b_2.png ... mean time per mutex wait and mean time per operation for mutex
#	lab2b_3.png ... successful iterations vs. threads for each sync method
#	lab2b_4.png ... throughput vs. num of threads for mutex sync partitioned lists
#	lab2b_5.png ... throughput vs. num of threads for spin-lock sync part. lists
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","

# total number of operations per second for each synchronization method
set title "List-1: Throughput vs. Number of Threads"
set xlabel "Number of Threads"
set logscale x 2 
set ylabel "Throughput (operations/s)"
set logscale y 10
set output 'lab2b_1.png'

# mutex and spin-lock protected, non-yield results
plot \
     "< grep 'list-none-m,.*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'mutex-protected' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,.*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7))  \
	title 'spin-lock-protected' with linespoints lc rgb 'green'


set title "List-2: Wait-for-Lock and Average Time per Operation vs. Number of Threads"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Time (ns)"
set logscale y 10
set output 'lab2b_2.png'
plot \
     "< grep list-none-m lab2b_list.csv" using ($2):($8) \
	title 'wait-for-lock' with linespoints lc rgb 'green', \
     "< grep list-none-m lab2b_list.csv" using ($2):($7) \
	title 'per operation' with linespoints lc rgb 'red'


set title "List-3: Successful Iterations vs. Threads for Each Synchronization Method"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Successful Iterations"
set logscale y 10
set output 'lab2b_3.png'
plot \
     "< grep list-id-none lab2b_list.csv" using ($2):($3) \
	title 'non-protected' with linespoints lc rgb 'green', \
     "< grep list-id-m lab2b_list.csv" using ($2):($3) \
	title 'mutex-protected' with linespoints lc rgb 'red', \
     "< grep list-id-s lab2b_list.csv" using ($2):($3) \
	title 'spin-lock-protected' with linespoints lc rgb 'blue'


set title "List-4: Throughput vs. Number of Threads for Mutex-Protected Partitioned lists"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Throughput (operations/s)"
set logscale y 10
set output 'lab2b_4.png'
plot \
     "< grep list-none-m,.*,1000,1 lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '1 list' with linespoints lc rgb 'green', \
     "< grep list-none-m,.*,1000,4 lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '4 lists' with linespoints lc rgb 'red', \
     "< grep list-none-m,.*,1000,8 lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '8 lists' with linespoints lc rgb 'blue', \
     "< grep list-none-m,.*,1000,16 lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '16 lists' with linespoints lc rgb 'violet'


set title "List-5: Throughput vs. Number of Threads for Spin-Lock-Protected Partitioned lists"
set xlabel "Number of Threads"
set logscale x 2
set ylabel "Throughput (operations/s)"
set logscale y 10
set output 'lab2b_5.png'
plot \
     "< grep list-none-s,.*,1000,1 lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '1 list' with linespoints lc rgb 'green', \
     "< grep list-none-s,.*,1000,4 lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '4 lists' with linespoints lc rgb 'red', \
     "< grep list-none-s,.*,1000,8 lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '8 lists' with linespoints lc rgb 'blue', \
     "< grep list-none-s,.*,1000,16 lab2b_list.csv" using ($2):(1000000000/($7)) \
	title '16 lists' with linespoints lc rgb 'violet'








