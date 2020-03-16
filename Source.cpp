#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <map>
using namespace std;

struct node {
	char c = '$';
	double val=0;
	node *left = NULL;
	node *right = NULL;

	node(char c, double val):c(c), val(val), left(NULL),right(NULL){}
	node():c('$'), val(0), left(NULL), right(NULL){}

};

struct OrderByValue {
	bool operator() (node const& a, node const& b) { return a.val > b.val; }
};

map < char, string > Dictionary;
map <string, char > InverseDic;


//this function aims to build both dictionary and inverse dictionary
void HuffmanRec(node n, string code = "") {
	if (n.c == '$') {
		HuffmanRec(*n.left, code + "0");
		HuffmanRec(*n.right, code + "1");
	}
	else
	{
		Dictionary[n.c] = code;
		InverseDic[code] = n.c;
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

int main() {

	//input file
	ifstream IpFileObj;
	IpFileObj.open("input.txt");

	//dictionary for calculating probability of each character
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
		cout << "File is empty";
		return 0;
	}

	//priority queue for nodes

	priority_queue<node, vector<node>, OrderByValue> pq;
	
	node* n;    //template
	
	//push each char with it's probability
    map<char, double>::iterator it;
	for (it = prob.begin(); it != prob.end(); it++) {
		it->second /= TotalNoChars;
		n = new node(it->first,it->second);
		pq.push(*n);
	}
	
	//special case
	if(pq.size() == 1){
		cout << "Same character in file, Doesn't need to be compressed";
		return 0;
	}

    //templates
	node *n1, *n2;

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
	HuffmanRec(head);

	//cout << endl;
	//map<char, string>::iterator it2;
	//for (it2 = Dictionary.begin(); it2 != Dictionary.end(); it2++) {
	//	if(it2->first==' ')
	//		cout << "space" << ":" << it2->second << endl;
	//	else
	//	    cout << it2->first << ":" << it2->second << endl;
	//}

	//cout << "\n Inverse Dictionary:" << endl << endl;

	//map<string, char>::iterator it3;
	//for (it3 = InverseDic.begin(); it3 != InverseDic.end(); it3++) {
	//	if (it3->second == ' ')
	//		cout << it3->first << ":" << "space" << endl;
	//	else
	//		cout << it3->first << ":" << it3->second << endl;
	//}



	//out decoded binary for each correspoding character in input file
	IpFileObj.open("input.txt");  //to read input as chars
	ofstream binEncodedFile("Encoded.bin", ios::out | ios::binary);   //to out encoded chars as binary
	
	string CurrentCode = "";

	while (IpFileObj.get(CurrentChar)) {
		CurrentCode = Dictionary[CurrentChar];
		for (size_t i = 0; i < CurrentCode.length(); i++)
		{
			if (CurrentCode[i] == '1')
				binEncodedFile << 1;
			else
				binEncodedFile << 0;
		}
	}
    
	IpFileObj.close();
	binEncodedFile.close();

//----------------------------End of Encoding-----------------------//

//------------------------------------------------------------------//
	
	
	
	ifstream EncodedFile("Encoded.bin", ios::in | ios::binary);
	
	ofstream Decoded_File("Decoded.txt", ios::out | ios::trunc);   //create a new file to store decoded characters

	string x="";     //temp for current string of binary code
	int CurrentBit = 0;
	while(EncodedFile.get(CurrentChar)){
		x += CurrentChar;                   //concatinate bits
		char decoded = searchInDic(x);     //check if exixts
		if (decoded != '$'){              
			Decoded_File << decoded;       //if exists, then print in output file
			x = "";                       //emptying x
		}
	}

	EncodedFile.close();
	Decoded_File.close();

	return 0;
}