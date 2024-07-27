#!/usr/bin/bash

# Read the list from /testcases/config
echo "====================================================================="
echo "                             Config                                  "
echo "====================================================================="

readarray -t list < ./testcases/config

# Print each item in the list
echo "List of testcases, total: ${#list[@]}"
for item in "${list[@]}"; do
    echo "- $item"
done

# Create a list to store failed testcases
failed_list=()



# Compile the code
echo "====================================================================="
echo "                            Compileing                               "
echo "====================================================================="
mkdir tmpdir
cmake -S . -B tmpdir
make -C tmpdir

if [ $? -eq 0 ]; then
    echo "Compile success"
else
    echo "Compile failed"
    exit 1
fi


# Run the testcases
echo "====================================================================="
echo "                         Running testcases                           "
echo "====================================================================="

for item in "${list[@]}"; do
    echo "Running test[$item]"
    if [ ! -f "./testcases/$item.ans" ]; then
        echo "Testcase $item.ans not found, generating..."
        gcc -o tmp ./testcases/$item.c
        ./tmp
        echo $? > ./testcases/$item.ans
        rm ./tmp
    fi
    ./tmpdir/code < ./testcases/$item.data > ./testcases/$item.out
    diff -Z ./testcases/$item.out ./testcases/$item.ans 
    if [ $? -eq 0 ]; then
        echo " Testcase $item passed"
    else
        echo " Testcase $item failed"
        # Add the failed testcase to a list
        failed_list+=("$item")
    fi
    echo "-------------------------------------------"

done

# Print the failed testcases
echo "====================================================================="
echo "                             Show result                             "
echo "====================================================================="
echo "Finished running testcases, failed count: ${#failed_list[@]}"
for item in "${failed_list[@]}"; do
    echo "- $item"
done