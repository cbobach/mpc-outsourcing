#!/usr/bin/python

import sys
import collections

def main():
    stats = {
        'preprocess': 4,
        'solder': 5,
        'prepare': 6,
        'eval': 7,
        'decode': 8
        }

    in_file = sys.argv[1]

    for key, val in stats.items():
        out_file = in_file + '_' + key

        plots = calc_plots( in_file, out_file, val)

        write_plots( out_file, plots[0] )
        write_avg( out_file, plots[1] )
        write_frac( out_file, plots[2] )

def calc_plots( in_file, out_file, col ):
    plots = {}
    plots_avg = {}
    plots_frac = {}

    f = open( in_file + '.csv', 'r')

    print("Reading from file: " + f.name)

    for line in f:
        s = line.replace(" ", "")
        values = s.split(",")

        tag = int(values[0])
        aes = int(values[1])

        plot = int(values[col])

        plots.setdefault(aes, {})
        plots[aes].setdefault(tag, [])

        plots[aes][tag].append(plot)

    f.close

    for aes in plots:
        if 0 in plots[aes] and 1 in plots[aes]:
            tags_0 = plots[aes][0]
            tags_1 = plots[aes][1]
            tag_avg = sum(tags_1)/len(tags_1) - sum(tags_0)/len(tags_0)
            plots_avg[aes] = tag_avg

        if sum(tags_0)/len(tags_0) != 0:
            tag_frac =  tag_avg / float( sum(tags_1)/len(tags_1) ) * 100
            plots_frac[aes] = tag_frac

    plots_frac = collections.OrderedDict(sorted(plots_frac.items()))

    return [plots, plots_avg, plots_frac]

def write_avg( out_file, values ):
    file = open( out_file + '_avg.csv', 'w' )

    for key, value in values.items():
        file.write( str(key) + ', ' + str(value) + '\n' )

    file.close

    print 'Wrote AVG to file: ' + out_file + '_avg.csv'

def write_frac( out_file, values ):
    file = open( out_file + '_frac.csv', 'w' )

    for key, value in values.items():
        file.write( str(key) + ', ' + str(value) + '\n' )

    file.close

    print 'Wrote FRACTIONS to file: ' + out_file + '_frac.csv'

def write_plots( out_file, values ):
    file = open( out_file + '_plots.csv', 'w' )

    for key, value in values.items():
        if 1 in value:
            for val in value[1]:
                file.write( str(key) + ', ' + str(val) + '\n' )

    file.close

    print 'Wrote PLOTS to file: ' + out_file + '_plots.csv'

main()
