TESTS=0
TESTS=(test-cases-codegen/*)
NUM_OF_TESTS=0
NUM_OF_TESTS=${#TESTS[@]}

SUCCESS=0

echo "Building Executable..."
./compile.sh
echo "Compiling tests..."
echo -n "" > failed.txt
mkdir cg_outputs
for f in "${TESTS[@]}"; do
	echo "****** Compiling test $f ******"
	./run.sh $f cg_outputs/${f##*/}.s
	./run_sol.sh $f cg_outputs/${f##*/}.sico
	if [[ -n "$RESULT" ]]; then
		echo "TEST FAILED"
	else
		echo "TEST PASSED"
		((SUCCESS++))
	fi
	echo ""
done
echo "Finished."
