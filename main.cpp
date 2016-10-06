// SARDORJON NAZIROV
// CISC 3410
// Assignment1
/*
   ____ ___ ____   ____   _____ _  _   _  ___
  / ___|_ _/ ___| / ___| |___ /| || | / |/ _ \
 | |    | |\___ \| |       |_ \| || |_| | | | |
 | |___ | | ___) | |___   ___) |__   _| | |_| |
  \____|___|____/ \____| |____/   |_| |_|\___/
 
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>
#include <time.h>
#include <vector>
#include <stack>
#include <queue>

using namespace std;


class TREE {
private:
	/*
	 A node can have up to 4 children.
	 Also a parent node is necessary to provide access
	 to the parent of a particular node on the tree
	 */
	struct Node{
		Node* child1;
		Node* child2;
		Node* child3;
		Node* child4;
		Node* parent;
		
		/*
		 The following vector will contain the set of numbers that
		 represent the state
		 */
		vector<int> state;
		
		/*  Bookkeeping variables
		 Cost - holds the the total cost of traveling to that node from the root
		 Index - keeps the location of the 0 in the vector 'state'
		 */
		int cost;
		int index;
		int distance;
		
		// String to store the direction
		string direction;
		
	};// END of struct
	
	// initial and goal states are stored below in vectors
	vector<int> initial, goal_state;
	
	// Vector stack to keep track of the nodes
	vector<Node*> stack;
	
	
	Node* root;
	
	int heuristic;
	bool success;

	
public:
	
	int stackDepth = 0;
	int nodesExpanded = 0;
	::stack<string> pathToGoal;
	int depthLimit = 50;
	int depth = 0;
	string searchType ;
	
	TREE() {
		root = NULL;
	}
	
	// FUNCTIONS
	void readFromFile();
	void construct(string);
	void insertNode(Node*, string);
	void createRoot(vector<int>);
	void move(Node*, Node*, char);
	void print(Node*);
	void didItSucceed(Node*);
	int h_value(Node*);
	void organize();
	bool isEmpty(){ return root==NULL; }
};


// ################# CONSTRUCT #######################################
void TREE::construct(string searchType) {

	if(searchType == "BFS"){
		cout << "\n######################| BFS |#########################\n";
	} else if(searchType == "A*"){
		cout << "\n######################| A* |#########################\n";
	} else {
		cout << "\n######################| DFS |#########################\n";
	}
	
	readFromFile();
	createRoot(initial);
	success = false;
	Node* current;
	heuristic = 1;

	/*
	 Stack size is larger than 0 at the beginnig
	 Because when the root node is created, it is also
	 pushed to the back of the stack
	 */
	int count = 0;
	while (stack.size() > 0) {
		if(searchType == "BFS"){
			current = stack.front();
			stack.erase(stack.begin());
			insertNode(current, searchType);
		} else if(searchType == "A*") {
			current = stack.front();
			stack.erase(stack.begin());
			insertNode(current, searchType);
		} else {
			current = stack.back();
			stack.pop_back();
			insertNode(current, searchType);
		}
		
		if (current->distance > count) {
			count = current->distance;
			cout << "n = " << count << ", ";
		}
	}
	cout << endl;
	stack.clear();
} // ################## END OF CONSTRUCT ##########################

void TREE::readFromFile(){
	ifstream infilePuzzle("./puzzle.txt");
	ifstream infileGoal("./goal.txt");
	string each_line, each_number;
	
	// We need to convert the strings into integers
	// and store them in the vectors
	
	// Get the info from the puzzle file
	// and push them into the 'initial' vector
	cout << "Puzzle " << endl;
	while(getline(infilePuzzle, each_line)){
		stringstream parse(each_line);
		while(getline(parse, each_number, ' ')){ // Delimiter is ' '
			initial.push_back(atoi(each_number.c_str()));
		}
		cout <<each_line << endl;
	}
	
	cout << endl;
	
	// Get the info from the goal file
	// and push them into the 'goal_state' vector
	cout << "Goal" << endl;
	while(getline(infileGoal, each_line)){
		stringstream parse(each_line);
		while(getline(parse, each_number, ' ')){ // Delimiter is ' '
			goal_state.push_back(atoi(each_number.c_str()));
		}
		cout << each_line << endl;
	}
	cout << "----------------------\n\n";
	
}
void TREE::createRoot(vector<int> initial) {
	
	Node* vertex = new Node;
	for (int i = 0; i < (signed)initial.size(); i++) {
		vertex->state.push_back(initial[i]);
		//cout << initial[i] << " ";
	}
	
	vertex->cost = h_value(vertex);
	//cout << endl << "**" << vertex->cost << endl;
	vertex->distance = 1;
	vertex->direction = "MOVE=>";
	vertex->child1 = NULL; vertex->child2 = NULL; vertex->child3 = NULL; vertex->child4 = NULL;
	vertex->parent = NULL;
	
	
	/*
	 The following for loop goes through the initial state's vector elements
	 and tries to find the location of 0, which is the blank tile.
	 When the 0 is found, it's location is set to the newly created root node's index
	 */
	for (int i = 0; i < (signed)initial.size(); i++) {
		if (initial[i] == 0) {
			vertex->index = i;
		}
	}
	
	/*
	 The following if statement checks if the root node is empty.
	 If it is, the root node is set equal to the newly created node.
	 Also, the root node is pushed into the stack
	 */
	if(isEmpty()) {
		root = vertex;
		stack.push_back(root);
		stackDepth++;
	}
}
/*
	HEURISTIC - h1
	Determine the number of misplaced tiles
 */
int TREE::h_value(Node* t) {
	int cost = 0;
	
	/*
	 The following for loop compares the received states elements to the goal_state's
	 elements. Whenever they don't match, the cost integer is incremented by 1
	 */
	for (int i = 0; i < (signed)t->state.size(); i++) {
		if (t->state[i] != goal_state[i]) {
			cost++;
		}
	}
	
	return cost;
}


/*
 - insertNode accepts Node* pointer to a parent node
 - It creates 4 different other nodes and makes the children of the parent node
 -
 */
void TREE::insertNode(Node* current, string searchType) {
	
	
	/***********************************************
	 Each block of code below does the following
	 1. Create a new node
	 2. Send the current node and new node to move() function, along with the direction
	 3. When move() function completes creating a new node, the new node's distance value
	 is set equal to (parent node -> distance) + 1
	 4. f(n) = g(n) + h(n)
	 f(n) => Estimated cost of the cheapest solution
	 g(n) => The cost to reach the node
	 h(n) => The cost to get from the node to the goal
	 5. Direction of the 0 (blank tile) where it will be moved in the new state
		6. All children of the new node point to NULL
	 7. Parent node of the new node points to the current(parent) node
	 *************************************************/
	// Block 1
	Node* vertex_up = new Node; //1
	move(current, vertex_up, 'u'); //2
	vertex_up->distance = current->distance + 1; //3
	vertex_up->cost = h_value(vertex_up) + vertex_up->distance; //4
	vertex_up->direction = "UP"; //5
	vertex_up->child1 = NULL; vertex_up->child2 = NULL; vertex_up->child3 = NULL; vertex_up->child4 = NULL; //6
	vertex_up->parent = current; //7
	
	// Block 2
	Node* vertex_right = new Node;
	move(current, vertex_right, 'r');
	vertex_right->distance = current->distance + 1;
	vertex_right->cost = h_value(vertex_right) + vertex_right->distance;
	vertex_right->direction = "RIGHT";
	vertex_right->child1 = NULL; vertex_right->child2 = NULL; vertex_right->child3 = NULL; vertex_right->child4 = NULL;
	vertex_right->parent = current;
	
	// Block 3
	Node* vertex_down = new Node;
	move(current, vertex_down, 'd');
	vertex_down->distance = current->distance + 1;
	vertex_down->cost = h_value(vertex_down) + vertex_down->distance;
	vertex_down->direction = "DOWN";
	vertex_down->child1 = NULL; vertex_down->child2 = NULL; vertex_down->child3 = NULL; vertex_down->child4 = NULL;
	vertex_down->parent = current;
	
	// Block 4
	Node* vertex_left = new Node;
	move(current, vertex_left, 'l');
	vertex_left->distance = current->distance + 1;
	vertex_left->cost = h_value(vertex_left) + vertex_left->distance;
	vertex_left->direction = "LEFT";
	vertex_left->child1 = NULL; vertex_left->child2 = NULL; vertex_left->child3 = NULL; vertex_left->child4 = NULL;
	vertex_left->parent = current;
	
	/**********************************************/
	
	/*
	 If current node does not have children and new node's state vector is not empty,
	 assign the nodes created above as the children of parent node (current).
	 Otherwise, delete the new node
	 If the goal state is not found yet, push the node to the back of the stack
	 */
	if ((current->child1 == NULL) && (vertex_up->state.size() > 0)) {
		current->child1 = vertex_up;
		if (!success){
			stack.push_back(current->child1);
			stackDepth++;
		}
	}
	else
		delete vertex_up;
	
	if ((current->child2 == NULL) && (vertex_right->state.size() > 0)) {
		current->child2 = vertex_right;
		if (!success){
			stack.push_back(current->child2);
			stackDepth++;
		}
	}
	else
		delete vertex_right;
	
	if ((current->child3 == NULL) && (vertex_down->state.size() > 0)) {
		current->child3 = vertex_down;
		if (!success){
			stack.push_back(current->child3);
			stackDepth++;
		}
	}
	else
		delete vertex_down;
	
	if ((current->child4 == NULL) && (vertex_left->state.size() > 0)) {
		current->child4 = vertex_left;
		if (!success){
			stack.push_back(current->child4);
			stackDepth++;
		}
	}
	else
		delete vertex_left;
	

	if (searchType == "BFS"){
		// Nothing
		didItSucceed(current);
	} else if (searchType == "A*"){
		organize();
		didItSucceed(current);
	} else if(searchType == "DFS"){
		didItSucceed(current);
		depth++;
		if(depth > depthLimit){
			stack.clear();
			cout << "GOAL STATE IS NOT FOUND!" << endl;
		}
	}

}


/*
 
 */
void TREE::move(Node* current, Node* t, char direction) {
	int temporary, index = 0;
	bool wrongMove = false;
	
	/*
	 The following for loop goes through the current state's vector elements
	 and pushes them into the new vector stack t, which is a new node on the tree.
	 Along the way, it searches for the location of 0 in the current vector and
	 when it finds it, sets the local integer index's value to the location index
	 of 0 in the vector
	 */
	for (int i = 0; i < (signed)current->state.size(); i++) {
		if (current->state[i] == 0) {
			index = i;
		}
		t->state.push_back(current->state[i]);
	}
	
	/* New node's index value is set equal to the index of 0 in
	 current state vector
	 */
	t->index = index;
	
	
	/*
	 The following if statement works as an identifier to
	 see if there is a backtracking happening. When true
	 wringMove is set to 'true'. It prevents from creating
		redundant nodes
	 */
	if (current->distance > 2) {
		//while(current->parent != NULL){
			if (index == current->parent->index) {
				wrongMove = true;
			}
			//current = current->parent;
		//}
		
	}
	
	/*
	 If statements that move the blank tile around depending
	 on the direction. If a request to move the blank tile to up
	 is received, the appropriate if statement will swap their places.
	 In other words, let's say if the 0 is in the center and the direction
	 is 'u' or 'up', the first if statement will swap the location of 0 with
	 the number above it
	 */
	if(direction == 'u'){
		if ((index > 2) && (!wrongMove)) {
			temporary = t->state[index];
			t->state[index] = t->state[index-3];
			t->state[index-3] = temporary;
			nodesExpanded++;
		}
		else
			t->state.erase(t->state.begin(), t->state.end());
	} else if (direction == 'r'){
		if ((index%3 != 2) && (!wrongMove)) {
			temporary = t->state[index];
			t->state[index] = t->state[index+1];
			t->state[index+1] = temporary;
			nodesExpanded++;
		}
		else
			t->state.erase(t->state.begin(), t->state.end());
	} else if (direction == 'd'){
		if ((index < 6) && (!wrongMove)) { // Try adding this:  && (!(index == current->index))
			temporary = t->state[index];
			t->state[index] = t->state[index+3];
			t->state[index+3] = temporary;
			nodesExpanded++;
		}
		else
			t->state.erase(t->state.begin(), t->state.end());
	} else if (direction == 'l'){
		if ((index%3 != 0) && (!wrongMove)) {
			temporary = t->state[index];
			t->state[index] = t->state[index-1];
			t->state[index-1] = temporary;
			nodesExpanded++;
		}
		else
			t->state.erase(t->state.begin(), t->state.end());
	}
}


/*
 organize() function's job is to sort the stack of nodes
 based on their cost. If a node has a higher cost than
 the node next to it, those nodes are swapped.
 Goal is to create a vector with nodes that have lower costs
 in the front
	A* search algorithm picks a node with the lowest cost first
 */
void TREE::organize() {
	Node* temporary;
	for (int i = 0; i < (signed)stack.size() - 1; i++) {
		for (int j = 0; j < (signed)stack.size() - 1; j++) {
			if (stack[j]->cost > stack[j+1]->cost) {
				temporary = stack[j];
				stack[j] = stack[j+1];
				stack[j+1] = temporary;
			}
		}
	}
}


void TREE::didItSucceed(Node* current) {
	bool	didItPass1 = true,
	didItPass2 = true,
	didItPass3 = true,
	didItPass4 = true;
	
	for (int i = 0; i < (signed)goal_state.size(); i++) {
		if (current->child1 != NULL){
			if (current->child1->state[i] != goal_state[i])
				didItPass1 = false;
		}
		else
			didItPass1 = false;
		if (current->child2 != NULL) {
			if (current->child2->state[i] != goal_state[i])
				didItPass2 = false;
		}
		else
			didItPass2 = false;
		if (current->child3 != NULL) {
			if (current->child3->state[i] != goal_state[i])
				didItPass3 = false;
		}
		else
			didItPass3 = false;
		if (current->child4 != NULL) {
			if (current->child4->state[i] != goal_state[i])
				didItPass4 = false;
		}
		else
			didItPass4 = false;
	}
	
	/*
	 When one of the following if statements is true,
	 it will send the node with the goal state to the print().
	 */
	if (didItPass1) {
		success = true;
		cout << "DONE!\n" << endl;
		print(current->child1);
		
	}
	else if (didItPass2) {
		success = true;
		cout << "DONE!\n" << endl << endl;
		print(current->child2);
	}
	else if (didItPass3) {
		success = true;
		cout << "DONE!\n" << endl;
		print(current->child3);
	}
	else if (didItPass4) {
		success = true;
		cout << "DONE!\n" << endl;
		print(current->child4);
	}
	
	if (success) {
		while (!stack.empty())
			stack.erase(stack.begin());
	}
}

void TREE::print(Node* successNode) {

	vector<Node*> tempVector;
	int tempCost = successNode->cost;
	//int tempDistance = successNode->distance;
	
	
	/*
	 The while loop below backtracks to the root node
	 and comes down the tree to the node with the goal state.
	 */
	
	while (successNode != NULL) {
		tempVector.push_back(successNode);
		
		/*
		 Get the success node's direction and push it in the stack
		 */
		pathToGoal.push(successNode->direction);
		
		// Track back to parent node
		successNode = successNode->parent;
	}
	cout << "Initial State" << endl;
	for (int j = (signed)tempVector.size()-1; j>=0; j--) {
		for (int i = 0; i < (signed)tempVector[j]->state.size(); i++) {
			if (tempVector[j]->state[i] == 0)
				cout << "0 ";
			else
				cout << tempVector[j]->state[i] << " ";
			if (i%3 == 2) cout << endl;
		}
		cout << endl;
		if(j - 1 == 0){
			cout << "Goal State reached" << endl;
		}
	}
	
	cout << "====================================\n";
	cout << "\t\t\tSUMMARY\n";
	cout << "Stack depth: " << stack.size() << endl;
	cout << "Cost: " << tempCost - 1 << endl;
	//cout << "Distance: " << tempDistance << endl;

}

void search(string searchType){
	clock_t start, finish, total;
	start = clock();
	
	TREE treeObject;
	treeObject.construct(searchType);
	
	
	finish = clock();
	total = finish - start;
	
	cout << "Nodes Expanded: " << treeObject.nodesExpanded << endl;
	cout << "Runtime: " << total << " msecs" << endl;
	cout << "Max stack: " << treeObject.stackDepth << endl;
	
	cout << "Path to the solution: ";
	while(!treeObject.pathToGoal.empty()){
		cout << treeObject.pathToGoal.top() << " ";
		treeObject.pathToGoal.pop();
	}
	cout << "\n========================================" <<endl;
}


// MAIN
int main() {
	
	search("BFS");
	search("A*");
	search("DFS");

}








