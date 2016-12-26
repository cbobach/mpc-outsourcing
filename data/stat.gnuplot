set datafile separator ","
set term png
set grid ytics lt 0 lw 1 lc rgb "#bbbbbb"
set grid xtics lt 0 lw 1 lc rgb "#bbbbbb"
set xlabel "#AES of composed"
set autoscale

#---EVALUATOR---
#PREPROCESS
set title "Evaluator - preprocess (ms)"
set output "plots/eval_preprocess_plots.png"
plot "csv/eval_preprocess_plots.csv" using 1:2 t "composed", 35*x+2100
set output "plots/eval_preprocess_avg.png"
plot "csv/eval_preprocess_avg.csv" using 1:2 t "avg tag"

set title "Evaluator - preprocess (%)"
set output "plots/eval_preprocess_frac.png"
plot "csv/eval_preprocess_frac.csv" using 1:2 w lines t "avg tag/total"

#SOLDER
set title "Evaluator - solder (ms)"
set output "plots/eval_solder_plots.png"
plot "csv/eval_solder_plots.csv" using 1:2 t "composed", 0.08*x

set output "plots/eval_solder_avg.png"
plot "csv/eval_solder_avg.csv" using 1:2 t "avg tag"

set title "Evaluator - solder (%)"
set output "plots/eval_solder_frac.png"
plot "csv/eval_solder_frac.csv" using 1:2 w lines t "avg tag/total"

#PREPARE
set title "Evaluator - prepare (ms)"
set output "plots/eval_prepare_plots.png"
plot "csv/eval_prepare_plots.csv" using 1:2 t "composed",  20500

set output "plots/eval_prepare_avg.png"
plot "csv/eval_prepare_avg.csv" using 1:2 t "avg tag"

set title "Evaluator - prepare (%)"
set output "plots/eval_prepare_frac.png"
plot "csv/eval_prepare_frac.csv" using 1:2 w lines t "avg tag/total"

#EVAL
set title "Evaluator - evaluate (ms)"
set output "plots/eval_eval_plots.png"
plot "csv/eval_eval_plots.csv" using 1:2 t "composed", 1.2*x+40

set output "plots/eval_eval_avg.png"
plot "csv/eval_eval_avg.csv" using 1:2 t "avg tag"

set title "Evaluator - evaluate (%)"
set output "plots/eval_eval_frac.png"
plot "csv/eval_eval_frac.csv" using 1:2 w lines t "avg tag/total"

#DECODE
set title "Evaluator - decode (ms)"
set output "plots/eval_decode_plots.png"
plot "csv/eval_decode_plots.csv" using 1:2 t "composed", 0.02*x-0.5

set output "plots/eval_decode_avg.png"
plot "csv/eval_decode_avg.csv" using 1:2 t "avg tag"

set title "Evaluator - decode (%)"
set output "plots/eval_decode_frac.png"
plot "csv/eval_decode_frac.csv" using 1:2 w lines t "avg tag/total"

#---CONSTRUCTOR---
#PREPROCESS
set title "Constructor - preprocess (ms)"
set output "plots/const_preprocess_plots.png"
plot "csv/const_preprocess_plots.csv" using 1:2 t "composed", 35*x+2100

set output "plots/const_preprocess_avg.png"
plot "csv/const_preprocess_avg.csv" using 1:2 t "avg tag"

set title "Constructor - preprocess (%)"
set output "plots/const_preprocess_frac.png"
plot "csv/const_preprocess_frac.csv" using 1:2 w lines t "avg tag/total"

#SOLDER
set title "Constructor - solder (ms)"
set output "plots/const_solder_plots.png"
plot "csv/const_solder_plots.csv" using 1:2 t "composed", 0.7*x

set output "plots/const_solder_avg.png"
plot "csv/const_solder_avg.csv" using 1:2 t "avg tag"

set title "Constructor - solder (%)"
set output "plots/const_solder_frac.png"
plot "csv/const_solder_frac.csv" using 1:2 w lines t "avg tag/total"

#PREPARE
set title "Constructor - prepare (ms)"
set output "plots/const_prepare_plots.png"
plot "csv/const_prepare_plots.csv" using 1:2 t "composed", 20500

set output "plots/const_prepare_avg.png"
plot "csv/const_prepare_avg.csv" using 1:2 t "avg tag"

set title "Constructor - prepare (%)"
set output "plots/const_prepare_frac.png"
plot "csv/const_prepare_frac.csv" using 1:2 w lines t "avg tag/total"

#EVAL
set title "Constructor - evaluate (ms)"
set output "plots/const_eval_plots.png"
plot "csv/const_eval_plots.csv" using 1:2 t "composed", 1.2*x+40

set output "plots/const_eval_avg.png"
plot "csv/const_eval_avg.csv" using 1:2 t "avg tag"

set title "Constructor - evaluate (%)"
set output "plots/const_eval_frac.png"
plot "csv/const_eval_frac.csv" using 1:2 w lines t "avg tag/total"

#DECODE
set title "Constructor - decode (ms)"
set output "plots/const_decode_plots.png"
plot "csv/const_decode_plots.csv" using 1:2 t "composed", 0.02*x-1

set output "plots/const_decode_avg.png"
plot "csv/const_decode_avg.csv" using 1:2 t "avg tag"

set title "Constructor - decode (%)"
set output "plots/const_decode_frac.png"
plot "csv/const_decode_frac.csv" using 1:2 w lines t "avg tag/total"

#TOTAL
set title "Tag time - upper bound (%)"
set output "plots/total_frac.png"
plot [0:10000] 300/(36*x+22500)*100 t "avg tag/total"
