#!/bin/bash
#for name in "facebookcombined" "SSSP-graph0" "Email-EuAll" "WikiTalk"
for name in "SSSP-graph1"
do
        for i in 1 2 3
        do
                ./make_and_run.sh ../Input/$name ../Output/out
        done
done
