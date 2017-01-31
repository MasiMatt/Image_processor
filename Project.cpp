// C++ implemntation of search() and traverse() methods
#include<iostream>
#include <cstdlib>
#include <vector>
#include<cmath>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <sstream>
using namespace std;

// A BTree node
class BTreeNode
{
public:
    double *keys;  // An array of keys
    int t;      // Minimum degree (defines the range for number of keys)
    BTreeNode **C; // An array of child pointers
    int n;     // Current number of keys
    bool leaf; // Is true when node is leaf. Otherwise false

    BTreeNode(int _t, bool _leaf)   // Constructor
    {
        // Copy the given minimum degree and leaf property
        t = _t;
        leaf = _leaf;

        // Allocate memory for maximum number of possible keys
        // and child pointers
        keys = new double[2*t-1];
        C = new BTreeNode *[2*t];

        // Initialize the number of keys as 0
        n = 0;
    }

    void insertNonFull(double k)
    {
        // Initialize index as index of rightmost element
        int i = n-1;

        // If this is a leaf node
        if (leaf == true)
        {

            while (i >= 0 && keys[i] > k)
            {
                keys[i+1] = keys[i];
                i--;
            }

            // Insert the new key at found location
            keys[i+1] = k;
            n = n+1;
        }
        else // If this node is not leaf
        {
            // Find the child which is going to have the new key
            while (i >= 0 && keys[i] > k)
                i--;

            // See if the found child is full
            if (C[i+1]->n == 2*t-1)
            {
                // If the child is full, then split it
                splitChild(i+1, C[i+1]);

                if (keys[i+1] < k)
                    i++;
            }
            C[i+1]->insertNonFull(k);
        }
    }

    void splitChild(int i, BTreeNode *y)
    {
        // Create a new node which is going to store (t-1) keys
        // of y
        BTreeNode *z = new BTreeNode(y->t, y->leaf);
        z->n = t - 1;

        // Copy the last (t-1) keys of y to z
        for (int j = 0; j < t-1; j++)
            z->keys[j] = y->keys[j+t];

        // Copy the last t children of y to z
        if (y->leaf == false)
        {
            for (int j = 0; j < t; j++)
                z->C[j] = y->C[j+t];
        }

        // Reduce the number of keys in y
        y->n = t - 1;

        // Since this node is going to have a new child,
        // create space of new child
        for (int j = n; j >= i+1; j--)
            C[j+1] = C[j];

        // Link the new child to this node
        C[i+1] = z;

        for (int j = n-1; j >= i; j--)
            keys[j+1] = keys[j];

        // Copy the middle key of y to this node
        keys[i] = y->keys[t-1];

        // Increment count of keys in this node
        n = n + 1;
    }

    // A function to traverse all nodes in a subtree rooted with this node
    vector <double> FindVectors(double key, double range)
    {
        // There are n keys and n+1 children, travers through n keys
        // and first n children
        int i;
        static int ctr = 0;
        static vector<double> vec(100);
        for (i = 0; i < n; i++)
        {
            // If this is not leaf, then before printing key[i],
            // traverse the subtree rooted with child C[i].
            if (leaf == false)
                C[i]->FindVectors(key, range);

            else if(keys[i] > key-range/2 && keys[i]  < key+range/2)
            {
                cout << keys[i] << " ";
                vec[ctr] = keys[i];
                ctr++;
            }
        }

        // Print the subtree rooted with last child
        if (leaf == false)
            C[i]->FindVectors(key, range);

        ctr = 0;
        //vec.clear();
        return vec;
    }

};

class BTree
{
    BTreeNode *root; // Pointer to root node
    int t;  // Minimum degree
public:
    // Constructor (Initializes tree as empty)
    BTree(int _t)
    {
        root = NULL;
        t = _t;
    }

    // function to traverse the tree
    vector <double> FindVectors(double key, double range)
    {
        vector<double> vec(100);
        if (root != NULL)
            vec = root->FindVectors(key, range);

        return vec;
    }

    // The main function that inserts a new key in this B-Tree
    void insert(double k)
    {
        // If tree is empty
        if (root == NULL)
        {
            // Allocate memory for root
            root = new BTreeNode(t, true);
            root->keys[0] = k;  // Insert key
            root->n = 1;  // Update number of keys in root
        }
        else // If tree is not empty
        {
            // If root is full, then tree grows in height
            if (root->n == 2*t-1)
            {
                // Allocate memory for new root
                BTreeNode *s = new BTreeNode(t, false);

                // Make old root as child of new root
                s->C[0] = root;

                // Split the old root and move 1 key to the new root
                s->splitChild(0, root);

                int i = 0;
                if (s->keys[0] < k)
                    i++;
                s->C[i]->insertNonFull(k);

                // Change root
                root = s;
            }
            else  // If root is not full, call insertNonFull for root
                root->insertNonFull(k);
        }
    }

    double CalculateKey(vector<int> vec)
    {
        double key = 0;
        for(int x = 0; x<vec.size(); x++)
            key = key + pow(vec[x],2);

        key = (sqrt(key))/200;
        return key;
    }
};

// Driver program to test above functions
int main()
{
    BTree t(2); // A B-Tree with minium degree 2
    vector <double> smallVec;
    vector <double> smallVec2(32000);
    vector <double> temp(32000);
    vector <int> summationVec(32000);
    vector< vector<int> > myVector;
    int ctr = -1;

    ifstream file("Dataset.txt");
    string line;
    double key = 0;
    cout << "Loading Database........";
    while(!file.eof())
    {
        getline(file, line);
        istringstream fin(line);
        stringstream ss(line);
        istream_iterator<int> begin(ss);
        istream_iterator<int> end;
        vector<int> data(begin, end);
        key = t.CalculateKey(data);
        myVector.push_back(data);
        smallVec.push_back(key);
        t.insert(key);
    }

    file.close();

    string strtest;

    cout << endl << "Enter the name of input file: " << endl;
    getline(cin, strtest);

    ifstream input(strtest);
    string line2;
    double key2 = 0;
    while(!input.eof())
    {
        getline(input, line2);
        istringstream fin(line2);
        stringstream ss2(line2);
        istream_iterator<int> begin(ss2);
        istream_iterator<int> end;
        vector<int> data2(begin, end);
        key2 = t.CalculateKey(data2);
        cout << endl << endl << "Entered key is: " << key2 << endl;

    }
    cin.clear();

    double distance = 0;

    cout << endl << "Enter Euclidean distance: " << endl;
    cin >> distance;

    cout << endl << "Keys within range: " << endl;
    temp = t.FindVectors(key2, distance);

    cin.clear();
    cin.ignore();

    for(int x=0; x<500; x++)
    {
        for(int y=0; y<smallVec.size(); y++)
        {
            if(temp[x] == smallVec[y] && temp[x] != 0)
            {
                smallVec2[++ctr] = y;
                break;
            }
        }
    }

    cout << endl;

    for(int x=0; x<6400; x++)
    {
        for(int y=0; y<=ctr; y++)
        {
            if(myVector[smallVec2[y]][x] <= 255 && myVector[smallVec2[y]][x] >= 0)
                summationVec[x] += myVector[smallVec2[y]][x];
        }
        summationVec[x] = summationVec[x]/(ctr+1);
    }

    string outtest;

    cout << endl << "Save output as: " << endl;
    getline (cin,outtest);

    ofstream myfile (outtest);
    if (myfile.is_open())
    {
        myfile << "P2\n";
        myfile << "80 80\n";
        myfile << "255\n";
        for(int x=0; x<32000; x++)
            myfile << summationVec[x] << " ";

        myfile.close();
    }
    else cout << "Unable to open file";
    cin.clear();
}
