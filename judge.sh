#!/usr/bin/bash

# Read the list from /testcases/config
echo -e "\033[1m====================================================================="
echo "                              Config                                  "
echo -e "=====================================================================\033[0m"

readarray -t list < ./testcases/config

# Print each item in the list
echo "List of testcases, total: ${#list[@]}"
for item in "${list[@]}"; do
    echo "- $item"
done

# Create a list to store failed testcases
failed_list=()



# Compile the code
echo -e "\033[1m====================================================================="
echo "                            Compileing                               "
echo -e "=====================================================================\033[0m"
# rm -rf ./tmpdir
mkdir tmpdir
cmake -S . -B tmpdir
make -C tmpdir

if [ $? -eq 0 ]; then
    echo -e "\033[1m\033[32mCompile success\033[0m"
else
    echo -e "\033[1m\033[31mCompile failed\033[0m"
    exit 1
fi


# Run the testcases
echo -e "\033[1m====================================================================="
echo "                         Running testcases                           "
echo -e "=====================================================================\033[0m"

for item in "${list[@]}"; do
    echo "Running test[$item]"
    if [ ! -f "./testcases/$item.ans" ]; then
        echo "Testcase $item.ans not found, generating..."
        gcc -o tmp ./testcases/$item.c
        ./tmp
        echo $? > ./testcases/$item.ans
        rm ./tmp
    fi
    start_time=$(date +%s%3N)
    ./tmpdir/code < ./testcases/$item.data > ./testcases/$item.out
    end_time=$(date +%s%3N)
    elapsed_time=$((end_time - start_time))
    elapsed_time_ms=$(echo "scale=3; $elapsed_time / 1000" | bc)
    echo -e "\033[1mElapsed time: \033[35m ${elapsed_time_ms} seconds\033[0m"
    diff -Z ./testcases/$item.out ./testcases/$item.ans 
    if [ $? -eq 0 ]; then
        echo -e "\033[32m Testcase $item passed\033[0m"
    else
        echo -e "\033[31m Testcase $item failed\033[0m"
        # Add the failed testcase to a list
        failed_list+=("$item")
    fi
    echo -e "\033[2m-------------------------------------------\033[0m"

done

# Print the failed testcases
echo -e "\033[1m====================================================================="
echo "                             Show result                             "
echo -e "=====================================================================\033[0m"
echo -e "Finished running testcases, failed count: \033[35m ${#failed_list[@]}\033[0m"
for item in "${failed_list[@]}"; do
    echo "- $item"
done