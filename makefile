all: main runner

main: granttoe_hw2.c
	gcc granttoe_hw2.c -o granttoe_hw2

runner: granttoe_hw2_runner.c
	gcc granttoe_hw2_runner.c -o granttoe_hw2_runner
