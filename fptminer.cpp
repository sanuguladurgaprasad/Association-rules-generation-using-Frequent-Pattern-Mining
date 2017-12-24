#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include <climits>
#include <vector>
#include <set>
#include <cstring>
#include <unordered_map>
#include <iterator>
#include <sstream>

using namespace std;

//structure of each node in FP Tree
struct fnode {
	int val;
	int count;
	int sup;
	fnode* succ;
	fnode* parent;
	fnode* aux;
};

//structure of Header node
struct header {
	int val;
	int count;
	fnode* succ;
};

vector<int> *Tid;
int* freq;
ofstream myOutput;
int Tran, Items, freq1, minSup;
double minConf;
fnode** tracker;
header *head;
unordered_map<string, int> htmap;
int itemSize;

//For generating token from a string with space as delimiter
template<typename Out>
void split(const string &s, char delim, Out result) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		*(result++) = item;
	}
}

vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, back_inserter(elems));
	return elems;
}

void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

template<typename Type>
void merge(Type &temp, int l, int m, int r){
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;
    int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = temp[l + i];
    for (j = 0; j < n2; j++)
        R[j] = temp[m + 1+ j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2){
        if (freq[L[i]-1] >= freq[R[j]-1]){
            temp[k] = L[i];
            i++;
        }
        else{
            temp[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1){
        temp[k] = L[i];
        i++;
        k++;
    }
    while (j < n2){
        temp[k] = R[j];
        j++;
        k++;
    }
}

template<typename Type>
void msort(Type &temp, int l, int r){
    if (l < r)
    {
        int m = l+(r-l)/2;
        msort(temp, l, m);
        msort(temp, m+1, r);
        merge(temp, l, m, r);
    }
}

//Efficient Recursive FPTree implementation without child pointers
//Reference : http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.162.1209&rep=rep1&type=pdf
void CreateFP(int item,fnode* prev,vector<int> section, int index) {
	set<int> s;
	fnode* node = new fnode;
	node->count = section.size();
	node->parent = prev;
	node->val = item;
	node->succ = NULL;
	node->aux = NULL;

	if (tracker[item - 1] == NULL) {
		tracker[item - 1] = node;
		for (int i = 0; i < freq1; i++) {
			if (head[i].val == item) {
				head[i].succ = node; break;
			}
		}
	}
	else {
		tracker[item - 1]->succ = node;
		tracker[item - 1] = node;
	}
	for (int i = 0; i < section.size(); i++) {
		if (Tid[section[i]].size() - 1 > index) {
			s.insert(Tid[section[i]][index + 1]);
		}
	}
	index++;
	set <int> ::iterator itr;
	for (itr = s.begin(); itr != s.end(); ++itr)
	{
		vector<int> sec;
		for (int i = 0; i < section.size(); i++) {
			if (Tid[section[i]].size() > index && Tid[section[i]][index] == (int)*itr) {
				sec.push_back(section[i]);
			}
		}
		CreateFP((int)*itr, node, sec, index);
	}
}

void FPtree() {
	set<int> s;
	for (int i = 0; i < Tran; i++) {
		if (Tid[i].size() > 0)s.insert(Tid[i][0]);
	}
	set <int> ::iterator itr;
	for (itr = s.begin(); itr != s.end(); ++itr)
	{
		vector<int> section;
		for (int i = 0; i < Tran; i++) {
			if ((Tid[i].size() > 0) && Tid[i][0] == (int)*itr) {
				section.push_back(i);
			}
		}
		CreateFP((int)*itr, NULL, section, 0);
	}
}
//Free the dynamically created objects
void freeTrees(header* tHead, int size) {
	for (int i = size - 1; i >= 0; i--) {
		if (tHead[i].succ != NULL) {
			fnode *track0 = tHead[i].succ;
			fnode *track1 = track0;
			while (track1 != NULL) {
				track1 = track0->succ;
				free(track0);
				track0 = track1;
			}
			track0 = NULL;
			track1 = NULL;
		}
	}
	free(tHead);
}

//FP Growth Algorithm implementation
void FPGrowth(header *tempHead,int size, string fPattern) {
	for (int i = size - 1; i >= 0; i--) {
		if (tempHead[i].count >= minSup) {
			string str = to_string(tempHead[i].val);
			str = str + " "+fPattern;
			htmap.insert(make_pair(str, tempHead[i].count));
			fnode *track1 = tempHead[i].succ;
			set <int> s;
			while (track1 != NULL) {
				fnode *track2 = track1;
				fnode *child = NULL;
				while (track2 != NULL) {
					track2 = track2->parent;
					if (track2 != NULL && track2->sup>=minSup) {
						s.insert(track2->val);
						if (track2->aux == NULL) {
							track2->aux = new fnode;
							track2->aux->count = track1->count;
							track2->aux->val = track2->val;
							track2->aux->succ = NULL;
							track2->aux->aux = NULL;
							track2->aux->parent = NULL;
						}
						else {
							track2->aux->count = track2->aux->count + track1->count;
						}
						if (child != NULL)child->parent = track2->aux;
						child = track2->aux;
						if (track2->parent == NULL)track2->aux->parent = NULL;
					}
				}
				track1 = track1->succ;
			}
			header *tHead = new header[s.size()];
			fnode** tempTracker = new fnode*[Items];
			for (int j = 0; j < Items; j++) {
				tempTracker[j] = NULL;
			}

			int c = 0;
			track1 = tempHead[i].succ;
			while (track1 != NULL) {
				fnode *track2 = track1;
				while (track2 != NULL) {
					track2 = track2->parent;
					if (track2!= NULL && track2->aux!=NULL && track2->sup>=minSup) {
						if (tempTracker[track2->val - 1] == NULL) {
							tempTracker[track2->val - 1] = track2->aux;
							tHead[c].succ = track2->aux;
							tHead[c].val = track2->aux->val;
							track2->aux = NULL;//detaching
							c++;
						}
						else {
							tempTracker[track2->val - 1]->succ = track2->aux;
							tempTracker[track2->val - 1] = track2->aux;
							track2->aux = NULL;//detaching
						}
					}
				}
				track1 = track1->succ;
			}
			for (int j = 0; j < s.size(); j++) {
				track1 = tHead[j].succ;
				int count = 0;
				while (track1 != NULL) {
					count = count + track1->count;
					track1 = track1->succ;
				}
				track1 = tHead[j].succ;
				while (track1 != NULL) {
					track1->sup = count;
					track1 = track1->succ;
				}
				tHead[j].count = count;
			}
			FPGrowth(tHead, s.size(),str);
			free(tempTracker);
			freeTrees(tHead, s.size());
		}
	}
}

//Apriori Rule generation function
vector<string> apGen(vector<string> v) {
	string s1, s2;
	size_t found1, found2;
	vector<string> newV;
	for (int i = 0; i < v.size(); i++) {
		found1 = v[i].find_last_of(" ");
		if (found1 != string::npos)s1 = v[i].substr(0, found1 + 1);
		else {
			s1 = ""; found1 = -1;
		}
		for (int j = i + 1; j < v.size(); j++) {
			found2 = v[j].find_last_of(" ");
			if (found2 != string::npos)s2 = v[j].substr(0, found2 + 1);
			else {
				s2 = ""; found2 = -1;
			}
			if (s1 == s2) {
				newV.push_back(s1 + v[i].substr(found1 + 1) + " " + v[j].substr(found2 + 1));
			}
			else break;
		}
	}
	return newV;
}

void rules(string s, vector<string> newV, int ruleCSize) {
	int k = itemSize,i=0;
	double conf;
	if (k > ruleCSize + 1) {
		while(i<newV.size()){
			vector<string> tokens = split(newV[i], ' ');
			string f1 = s;
			for (int j = 0; j < tokens.size(); j++) {
				size_t found = f1.find(tokens[j] + " ");
				while (found != string::npos && found>0 && f1[found - 1] != ' ') {
					found = f1.find(tokens[j] + " ", found + 1);
				}
				f1 = f1.substr(0, found) + f1.substr(found + 1 + tokens[j].length());
			}
			conf = (double)htmap.at(s)/htmap.at(f1);
			if (conf >= minConf) {
				myOutput << "|" << f1.erase(f1.length()-1,1) << "|" << newV[i] << "|" <<htmap.at(s)<<"|"<<conf <<endl;
				i++;
			}
			else
				newV.erase(newV.begin()+i);
		}
		rules(s, apGen(newV), ++ruleCSize);
	}
}

void RuleGen() {
	unordered_map<string, int>::iterator itr;
	vector<string> v;
	string s;
	for (itr = htmap.begin(); itr != htmap.end(); itr++)
	{
		s = itr->first;
		if (s.find(' ')) {
			v = split(s, ' ');
			itemSize = v.size();
			rules(s, v, 0);
		}
		v.clear();
	}
}

int main(int argc, char *argv[]) {
	int start_s,stop_s;
	start_s = clock();
	minSup = atoi(argv[1]);
	minConf = atof(argv[2]);
	string ipFile = argv[3];
	string opFile = argv[4];
	ifstream myInput(ipFile);
	myOutput.open(opFile);
//Reading transaction data from input file
	int T, I;
	Tran = INT_MIN;
	Items = INT_MIN;
	while (myInput >> T >> I) {
		if (T>Tran)Tran = T;
		if (I>Items)Items = I;
	}

	Tran++;
	Tid = new vector<int>[Tran];
	freq = new int[Items];
	for (int i = 0; i<Items; i++)freq[i] = 0;
	myInput.clear();
	myInput.seekg(0, ios::beg);

	while (myInput >> T >> I) {
		freq[I - 1]++;
	}
	myInput.clear();
	myInput.seekg(0, ios::beg);
	while (myInput >> T >> I) {
		if(freq[I-1] >= minSup)
			Tid[T].push_back(I);
	}
//Sorting each transaction in descending order of the item frequencies
	for (int i = 0; i < Tran; i++) {
		msort(Tid[i],0,Tid[i].size()-1);
	}
	freq1 = 0;
	for (int i = 0; i < Items; i++) {
		if (freq[i] >= minSup) freq1 += 1;
	}
	myInput.close();

	head = new header[freq1];
	tracker = new fnode*[Items];
	int c= 0;
	for (int i = 0; i < Items; i++) {
		if (freq[i] >= minSup) {
			head[c].val = i + 1;
			head[c].count = freq[i];
			head[c].succ = NULL;
			c++;
		}
	}
	for (int i = 0; i < Items; i++) {
		tracker[i] = NULL;//Keeps track of linked list recent node
	}
  FPtree();
	for (int j = 0; j < freq1; j++) {
		fnode* track1 = head[j].succ;
		while (track1 != NULL) {
			track1->sup = head[j].count;
			track1 = track1->succ;
		}
	}

	FPGrowth(head,freq1,"");
	stop_s = clock();
	cout << "Time taken for generating frequent itemsets: " << (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << endl;
	if(minSup>20){
		start_s = clock();
		RuleGen();
		stop_s = clock();
		cout << "Time taken for generating association rules: " << (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << endl;
	}
	else
	{
		string str;
		unordered_map<string, int>::iterator itr;
		for (itr = htmap.begin(); itr != htmap.end(); itr++) {
			str = itr->first;
			myOutput << "|"<<str.erase(str.length()-1,1) << "|{}|"<<itr->second <<"|-1"<< endl;
		}
	}
	myOutput.close();
}
