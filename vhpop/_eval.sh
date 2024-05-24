#!/bin/bash
echo "Running evaluation"

# Assign variables from command line arguments
folder="$1"

# Iterate through every subfolder of this folder
for domainFolder in $folder/*; do
    domain=$(basename $domainFolder)
    echo "Evaluating domain $domain"
    # Loop through numbers 1 to 22
    for ((i = 1; i <= 22; i++)); do
        # instances are called pfile1 to pfile22
        instanceName="pfile$i.pddl"
        # Check if file exists, if not, break
        if [ ! -f "$domainFolder/$instanceName" ]; then
            break
        fi
        echo "Evaluating problem $i"
        # Run the planner on the instance
        ./vhpop -h ILP -f LCFR -l 10000 -f MW -l unlimited -T 5 $domainFolder/domain.pddl $domainFolder/$instanceName
    done
done