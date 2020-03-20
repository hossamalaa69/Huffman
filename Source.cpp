#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <map>

using namespace std;

struct node {
	char c = '$';
	double val = 0;
	node* left = NULL;
	node* right = NULL;

	node(char c, double val) :c(c), val(val), left(NULL), right(NULL) {}
	node() :c('$'), val(0), left(NULL), right(NULL) {}

};

struct OrderByValue {
	bool operator() (node const& a, node const& b) { return a.val > b.val; }
};

map < char, string > Dictionary;
map <string, char > InverseDic;
char EndOfFile = '#';
#define ConstSize 31457280

//this function aims to build both Dictionary and inverse Dictionary
void HuffmanRec(node n, string code = "") {
	if (n.c == '$') {
		HuffmanRec(*n.left, code + "1");
		HuffmanRec(*n.right, code + "0");
	}
	else
	{
		Dictionary[n.c] = code;
		InverseDic[code] = n.c;
	}
}

void ViewDictionary() {
	cout << endl;
	map<char, string>::iterator it2;
	for (it2 = Dictionary.begin(); it2 != Dictionary.end(); it2++) {
		if (it2->first == ' ')
			cout << "space" << ":" << it2->second << endl;
		else
			cout << it2->first << ":" << it2->second << endl;
	}

	cout << "\n inverse Dictionary:" << endl << endl;

	map<string, char>::iterator it3;
	for (it3 = InverseDic.begin(); it3 != InverseDic.end(); it3++) {
		if (it3->second == ' ')
			cout << it3->first << ":" << "space" << endl;
		else
			cout << it3->first << ":" << it3->second << endl;
	}
}

//this function searches for corresponding character in Inverse Dictionary
//returns character if it exists, otherwise it returns '$'

char searchInDic(string x) {
	map<string, char>::iterator itInv;
	itInv = InverseDic.find(x);
	if (itInv == InverseDic.end())
		return '$';
	else
		return InverseDic[x];
}

string getBinary(int a) {
	string s = "";    //store 8 bits binary of char
	if (a < 0)
		a += 256;          //if ascii code is nagative
	while (a != 0) {
		s += ((a % 2) + 48);   
		a >>= 1;
	}
	while (s.length() != 8) {   //if code length <8 , then fill MSB by zeros to be 8 bits
		s += "0";
	}

	string reflectS = "";     //rotate string to be in form of binary

	for (int i = 7; i >= 0; i--)
		reflectS += s[i];

	return reflectS;
}




void Encode2Binary(string Block, char& Bin) {
	int BinaryByte = 0;
	int BlockSz = Block.size();
	int val;
	for (int i = 0; i < BlockSz; i++) {
		BinaryByte = BinaryByte << 1;
		val = 0;
		if (Block[i] == '1') val++;
		BinaryByte |= val;

	}
	if (BlockSz < 8) BinaryByte = BinaryByte << (8 - BlockSz);
	Bin = BinaryByte;
}

int main() {

	//input file
	ifstream IpFileObj;
	IpFileObj.open("input.txt");

	//Dictionary for calculating probability of each character
	map<char, double> prob;

	int TotalNoChars = 0;     // total number of characters in file
	char CurrentChar;

	//while there're characters in file, increase count
	while (IpFileObj.get(CurrentChar)) {
		TotalNoChars++;
		prob[CurrentChar]++;
	}

	//after reading we don't need file anymore
	IpFileObj.close();

	//special case
	if (!TotalNoChars) {

		cout << endl << "File is empty" << endl;

		ofstream binEncodedFile("Encoded.bin", ios::out | ios::binary);   //to out encoded chars as binary
		binEncodedFile.clear();
		ofstream Decoded_File("Decoded.txt", ios::out | ios::trunc);   //create a new file to store decoded characters
		Decoded_File.clear();

		return 0;
	}
	// adding indication for file ending
	TotalNoChars++;
	prob[EndOfFile]++;
	// priority queue for nodes

	priority_queue<node, vector<node>, OrderByValue> pq;

	node* n;    //template

	//push each char with it's probability calculated
	map<char, double>::iterator it;
	for (it = prob.begin(); it != prob.end(); it++) {
		it->second /= TotalNoChars;
		n = new node(it->first, it->second);
		pq.push(*n);
	}

	//templates
	node* n1, * n2;

	//generating tree
	while (pq.size() > 1) {
		n1 = new node(pq.top());
		pq.pop();
		n2 = new node(pq.top());
		pq.pop();
		n = new node();
		n->val = n1->val + n2->val;
		n->left = n1;
		n->right = n2;
		pq.push(*n);
	}

	node head = pq.top();
	pq.pop();
	if (prob.size() > 1) {
		HuffmanRec(head);
	}
	else {
		HuffmanRec(head, "0");
	}


	//out decoded binary for each correspoding character in input file
	IpFileObj.open("input.txt");  //to read input as chars
	ofstream binEncodedFile("Encoded.bin", ios::out | ios::binary);   //to out encoded chars as binary
	binEncodedFile.clear();
	//Making coded string
	char* Code = new char[ConstSize];
	long int index = 0;
	while (IpFileObj.get(CurrentChar)) {
		for (int i = 0; i < Dictionary[CurrentChar].size(); i++) {
			Code[index] = Dictionary[CurrentChar][i];
			index++;
		}
	}
	//adding EOF code
	for (int i = 0; i < Dictionary[EndOfFile].size(); i++) {
		Code[index] = Dictionary[EndOfFile][i];
		index++;
	}
	//Outing in binary file
	//dividing code into blocks of 8bits(bytes) to be stored in file
	string CurrentBlock;
	char* EncodedBlock = new char[ConstSize];
	long int EBix = 0;
	int ix = 0;
	int CodeLen = index;
	char EncodedVal;
	while (ix < CodeLen) {
		CurrentBlock = "";
		if (CodeLen - ix >= 8) {
			for (int i = 0; i < 8; i++) {
				CurrentBlock += Code[ix];
				ix++;
			}
		}
		else {
			int dummy = ix;
			for (int i = 0; i < CodeLen - dummy; i++) {
				CurrentBlock += Code[ix];
				ix++;
			}
		}
		Encode2Binary(CurrentBlock, EncodedVal);
		EncodedBlock[EBix] = EncodedVal;
		EBix++;
	}
	ix = 0;
	int EncBlockSz = EBix;
	while (ix < EncBlockSz) {
		binEncodedFile << EncodedBlock[ix];
		ix++;
	}

	IpFileObj.close();
	binEncodedFile.close();

	ViewDictionary();

	//----------------------------End of Encoding-----------------------//

	//------------------------------------------------------------------//



	ifstream EncodedFile("Encoded.bin", ios::in | ios::binary);

	ofstream Decoded_File("Decoded.txt", ios::out | ios::trunc);   //create a new file to store decoded characters
	Decoded_File.clear();

	//store all chars in encoded file
	char* AllBinary = new char[ConstSize];
	long int count = 0;    //count of chars
	while (EncodedFile.get(CurrentChar)) {
		string binaryOfChar = getBinary((int)CurrentChar);     //store binary of each char
		for (int i = 0; i < 8; i++) {
			AllBinary[count] = binaryOfChar[i];
			count++;
		}
	}

	string x = "";
	for (int i = 0; i < count; i++) {
		x += AllBinary[i];
		char decoded = searchInDic(x);     //check if exixts
		if (decoded == '#')
			break;
		if (decoded != '$') {
			Decoded_File << decoded;       //if exists, then print in output file
			x = "";                       //emptying x
		}
	}

	EncodedFile.close();
	Decoded_File.close();

	return 0;
}