/*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define MAX_NUM_OF_COORDINATES_PER_LINE 5
#define NUMS_IN_XY 2
#define NUMERICAL_BASE 10
#define NUM_POSSIBLE_ADJ_CELLS 4

//New types and structure Prototypes
typedef struct {
	int x;
	int y;
} xy_t;

/*----------------------------------------------------------------------------*/

//The two structures below were obtained from
//Author: 
/*
A
l
i
s
t
a
i
r
 
 M
 o
 f
 f
 a
 t
*/
typedef struct node node_t;

struct node {
	xy_t xy;
	int counter;
	node_t *next;
};

typedef struct {
	node_t *head;
	node_t *foot;
	int counter;
} list_t;

/*----------------------------------------------------------------------------*/

//Function Prototypes
int in_grid(xy_t xy, xy_t grid_size);
list_t *get_blocks(xy_t grid_size);
void print_basic_information(xy_t grid_size, xy_t init_cell, xy_t goal_cell,
	int block_counter);
void print_route(list_t *path, int path_counter);
int check_status(list_t *path, list_t *block, xy_t grid_size, xy_t init_cell,
	xy_t goal_cell, node_t **last_legal);
int compare_xy(xy_t xy1, xy_t xy2);
int check_for_illegal_moves(list_t *path, xy_t grid_size);
int check_for_block(list_t *path, list_t *block, node_t **last_legal);
void print_status(int status);
void print_grid(list_t *path, list_t *block, xy_t grid_size, xy_t init_cell,
	xy_t goal_cell);
int fix_path(node_t *last_legal, list_t *path, list_t *block, xy_t grid_size);
int choose_adjacent_cell(int *adjacent_cells, int adjacent_cells_counter,
	node_t *BFS_array, node_t prev);
node_t *xy_in_list(xy_t xy, list_t *list);
int check_and_insert(xy_t curr_xy, list_t *after_illegal_cell,
	list_t *BFS_queue, list_t *block, xy_t grid_size, int counter);
int find_list_length(list_t *list);

/*----------------------------------------------------------------------------*/

//The three functions below were obtained from
//Author: 
/*
A
l
i
s
t
a
i
r
 
 M
 o
 f
 f
 a
 t
*/
list_t *make_empty_list(void) {
	list_t *list;
	list = (list_t*)malloc(sizeof(*list));
	assert(list!=NULL);
	list->head = list->foot = NULL;
	return list;
}

void free_list(list_t *list) {
	node_t *curr, *prev;
	assert(list!=NULL);
	curr = list->head;
	while (curr) {
		prev = curr;
		curr = curr->next;
		free(prev);
	}
	free(list);
}

list_t *insert_at_foot(list_t *list, xy_t value, int counter) {
	node_t *new;
	new = (node_t*)malloc(sizeof(*new));
	assert(list!=NULL && new!=NULL);
	new->xy = value;
	new->counter = counter;
	new->next = NULL;
	if (list->foot==NULL) {
		/* this is the first insertion into the list */
		list->head = list->foot = new;
	} else {
		list->foot->next = new;
		list->foot = new;
	}
	return list;
}

/*----------------------------------------------------------------------------*/

//Defining functions
//Check if xy is in grid
int in_grid(xy_t xy, xy_t grid_size) {
	return !((xy.x>=grid_size.x) || (xy.y>=grid_size.y) || (xy.x<0) || (xy.y<0));
}

//Getting blocks
list_t *get_blocks(xy_t grid_size) {
	//Declaring variables
	list_t *block;
	block = make_empty_list();
	xy_t cell;

	//Getting blocks and inserting them at the foot of the block list
	while (scanf("[%d,%d]\n", &cell.y, &cell.x) == NUMS_IN_XY) {
		if (in_grid(cell, grid_size) == 1) {
			block = insert_at_foot(block, cell, 0);
			(block->counter)++;
		}
	}
	return block;
}

//Printing basic information
void print_basic_information(xy_t grid_size, xy_t init_cell, xy_t goal_cell,
	int block_counter) {
	printf("The grid has %d rows and %d columns.\n",
		grid_size.y, grid_size.x);
	printf("The grid has %d block(s).\n", block_counter);
	printf("The initial cell in the grid is [%d,%d].\n",
		init_cell.y, init_cell.x);
	printf("The goal cell in the grid is [%d,%d].\n",
		goal_cell.y, goal_cell.x);
}

//Printing the proposed route
void print_route(list_t *path, int path_counter) {
	//Declaring variables
	int printed_this_line_counter = 0, printed_counter = 0;
	node_t *curr_path_cell = path->head;

	while (curr_path_cell) {
		//Printing a newline and resetting the counter
		if (printed_this_line_counter == MAX_NUM_OF_COORDINATES_PER_LINE) {
			printf("\n");
			printed_this_line_counter = 0;
		}

		//For the normal coordinates
		if (printed_counter < (path_counter - 1)) {
			printf("[%d,%d]->", curr_path_cell->xy.y, curr_path_cell->xy.x);
			printed_this_line_counter++;
		}

		//For the last coordinates
		else {
			printf("[%d,%d].\n", curr_path_cell->xy.y, curr_path_cell->xy.x);
			printed_this_line_counter++;
		}

		printed_counter++;
		curr_path_cell = curr_path_cell->next;
	}
}

//Checking the status of the route
int check_status(list_t *path, list_t *block, xy_t grid_size, xy_t init_cell,
	xy_t goal_cell, node_t **last_legal) {
	if (compare_xy(init_cell, (path->head)->xy) == 0) {
		return 1;
	}
	else if (compare_xy(goal_cell, (path->foot)->xy) == 0) {
		return 2;
	}
	else if (check_for_illegal_moves(path, grid_size) == 1) {
		return 3;
	}
	else if (check_for_block(path, block, last_legal) == 1) {
		return 4;
	}
	return 5;
}

//Comparing two coordinates
int compare_xy(xy_t xy1, xy_t xy2) {
	if ((xy1.x == xy2.x) && ((xy1.y == xy2.y))) {
		return 1;
	}
	return 0;
}

//Check for illegal moves
int check_for_illegal_moves(list_t *path, xy_t grid_size) {
	//Declaring Variables
	int blocks_moved;
	node_t *curr_path_cell = path->head;
	xy_t curr, prev;

	//Setting prev to curr for first iteration
	prev.x = curr_path_cell->xy.x; prev.y = (curr_path_cell->xy).y;

	//Iterates through the path list checking if its in the grid and if its
	//moving a legal distance.
	while (curr_path_cell) {
		curr.x = curr_path_cell->xy.x; curr.y = (curr_path_cell->xy).y;

		if (in_grid(curr, grid_size) == 0) {
			return 1;
		}

		blocks_moved = abs(curr.x - prev.x) + abs(curr.y - prev.y);
		if (blocks_moved > 1) {
			return 1;
		}

		curr_path_cell = curr_path_cell->next;
		prev.x = curr.x; prev.y = curr.y;
	}
	return 0;
}

//Check for block in path
int check_for_block(list_t *path, list_t *block, node_t **last_legal) {
	node_t *curr_path_cell = path->head, *curr_block_cell = block->head;
	node_t *prev_path_cell = NULL;
	//Iteratres through path list first and then block list checking if there
	//is ever an intersection and provides the index of the last legal move
	while (curr_path_cell) {
		while (curr_block_cell) {
			if (compare_xy(curr_path_cell->xy, curr_block_cell->xy) == 1) {
				*last_legal = prev_path_cell;
				return 1;
			}
			curr_block_cell = curr_block_cell->next;
		}
		prev_path_cell = curr_path_cell;
		curr_path_cell = curr_path_cell->next;
		curr_block_cell = block->head;
	}
	return 0;
}

//Printing status
void print_status(int status) {
	if (status == 1) {
		printf("Initial cell in the route is wrong!\n");
	}
	else if (status == 2) {
		printf("Goal cell in the route is wrong!\n");
	}
	else if (status == 3) {
		printf("There is an illegal move in this route!\n");
	}
	else if (status == 4) {
		printf("There is a block on this route!\n");
	}
	else if (status == 5) {
		printf("The route is valid!\n");
	}
	else if (status == 6) {
		printf("The route cannot be repaired!\n");
	}
}

//Printing Grid
void print_grid(list_t *path, list_t *block, xy_t grid_size, xy_t init_cell,
	xy_t goal_cell) {
	//Creating 2d character array that contains string
	int num_rows = grid_size.y + 1;

	//One extra column for the ruler, another extra column for nullbytes
	int num_cols = grid_size.x + 1 + 1;
	int x, y;
	char grid[num_rows][num_cols];

	//Set every character to a space
	for (y = 0; y < num_rows; y++) {
		for (x = 0; x < num_cols; x++) {
			grid[y][x] = ' ';
		}
	}

	//Set every last character of each row to \0
	for (y = 0; y < num_rows; y++) {
		grid[y][num_cols - 1] = '\0';
	}

	//Set the first row as a ruler
	for (x = 0; x < (num_cols - 1 - 1); x++) {
		grid[0][x + 1] = (x % NUMERICAL_BASE) + '0';
	}

	//Set the first col as a ruler
	for (y = 0; y < (num_rows - 1); y++) {
		grid[y + 1][0] = (y % NUMERICAL_BASE) + '0';
	}

	//Show characters that are part of the path
	node_t *curr_path_cell = path->head;
	while (curr_path_cell) {
		grid[curr_path_cell->xy.y + 1][curr_path_cell->xy.x + 1] = '*';
		curr_path_cell = curr_path_cell->next;
	}

	//Show characters that are block
	node_t *curr_block_cell = block->head;
	while (curr_block_cell) {
		grid[curr_block_cell->xy.y + 1][curr_block_cell->xy.x + 1] = '#';
		curr_block_cell = curr_block_cell->next;
	}

	//Show initial and goal cells
	grid[init_cell.y + 1][init_cell.x + 1] = 'I';
	grid[goal_cell.y + 1][goal_cell.x + 1] = 'G';

	//Printing the grid
	for (y = 0; y < num_rows; y++) {
		printf("%s\n", grid[y]);
	}
}

//Repairing Broken Route
int fix_path(node_t *last_legal, list_t *path, list_t *block,
	xy_t grid_size) {
	//Defining variables to fix route
	//The first illegal cell
	node_t *illegal_cell;
	illegal_cell = last_legal->next;

	//Unused path cells
	list_t *unused_path = make_empty_list();
	unused_path->head = illegal_cell;

	//List of path cells after the illegal cell
	list_t *after_illegal_cell = make_empty_list();
	after_illegal_cell->head = illegal_cell->next;
	after_illegal_cell->foot = path->foot;

	//Breadth First Seach queue
	list_t *BFS_queue;
	BFS_queue = make_empty_list();
	BFS_queue->head = last_legal;
	BFS_queue->foot = last_legal;
	BFS_queue->head->next = NULL;
	BFS_queue->head->counter = 0;

	node_t *curr_BFS_cell;
	curr_BFS_cell = BFS_queue->head;
	xy_t curr_xy;
	int path_found = 0;

	//Branching out and creating BFS queue
	//Break if it's part of after_illegal_cell which means path has been fixed
	while (path_found != 1) {
		//Checking above
		curr_xy.x = (curr_BFS_cell->xy.x);
		curr_xy.y = (curr_BFS_cell->xy.y) - 1;
		if (check_and_insert(curr_xy, after_illegal_cell, BFS_queue,	block,
			grid_size, curr_BFS_cell->counter + 1) == 1) {
			break;
		}

		//Checking below
		curr_xy.x = (curr_BFS_cell->xy.x);
		curr_xy.y = (curr_BFS_cell->xy.y) + 1;
		if (check_and_insert(curr_xy, after_illegal_cell, BFS_queue,	block,
			grid_size, curr_BFS_cell->counter + 1) == 1) {
			break;
		}

		//Checking to the left
		curr_xy.x = (curr_BFS_cell->xy.x) - 1;
		curr_xy.y = (curr_BFS_cell->xy.y);
		if (check_and_insert(curr_xy, after_illegal_cell, BFS_queue,	block,
			grid_size, curr_BFS_cell->counter + 1) == 1) {
			break;
		}

		//Checking to the right
		curr_xy.x = (curr_BFS_cell->xy.x) + 1;
		curr_xy.y = (curr_BFS_cell->xy.y);
		if (check_and_insert(curr_xy, after_illegal_cell, BFS_queue,	block,
			grid_size, curr_BFS_cell->counter + 1) == 1) {
			break;
		}

		curr_BFS_cell = curr_BFS_cell->next;

		//Stopping BFS if the end of the queue has been reached and
		//resetting BFS_queue->head->next to its previous value
		if (curr_BFS_cell == NULL) {
			BFS_queue->head->next = illegal_cell;
			return 0;
		}
	}

	//Converting BFS_queue to an array
	int BFS_counter = find_list_length(BFS_queue);
	curr_BFS_cell = BFS_queue->head;
	node_t BFS_array[BFS_counter];
	int i;
	for (i = 0; i < BFS_counter; i++) {
		BFS_array[i].xy.x = curr_BFS_cell->xy.x;
		BFS_array[i].xy.y = curr_BFS_cell->xy.y;
		BFS_array[i].counter = curr_BFS_cell->counter;
		curr_BFS_cell = curr_BFS_cell->next;
	}

	//Finding the new path
	//This path array contains indexes of the correct path cells from BFS_array
	int BFS_path_length = BFS_queue->foot->counter + 1;
	int BFS_path[BFS_path_length];
	int adjacent_cells[NUM_POSSIBLE_ADJ_CELLS] = {0};
	int adjacent_cells_counter = 0, chosen_cell = 0;
	int local_BFS_path_counter = BFS_path_length - 1;
	BFS_path[local_BFS_path_counter] = BFS_counter - 1;
	int local_BFS_counter = BFS_counter - 1, blocks_moved;
	node_t prev, curr;

	while ((local_BFS_path_counter != -1) && ((local_BFS_counter != -1))) {
		//Prev node is a node on the path that we compare against to
		//find out if the curr node is adjacent
		prev.xy.x = BFS_array[BFS_path[local_BFS_path_counter]].xy.x;
		prev.xy.y = BFS_array[BFS_path[local_BFS_path_counter]].xy.y;
		prev.counter = BFS_array[BFS_path[local_BFS_path_counter]].counter;
		curr.xy.x = BFS_array[local_BFS_counter].xy.x;
		curr.xy.y = BFS_array[local_BFS_counter].xy.y;;
		curr.counter = BFS_array[local_BFS_counter].counter;

		//If the BFS counter value is 2 lower than the current one
		//Add the adjacent_cell with the most priority to BFS_path and continue
		//Because at that point it can't be adjacent
		if (curr.counter == (prev.counter - 2)) {
			chosen_cell = choose_adjacent_cell(adjacent_cells,
				adjacent_cells_counter,	BFS_array, prev);
			local_BFS_path_counter--;
			BFS_path[local_BFS_path_counter] = chosen_cell;
			adjacent_cells_counter = 0;
			for (i = 0; i < NUM_POSSIBLE_ADJ_CELLS; i++){
				adjacent_cells[i] = 0;
			}
			continue;
		}

		//Checking if curr node is adjacent
		blocks_moved = abs(curr.xy.x - prev.xy.x) + abs(curr.xy.y - prev.xy.y);
		if (blocks_moved == 1) {
			adjacent_cells[adjacent_cells_counter] = local_BFS_counter;
			adjacent_cells_counter++;
		}
		local_BFS_counter--;
	}
	BFS_path[0] = 0;

	//Freeing BFS_queue
	BFS_queue->head = BFS_queue->head->next;
	free_list(BFS_queue);

	//Inserting fixed route into path
	path->foot = last_legal;
	for (i = 1; i < BFS_path_length; i++) {
		insert_at_foot(path, BFS_array[BFS_path[i]].xy, 0);
	}

	//This is used later to determine the path nodes are abandoned
	node_t *connection_node = path->foot;

	//Connecting it to the after_illegal_cell path
	path->foot->next = xy_in_list(path->foot->xy, after_illegal_cell)->next;
	path->foot = after_illegal_cell->foot;
	free(after_illegal_cell);

	//Finding and freeing unused path cells
	node_t *curr_unused_path = unused_path->head;
	while(compare_xy(curr_unused_path->next->xy, connection_node->xy) == 0) {
		curr_unused_path = curr_unused_path->next;
	}
	unused_path->foot = curr_unused_path;
	unused_path->foot->next = NULL;
	free_list(unused_path);

	return 1;
}

//Choosing adjacent_cell with preference being up down left right
int choose_adjacent_cell(int *adjacent_cells, int adjacent_cells_counter,
	node_t *BFS_array, node_t prev) {
	int i;

	//Check if its above
	for (i = 0; i < adjacent_cells_counter; i++) {
		if (BFS_array[adjacent_cells[i]].xy.y == (prev.xy.y-1)) {
			return adjacent_cells[i];
		}
	}

	//Check if its below
	for (i = 0; i < adjacent_cells_counter; i++) {
		if (BFS_array[adjacent_cells[i]].xy.y == (prev.xy.y+1)) {
			return adjacent_cells[i];
		}
	}

	//Check if its to the left
	for (i = 0; i < adjacent_cells_counter; i++) {
		if (BFS_array[adjacent_cells[i]].xy.x == (prev.xy.x-1)) {
			return adjacent_cells[i];
		}
	}

	//Check if its to the right
	for (i = 0; i < adjacent_cells_counter; i++) {
		if (BFS_array[adjacent_cells[i]].xy.x == (prev.xy.x+1)) {
			return adjacent_cells[i];
		}
	}

	return 0;
}

//Check if xy is in a list
node_t *xy_in_list(xy_t xy, list_t *list) {
	node_t *list_cell = list->head;
	while (list_cell) {
		if (compare_xy(xy, list_cell->xy) == 1) {
			return list_cell;
		}
		list_cell = list_cell->next;
	}
	return NULL;
}

//Check if branch is valid and insert it into BFS_queue
//Return 1 if it reconnects to the path
int check_and_insert(xy_t curr_xy, list_t *after_illegal_cell,
	list_t *BFS_queue, list_t *block, xy_t grid_size, int counter) {
	if (xy_in_list(curr_xy, BFS_queue)) {
		return 0;
	}
	else if (!in_grid(curr_xy, grid_size)) {
		return 0;
	}
	else if (xy_in_list(curr_xy, block)) {
		return 0;
	}
	else if (xy_in_list(curr_xy, after_illegal_cell)) {
		insert_at_foot(BFS_queue, curr_xy, counter);
		return 1;
	}
	else {
		insert_at_foot(BFS_queue, curr_xy, counter);
		return 0;
	}
}

//Find list length
int find_list_length(list_t *list) {
	int list_counter = 0;
	node_t *list_cell;
	list_cell = list->head;
	while (list_cell) {
		list_counter++;
		list_cell = list_cell->next;
	}
	return list_counter;
}

/*----------------------------------------------------------------------------*/

int main(int argc, char *argv[]) {
	//Declaration of Variables
	int status;
	xy_t grid_size, init_cell, goal_cell;

	//Getting grid_size, init_cell and goal_cell
	scanf("%dx%d\n", &grid_size.y, &grid_size.x);
	scanf("[%d,%d]\n", &init_cell.y, &init_cell.x);
	scanf("[%d,%d]\n", &goal_cell.y, &goal_cell.x);

	//Checking if init_cell and goal_cell is inside the grid
	if ((in_grid(init_cell, grid_size) == 0)) {
		return EXIT_FAILURE;
	}
	if ((in_grid(goal_cell, grid_size) == 0)) {
		return EXIT_FAILURE;
	}

	//List for block and path
	list_t *path, *block;
	int path_counter = 0, block_counter = 0;
	xy_t cell;
	path = make_empty_list();
	node_t *last_legal = NULL;

	//Getting blocks
	block = get_blocks(grid_size);

	//Getting path as a list
	char ch = getchar();
	ch = ' ';
	while ((ch != EOF) && ((ch != '$'))) {
		if (scanf("[%d,%d]", &cell.y, &cell.x) == 2) {
			path = insert_at_foot(path, cell, 0);
		}
		ch = getchar();
	}

	//Print Basic Information
	printf("==STAGE 0=======================================\n");
	block_counter = find_list_length(block);
	print_basic_information(grid_size, init_cell, goal_cell, block_counter);
	printf("The proposed route in the grid is:\n");
	path_counter = find_list_length(path);
	print_route(path, path_counter);
	status = check_status(path, block, grid_size, init_cell, goal_cell,
		&last_legal);
	print_status(status);
	printf("==STAGE 1=======================================\n");
	print_grid(path, block, grid_size, init_cell, goal_cell);

	int stage_2_equals_has_been_printed = 0;
	int line_of_equals_has_been_printed = 0;

	//Routes that can't be fixed because of bad goal or init poistions
	//or routes that go outside the grid
	if ((status == 1) || (status == 2) || (status == 3)) {
		printf("================================================\n");
		free_list(path);
		free_list(block);
		return 0;
	}

	//Fixing the route if there was a block on path
	if (status == 4) {
		if (fix_path(last_legal, path, block, grid_size) == 1) {
			path_counter = find_list_length(path);
			status = check_status(path, block, grid_size, init_cell,
				goal_cell, &last_legal);
		}
		else {
			status = 6;
		}
		printf("------------------------------------------------\n");
		print_grid(path, block, grid_size, init_cell, goal_cell);
		printf("------------------------------------------------\n");
		if (status !=6) {
			print_route(path, path_counter);
		}
		print_status(status);
	}

	//Getting new blocks later in the file
	while (ch != EOF) {
		if (ch == '$') {
			if (stage_2_equals_has_been_printed == 0) {
				printf("==STAGE 2=======================================\n");
				stage_2_equals_has_been_printed = 1;
			}

			//Getting blocks and freeing previous blocks
			ch = getchar();
			ch = getchar();
			free_list(block);
			block = get_blocks(grid_size);

			print_grid(path, block, grid_size, init_cell, goal_cell);
			int status = check_status(path, block, grid_size, init_cell,
				goal_cell, &last_legal);
			if (status == 4) {
				printf("------------------------------------------------\n");
				while (status == 4) {
					if (fix_path(last_legal, path, block, grid_size) == 1) {
						path_counter = find_list_length(path);
						status = check_status(path, block, grid_size, init_cell,
							goal_cell, &last_legal);
					}
					else {
						status = 6;
					}
				}
				print_grid(path, block, grid_size, init_cell, goal_cell);
				printf("------------------------------------------------\n");
				if (status !=6 ) {
					print_route(path, path_counter);
				}
				print_status(status);
			}
			printf("================================================\n");
			line_of_equals_has_been_printed = 1;
		}
		ch = getchar();
	}

	if (line_of_equals_has_been_printed == 0) {
		printf("================================================\n");
	}

	free_list(block);
	free_list(path);
	return 0;
}

// algorithms are fun
