#include <stdio.h>

int arraySearch(int *p_a, int arr_size, int target)
{
	int result = -1;

	asm volatile(
		"li t0, 0\n" // i = 0
		"1:\n"
		"bge t0, %[size], 2f\n" // if i >= size, jump to end
		"slli t1, t0, 2\n" // offset = i * 4 (int size)
		"add t2, %[arr], t1\n" // t2 = p_a + offset
		"lw t3, 0(t2)\n" // t3 = *(p_a + i)
		"beq t3, %[target], 3f\n" // if *(p_a + i) == target, jump to found
		"addi t0, t0, 1\n" // i++
		"j 1b\n" // loop

		"2:\n"
		"j 4f\n" // jump to exit

		"3:\n"
		"mv %[res], t0\n" // result = i

		"4:\n"
		: [res] "+r"(result) // output: result is modified
		: [arr] "r"(p_a), [size] "r"(arr_size),
		  [target] "r"(target) // inputs
		: "t0", "t1", "t2", "t3" // clobbered registers
	);
	return result;
}

// int arraySearch(int *p_a, int arr_size, int target)
// {
// 	for (int i = 0; i < arr_size; i++) {
// 		if (*(p_a + i) == target) {
// 			return i;
// 		}
// 	}
// }

// Main function to test the implementation
int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Usage: %s <input_file>\n", argv[0]);
		return 1;
	}

	FILE *input = fopen(argv[1], "r");
	if (!input) {
		fprintf(stderr, "Error opening file: %s\n", argv[1]);
		return 1;
	}
	int arr_size;
	fscanf(input, "%d", &arr_size);
	int arr[arr_size];

	// Read integers from input file into the array
	for (int i = 0; i < arr_size; i++) {
		int data;
		fscanf(input, "%d", &data);
		arr[i] = data;
	}
	int target;
	fscanf(input, "%d", &target);
	fclose(input);

	int *p_a = &arr[0];

	int index = arraySearch(p_a, arr_size, target);

	// Print the result
	printf("%d ", index);
	printf("\n");

	return 0;
}
