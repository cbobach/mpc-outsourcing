#!/bin/bash

# -e tagget specificere antal traade, til preprocessing, prepare og evaluation
# paa en baerbar er 8-16 fint, ellers op mod 500, dog hoejest anta circuits.
# En traad pr. circuit.
# Kan fejle pa lave tal. Bliver bedre paa store circuits.
#
# Ved at koere -e 1,1,1 bliver composed circuit set som et der ikke kan optimers
# derved opfattes aes delen som en stor udregning og derfor tungere. Men dette er
# sjaelend tilfaeledet i virkeliheden.

echo "Running timing script for MPC-outsourcing:";
echo ""

PORT=28001

NUM_TAG=$1
NUM_AES=$2

function clean_up {
    if (( SIZE>0 ))
    then
        killall -9 MPCOutsourcingEvaluator
        killall -9 MPCOutsourcingConstructor
    else
        break
    fi

    echo ""
    echo "Moving data for plotting"
    cp const.timing csv/const.csv
    wait $!

    cp eval.timing csv/eval.csv
    wait $!

    echo ""
    echo "Generating plots"
    ./csv/stat.py csv/eval
    wait $!

    ./csv/stat.py csv/const
    wait $!

    gnuplot stat.gnuplot

    echo ""
    echo "Moving data to project folder"

    cp -r ./* ~/Projektfag\ i\ datalogi/data/

    echo ""
    echo "Exitting, cleaning up"

    exit
}

function restart {
    PORT=28001

    if (( SIZE>0 ))
    then
        killall -9 MPCOutsourcingEvaluator
        killall -9 MPCOutsourcingConstructor
    else
        break
    fi

    echo "---! RESTARTING WITH VALUES: #AES: $NUM_AES, PORT: $PORT !---"

    singelthreaded
}

function is_running {

    COUTN=0
    while true
    do
        LIST=$(top -b -n 1 |grep 'MPC')
        SIZE=${#LIST}

        if (( SIZE>0 ))
        then
            if (( COUNT<60 ))
            then
                COUNT=$((COUNT+1))
                sleep 1
            else
                COUNT=0
                restart
            fi
        else
            COUNT=0
            echo ""
            break
        fi
    done
}

function singelthreaded {

    echo "___ SINGLETHREADED: #TAG: $NUM_TAG, #AES: $NUM_AES, PORT: $PORT ___"

    ../build/release/MPCOutsourcingConstructor -c outsc -e 1,1,1 -ip localhost -p $PORT -t $NUM_TAG -a $NUM_AES & PIDCONST=$!
    ../build/release/MPCOutsourcingEvaluator -c outsc -e 1,1,1 -ip localhost -p $PORT -t $NUM_TAG -a $NUM_AES & PIDEVAL=$!

    is_running
}

function run {
    while ((NUM_AES <= 100))
    do
        singelthreaded

        PORT=$(($PORT+1))

        if ((NUM_AES == 100))
        then
          NUM_AES=0
        else
          NUM_AES=$(($NUM_AES+1))
        fi
    done
}

trap clean_up SIGINT

run

exit
