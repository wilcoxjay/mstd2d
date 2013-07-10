#!/bin/bash
# input contains entries of the form:

# --
# finished searching {
#    (1/2, ..., ),
#    ...
#    (3/4, ..., ) | 5
# }

# the following sed script munges those entries into the form:

# (1/2, ...,), ..., (3/4, ..., ) 

sed '/finished/d
     N;N;N;N;N;N;N
     s/\n/ /g
     s/|.*//
     s/[[:space:]][[:space:]]*/ /g'
