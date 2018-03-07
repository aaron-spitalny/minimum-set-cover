#include <iostream>
#include <list>
#include <set>
#include <fstream>
#include <sstream>

using namespace std;

class Set
{
public:
    int domain;
    int numberOfSubsets;
    int*** setCover;
    Set(int d, int s)
    {
        domain = d;
        numberOfSubsets = s;
        setCover = new int**[numberOfSubsets+3];
        for(int i = 0; i < numberOfSubsets+3; i++) {
            setCover[i] = new int*[domain];
            for(int j = 0; j < domain; j++) {
                setCover[i][j] = new int[domain];
            }
        }
        for (int i = 0; i < numberOfSubsets+3; i++) {
            for (int j = 0; j < domain; j++) {
                setCover[i][j][0] = 0;
            }
        }
    }
    ~Set()
    {
        for(int i = 0; i < numberOfSubsets+3; i++) {
            for(int j = 0; j < domain; j++) {
                delete setCover[i][j];
            }
            delete [] setCover[i];
        }
        delete [] setCover;
    }

    void print(int z)
    {
        cout << endl;
        for (int i = 0; i < numberOfSubsets+3; i++) {
            cout << i << ": ";
            for (int j = 0; j < domain; j++) {
                cout << setCover[i][j][z]<< " ";
            }
            cout << endl;
        }
    }
};

void swapRows(Set* theSet, int row1, int row2, int* someArray, int z)
{
    int temp;
    temp = someArray[row1];
    someArray[row1] = someArray[row2];
    someArray[row2] = temp;
    for(int i = 0; i < theSet->domain; i++) {
        temp = theSet->setCover[row1][i][z];
        theSet->setCover[row1][i][z] = theSet->setCover[row2][i][z];
        theSet->setCover[row2][i][z] = temp;
    }
}

void orderSubsets(Set* theSet, int z)
{
    int sizeOfSubsets1[theSet->numberOfSubsets];
    int sizeOfSubsets2[theSet->numberOfSubsets];
    int count;
    for (int i = 1; i < theSet->numberOfSubsets; i++) {
        count = 0;
        for (int j = 1; j < theSet->domain; j++) {
            if(theSet->setCover[i][j][z]) {
                count++;
            }
            sizeOfSubsets1[i] = count;
            sizeOfSubsets2[i] = count;
        }
    }
    sort(sizeOfSubsets2+1, sizeOfSubsets2+theSet->numberOfSubsets, greater<int>());
    for (int i = 1; i < theSet->numberOfSubsets; i++) {
        for (int j = 1; j < theSet->numberOfSubsets; j++) {
            if(sizeOfSubsets2[i] == sizeOfSubsets1[j] && i != j) {
                swapRows(theSet, i, j, sizeOfSubsets1, z);
            }
        }
    }
}

int num = 100;


bool isASolution(Set* theSet, int z)
{
    return (theSet->setCover[0][0][z] == theSet->domain-1);
}

void processASolution(Set* theSet, int z)
{
    if(num == 100 || theSet->setCover[theSet->numberOfSubsets+2][0][z] < num ) {
        num = theSet->setCover[theSet->numberOfSubsets+2][0][z];
    }
    for(int i = 0; i < theSet->domain; i++) {
        cout << theSet->setCover[theSet->numberOfSubsets+2][i][z] << " ";
    }
    cout << endl;
}

void constructCandidate(Set* theSet, int z, int* c, int* candidates)
{
   // orderSubsets(theSet, z);
    //cout << z << endl;
    *candidates = 0;
    int column = 0;
    int smallest = 0;
    for(int i = 1; i < theSet->domain; i++) {
        //select column if it has only one subset
        if(theSet->setCover[0][i][z] == 1) {
            column = i;
            break;
        } else if((theSet->setCover[0][i][z] < smallest && theSet->setCover[0][i][z] !=0) || (smallest == 0  && theSet->setCover[0][i][z] !=0)) {
            column = i;
            smallest = theSet->setCover[0][i][z];
        } else if(theSet->setCover[0][i][z] == smallest) {
            if(theSet->setCover[theSet->numberOfSubsets][i][z] < theSet->setCover[theSet->numberOfSubsets][column][z]) {
                column = i;
            } else if(theSet->setCover[theSet->numberOfSubsets][i][z] == theSet->setCover[theSet->numberOfSubsets][column][z]) {
                if(theSet->setCover[theSet->numberOfSubsets+1][i][z] < theSet->setCover[theSet->numberOfSubsets+1][column][z]) {
                    column = i;
                }
            }
        }
    }
    //get candidates from the column
    for(int i = 1; i < theSet->numberOfSubsets; i++) {
        if(theSet->setCover[i][column][z]) {
            c[*candidates] = i;
            *candidates = *candidates + 1;
        }
    }

}

//calculate how many columns will be covered when a column is deleted and variance
void updateColumnCover(Set* theSet, int z)
{
    int variance[theSet->domain];
    variance[0] = 0;
    int count = 1;
    int mean = 0;
    int cover[theSet->domain];
    for(int i = 0; i < theSet->domain; i++) {
        cover[i] = 0;
    }
    for(int i = 1; i < theSet->domain; i++) {
        for(int j = 1; j < theSet->numberOfSubsets; j++) {
            if(theSet->setCover[j][i][z]) {
                mean = theSet->setCover[j][0][z] + mean;
                variance[count] = theSet->setCover[j][0][z];
                count++;
                for(int k = 1; k < theSet->domain; k++) {
                    if(theSet->setCover[j][k][z]) {
                        if(cover[k] == 0) {
                            cover[k] = 1;
                            cover[0]++;
                        }
                    }
                }
            }
        }
        theSet->setCover[theSet->numberOfSubsets][i][z] = cover[0];
        mean = mean/2;
        for (int i = 1; i < count; i++) {
            variance[0] += ((variance[i] - mean) * (variance[i] - mean))/count;
        }
        theSet->setCover[theSet->numberOfSubsets+1][i][z] = variance[0];
        for(int i = 0; i < theSet->domain; i++) {
            cover[i] = 0;
        }
        count = 0;
        variance[0] = 0;
    }
}

//constructs the 2d matrix for the candidate
void processCandidate(Set* theSet, int z, int candidate)
{
    //copy the 2d array from z-1 to z
    for (int i = 0; i < theSet->numberOfSubsets+3; i++) {
        for (int j = 0; j < theSet->domain; j++) {
            theSet->setCover[i][j][z] = theSet->setCover[i][j][z-1];
        }
    }
    //increment the number of candidates
    theSet->setCover[theSet->numberOfSubsets+2][0][z]++;

    //clear the columns that contain the candidate
    for(int i = 1; i < theSet->domain; i++) {
        if(theSet->setCover[candidate][i][z]) {
            theSet->setCover[0][i][z] = 0;
            theSet->setCover[0][0][z]++;
            theSet->setCover[theSet->numberOfSubsets][i][z] = 0;
            theSet->setCover[theSet->numberOfSubsets+1][i][z] = 0;
            theSet->setCover[theSet->numberOfSubsets+2][i][z] = candidate;
            for(int j = 0; j < theSet->numberOfSubsets; j++) {
                if(theSet->setCover[j][i][z]) {
                    theSet->setCover[j][i][z] = 0;
                    theSet->setCover[j][0][z]--;
                }
            }
        }
    }
    updateColumnCover(theSet, z);
}


void backtrack(Set *theSet, int z)
{
    theSet->print(z);
    int c[theSet->numberOfSubsets-1];  //candidates for the next position
    int candidates;   //next position candidate count
    if(isASolution(theSet, z)) {
        processASolution(theSet, z);
    } else if(theSet->setCover[theSet->numberOfSubsets+2][0][z] >= num-1) {
        return;
    } else {
        constructCandidate(theSet, z, c, &candidates);
        z = z + 1;
        for(int i = 0; i < candidates; i++) {
            processCandidate(theSet, z, c[i]);
            backtrack(theSet, z);
        }
    }
}

void findUpperBound(Set* setOfSubsets, int** subsets, int *a, int numberOfSubs, int domain)
{
    bool flag = true;
    bool someOtherflag = true;
    while(subsets[numberOfSubs][domain] != (domain-1)) {
        while(flag) {
            flag = false;
            for(int i = 1; i < domain; i++) {
                if(subsets[numberOfSubs][i] == 1) { //find column with 1 cover
                    flag = true;
                    for(int j = 1; j < numberOfSubs; j++) { //find row that covers
                        if(subsets[j][i]) {
                            a[i] = j;
                            for(int k = 1; k < domain; k++) { //for every element in row j
                                if(subsets[j][k]) {
                                    a[k] = j;
                                    subsets[numberOfSubs][domain]++;
                                    subsets[numberOfSubs][k] = 0; // set total to 0
                                    for(int n = 1; n < numberOfSubs; n++) {//for every element k in row j  clear column k
                                        if(subsets[n][k]) {
                                            if(someOtherflag) {
                                                setOfSubsets->setCover[n][k] = 0;
                                            }
                                            subsets[n][k] = 0;
                                            subsets[n][domain]--;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        someOtherflag = false;
        int largest = 0;
        int index = 0;
        for(int i = 1; i < numberOfSubs; i++) {
            if(subsets[i][domain] > largest) {
                largest = subsets[i][domain];
                index = i;
            }
        }
        for(int k = 1; k < domain; k++) { //for every element
            if(subsets[index][k]) {
                a[k] = index;
                subsets[numberOfSubs][domain]++;
                for(int n = 1; n < numberOfSubs; n++) {
                    if(subsets[n][k]) {
                        subsets[n][k] = 0;
                        subsets[n][domain]--;
                    }
                }
            }
        }
    }

    set<int> cover;
    for(int i =1; i < domain; i++) {
        cout << a[i] << " ";
        cover.insert(a[i]);
    }
    cout << endl;
    cout << cover.size() << endl;
    num =  cover.size();
}

int main()
{
    ifstream infile("/Users/aaronspitalny/Desktop/MinimumSetCover/covered/s-c-12-6.txt");
    if ( !infile.is_open() ) {
        cout << endl << "ERROR: Unable to open input file" << endl;
        exit(1);
    }
    int domain;
    infile >> domain;
    int subsets;
    infile >> subsets;
    Set* theSet = new Set(domain+1, subsets+1);
    string subset;
    getline(infile,subset);
    int count = 1;
    int n;
    while(getline(infile,subset)) {
        stringstream stream(subset);
        while(stream >> n) {
            theSet->setCover[count][n][0]= 1;
            theSet->setCover[0][n][0]++;
            theSet->setCover[count][0][0]++;
        }
        count++;
    }
    infile.close();
    orderSubsets(theSet, 0);
    // theSet->print(0);
//    findUpperBound(theSet, subsets+1, domain+1);
updateColumnCover(theSet, 0);
    backtrack(theSet, 0);

    return 0;
}
