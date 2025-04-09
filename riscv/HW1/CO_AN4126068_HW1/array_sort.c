#include <stdio.h>

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
	fclose(input);

	int *p_a = &arr[0];

	// array a bubble sort
	/* Original C code segment
  for (int i = 0; i < arr_size - 1; i++) {
      for (int j = 0; j < arr_size - i -1; j++) {
          if (*(p_a + j) > *(p_a + j + 1)) {
              int tmp = *(p_a + j);
              *(p_a + j) = *(p_a + j + 1);
              *(p_a + j + 1) = tmp;
          }
      }
  }
  */

	// clang-format off
    for (int i = 0; i < arr_size - 1; i++) {
        for (int j = 0; j < arr_size - i - 1; j++) {
            asm volatile(
                "slli t3, %1, 2       \n\t" // Compute offset: t3 = j * 4, slli = shift left logical immediate (%1 = j)
                "add  t2, %0, t3      \n\t" // Compute address: t2 = p_a + (j * 4), t2 = base pointer + offset (%0 = base pointer)
                "lw   t4, 0(t2)       \n\t" // Load arr[j] into t4, lw = load word (4 bytes)
                "lw   t5, 4(t2)       \n\t" // Load arr[j+1] into t5, 4(t2) = t2 + 4 = arr[j+1]
                "blt  t5, t4, 1f      \n\t" // If t5 (arr[j+1]) < t4 (arr[j]), jump to swap (label 1), blt = branch if less than
                "j    2f              \n\t" // Otherwise, skip swap (jump to label 2)
                "1:                   \n\t" // Label 1: swap
                "sw   t5, 0(t2)       \n\t" // Store t5 into t2 (arr[j])
                "sw   t4, 4(t2)       \n\t" // Store t4 into t2 + 4 (arr[j+1])
                "2:                   \n\t" // End
                : // No outputs
                : "r"(p_a), "r"(j) // %0 = p_a, %1 = j
                : "t2", "t3", "t4", "t5", "memory" // Tell the compiler that t2, t3, t4, t5, and memory are modified
            );
        }
    }
	// clang-format on
	p_a = &arr[0];
	for (int i = 0; i < arr_size; i++)
		printf("%d ", *p_a++);
	printf("\n");
	return 0;
}
