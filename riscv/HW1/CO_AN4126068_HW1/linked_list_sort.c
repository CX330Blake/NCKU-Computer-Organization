#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
	int data;
	struct Node *next;
} Node;

// Split the linked list into two parts
void splitList(Node *head, Node **firstHalf, Node **secondHalf)
{
	asm volatile(
		// Handle empty or single node list
		"beqz %[head], CaseNullList\n\t"
		"ld t0, 8(%[head])\n\t" // t0 = head->next
		"beqz t0, CaseSingleNode\n\t" // if head->next == NULL

		// Initialize slow = head, fast = head->next
		"mv t1, %[head]\n\t" // t1 = slow
		"mv t2, t0\n\t" // t2 = fast

		"SplitLoop:\n\t"
		"beqz t2, AfterLoop\n\t"
		"ld t0, 8(t2)\n\t" // t0 = fast->next
		"beqz t0, AfterLoop\n\t"
		"ld t1, 8(t1)\n\t" // slow = slow->next
		"ld t2, 8(t2)\n\t" // fast = fast->next
		"ld t2, 8(t2)\n\t" // fast = fast->next->next
		"j SplitLoop\n\t"

		"AfterLoop:\n\t"
		"sd %[head], 0(%[firstHalf])\n\t" // *firstHalf = head
		"ld t0, 8(t1)\n\t" // t0 = slow->next
		"sd t0, 0(%[secondHalf])\n\t" // *secondHalf = slow->next
		"sd zero, 8(t1)\n\t" // slow->next = NULL
		"j SplitDone\n\t"

		"CaseSingleNode:\n\t"
		"sd %[head], 0(%[firstHalf])\n\t"
		"sd zero, 0(%[secondHalf])\n\t"
		"j SplitDone\n\t"

		"CaseNullList:\n\t"
		"sd %[head], 0(%[firstHalf])\n\t"
		"sd zero, 0(%[secondHalf])\n\t"

		"SplitDone:\n\t"
		:
		: [head] "r"(head), [firstHalf] "r"(firstHalf),
		  [secondHalf] "r"(secondHalf)
		: "t0", "t1", "t2", "memory");
}
// void splitList(Node *head, Node **firstHalf, Node **secondHalf)
// {
// 	if (!head || !head->next) {
// 		*firstHalf = head;
// 		*secondHalf = NULL;
// 		return;
// 	}
//
// 	Node *slow = head;
// 	Node *fast = head->next;
//
// 	while (fast && fast->next) {
// 		slow = slow->next;
// 		fast = fast->next->next;
// 	}
//
// 	*firstHalf = head;
// 	*secondHalf = slow->next;
// 	slow->next = NULL;
// }

// Merge two sorted linked lists
Node *mergeSortedLists(Node *a, Node *b)
{
	Node *result = NULL;
	Node *tail = NULL;

	/*
     * Block B (mergeSortedList), which merges two sorted lists into one
     */

	asm volatile(
		"beq %[a], zero, a_null\n\t" // 若 a 為 NULL，跳到 a_null
		"beq %[b], zero, b_null\n\t" // 若 b 為 NULL，跳到 b_null

		// 初始決定第一個節點
		"lw t0, 0(%[a])\n\t" // t0 = a->data
		"lw t1, 0(%[b])\n\t" // t1 = b->data
		"ble t1, t0, set_b_as_head\n\t" // 若 b->data <= a->data，選擇 b 為頭
		// 否則，選擇 a 為頭
		"mv %[result], %[a]\n\t" // result = a
		"mv %[tail], %[a]\n\t" // tail = a
		"lw t4, 4(%[a])\n\t" // t4 = a->next
		"mv %[a], t4\n\t" // a = t4
		"j merge_loop\n\t"

		"set_b_as_head:\n\t"
		"mv %[result], %[b]\n\t" // result = b
		"mv %[tail], %[b]\n\t" // tail = b
		"lw t4, 4(%[b])\n\t" // t4 = b->next
		"mv %[b], t4\n\t" // b = t4
		"j merge_loop\n\t"

		"merge_loop:\n\t"
		"loop_start:\n\t"
		"beq %[a], zero, append_b\n\t" // 若 a 為 NULL，跳到 append_b
		"beq %[b], zero, append_a\n\t" // 若 b 為 NULL，跳到 append_a
		"lw t2, 0(%[a])\n\t" // t2 = a->data
		"lw t3, 0(%[b])\n\t" // t3 = b->data
		"ble t2, t3, a_is_less_or_equal\n\t" // 若 a->data <= b->data，處理 a 分支

		// b 分支：b 較小
		"lw t4, 4(%[b])\n\t" // t4 = b->next
		"sw %[b], 4(%[tail])\n\t" // tail->next = b
		"mv %[tail], %[b]\n\t" // tail = b
		"mv %[b], t4\n\t" // b = t4
		"j loop_start\n\t"

		"a_is_less_or_equal:\n\t"
		"lw t4, 4(%[a])\n\t" // t4 = a->next
		"sw %[a], 4(%[tail])\n\t" // tail->next = a
		"mv %[tail], %[a]\n\t" // tail = a
		"mv %[a], t4\n\t" // a = t4
		"j loop_start\n\t"

		"append_a:\n\t"
		"sw %[a], 4(%[tail])\n\t" // 接上剩餘的 a 鏈表
		"j end_merge\n\t"

		"append_b:\n\t"
		"sw %[b], 4(%[tail])\n\t" // 接上剩餘的 b 鏈表
		"j end_merge\n\t"

		"a_null:\n\t"
		"mv %[result], %[b]\n\t" // 若 a 為 NULL，result = b
		"j end_merge\n\t"

		"b_null:\n\t"
		"mv %[result], %[a]\n\t" // 若 b 為 NULL，result = a

		"end_merge:\n\t"
		: [result] "=r"(result), [tail] "=r"(tail), [a] "+r"(a),
		  [b] "+r"(b)
		:
		: "t0", "t1", "t2", "t3", "t4", "memory");

	return result;
}

// Node *mergeSortedLists(Node *a, Node *b)
// {
// 	if (!a)
// 		return b;
// 	if (!b)
// 		return a;
//
// 	Node *result = NULL;
//
// 	if (a->data <= b->data) {
// 		result = a;
// 		result->next = mergeSortedLists(a->next, b);
// 	} else {
// 		result = b;
// 		result->next = mergeSortedLists(a, b->next);
// 	}
//
// 	return result;
// }

// Merge Sort function for linked list
Node *mergeSort(Node *head)
{
	if (!head || !head->next)
		return head; // Return directly if there is only one node

	Node *firstHalf, *secondHalf;
	splitList(head, &firstHalf,
		  &secondHalf); // Split the list into two sublists

	firstHalf = mergeSort(firstHalf); // Recursively sort the left half
	secondHalf = mergeSort(secondHalf); // Recursively sort the right half

	return mergeSortedLists(firstHalf,
				secondHalf); // Merge the sorted sublists
}

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
	int list_size;
	fscanf(input, "%d", &list_size);
	Node *head = (list_size > 0) ? (Node *)malloc(sizeof(Node)) : NULL;
	Node *cur = head;
	for (int i = 0; i < list_size; i++) {
		fscanf(input, "%d", &(cur->data));
		if (i + 1 < list_size)
			cur->next = (Node *)malloc(sizeof(Node));
		cur = cur->next;
	}
	fclose(input);

	// Linked list sort
	head = mergeSort(head);

	cur = head;
	while (cur) {
		printf("%d ", cur->data);
		/*
     * Block C (Move to the next node), which updates the pointer to
     * traverse the linked list
     */

		// cur = cur->next;

		// asm volatile(
		// 	"ld %[cur], 8(%[cur])\n\t" // Load from cur+8 (cur->next) into cur for 64-bit
		// 	: [cur] "+r"(cur)
		// 	:
		// 	: "memory");

		asm volatile(
			"lw %[cur], 4(%[cur])\n\t" // load cur->next (offset 4) into cur for RV32
			: [cur] "+r"(cur)
			:
			: "memory");
	}
	printf("\n");
	return 0;
}
