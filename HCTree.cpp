#include "HCTree.hpp"
#include <stack>



/*
Recursively prints out contents of the tree
*/
void printHuffTree(HCNode* node) {
	if (node == nullptr) return;
	cout << node->count << endl;
	printHuffTree(node->c0);
	printHuffTree(node->c1);
}


/*
Helper Recursive function to deallocate the entire tree
*/
void destructorHelper(HCNode* node) {
	if (node == nullptr) return;
	destructorHelper(node->c0);
	HCNode* Child1 = (node->c1);
	delete node;
	destructorHelper(Child1);
}

HCTree::~HCTree() {
	if (this->root == nullptr) return;
	HCNode* delNode = this->root;
	destructorHelper(delNode);
	this->root = nullptr;

}


/*
A mapping of the leaves.
Taken from this->leaves vector
Except it only has all the non-zero values
*/
map<char, int> HCTree::getNonZeroLeavesVector() const {
	map<char, int> nonZeroLeaves;
	for (auto i : this->leaves) {
		if (i != nullptr) {
			nonZeroLeaves[i->symbol] = i->count;
		}
	}
	return nonZeroLeaves;
}

void HCTree::build(const vector<int>& freqs)
{
	priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> pq;

	for (long unsigned int i = 0; i < freqs.size(); i++)
	{
		if (freqs[i] != 0) {
			//(count, symbol)
			HCNode* node = new HCNode(freqs[i], i);
			pq.push(node);
			this->leaves[i] = node;
			//leaves[i] i is the symbol
		}
	}

	while (pq.size() > 1) {
		//cout << pq.top()->count << endl;
		HCNode* newParentNode = new HCNode(0, 0);
		pq.top()->p = newParentNode;
		newParentNode->c0 = pq.top();
		pq.pop();

		pq.top()->p = newParentNode;
		newParentNode->c1 = pq.top();
		pq.pop();

		newParentNode->count = newParentNode->c0->count + newParentNode->c1->count;

		if (newParentNode->c0->count > newParentNode->c1->count) {
			newParentNode->c1->p = newParentNode->c0; //temporarily c1-p to c0 to retain connection
			newParentNode->c0 = newParentNode->c1; //fixing c0 to point to lower frequency node
			newParentNode->c1 = newParentNode->c0->p; //fixing c1 to point to higher frequency node using the temporarily link created above
			newParentNode->c0->p = newParentNode; //fixing c0 parent pointer and pointing it back to newParentNode
		}
		pq.push(newParentNode);
	}

	this->root = pq.top();

	//printHuffTree(this->root);
}


void HCTree::encode(unsigned char symbol, FancyOutputStream& out) const {
	HCNode* leafPointer = this->leaves[(int)symbol];

	stack<int> bitStack;
	while (leafPointer->p != nullptr) {
		if (leafPointer->p->c0 == leafPointer) bitStack.push(0);
		if (leafPointer->p->c1 == leafPointer) bitStack.push(1);
		leafPointer = leafPointer->p;
	}

	while (!bitStack.empty()) {
		out.write_bit(bitStack.top());
		bitStack.pop();
	}

}

unsigned char HCTree::decode(FancyInputStream& in) const {
	HCNode* node = this->root;

	while (node->c0 != nullptr) {
		int bit = in.read_bit();
		if (bit == 0) node = node->c0;
		if (bit == 1) node = node->c1;
		if (bit == -1) break;
	}
	return node->symbol;

}
