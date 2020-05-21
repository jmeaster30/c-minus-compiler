TESTS=0
TESTS=(test-cases-codegen/*)
NUM_OF_TESTS=0
NUM_OF_TESTS=${#TESTS[@]}

SUCCESS=0

echo "Building Executable..."
./compile.sh
echo "Running tests..."
echo -n "" > failed.txt
mkdir cg_outputs
for f in "${TESTS[@]}"; do
	echo "****** Executing test $f ******"
	./run.sh $f cg_outputs/${f##*/}.s
	./run_sol.sh $f cg_outputs/${f##*/}.sico
	#python clear_whitespace.py cg_outputs/${f##*/}.s
        #python clear_whitespace.py cg_outputs/${f##*/}.sico
	spim -f cg_outputs/${f##*/}.s > my_results.txt
	spim -f cg_outputs/${f##*/}.sico > correct_results.txt
	RESULT=$(diff my_results.txt correct_results.txt)
	if [[ -n "$RESULT" ]]; then
		echo "TEST FAILED"
		echo $f >> failed.txt
	else
		echo "TEST PASSED"
		((SUCCESS++))
	fi
	echo ""
done
echo "$SUCCESS out of $NUM_OF_TESTS passed."
	
