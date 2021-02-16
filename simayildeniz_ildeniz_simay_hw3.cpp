# include <iostream>
# include <string>
# include <vector>
# include <algorithm>
# include <thread>
# include <fstream>
# include <sstream>
# include <chrono>
# include <map>
# include "WordItem.h"
# include "BinarySearchTree.h"
# include "HashTable.h"

using namespace std;

WordItem item_NOT_found;

BinarySearchTree<WordItem> searchTree(item_NOT_found); //initial root is item not found
HashTable<WordItem> hashMap(item_NOT_found,53); // initially arraysize is 53 which is a prime number

string makeStringValid( string & word)
{
	//convert all letters of the word into lowercase
	for_each(word.begin(), word.end(), [](char & c) 
	{  c = ::tolower(c); });

	// for punctuation, digits and non letter, chars
	for (int i = 0; i < word.length(); i++) 
	{
		if( !((word[i] <= 'z' && word[i] >= 'a') || (word[i] <= 'Z' && word[i] >= 'A')) )  
		{
			word[i] = ' ' ;
		}
	}
	return word;
}

//reads the file word by word, lowercases and removes punction in every word
void readFile_and_UpdateTree_AND_UpdateHashTable(string fileName, BinarySearchTree<WordItem> & searchTree, HashTable<WordItem> & hashMap)
{
	fstream inFile;
	string word1;

	inFile.open(fileName.c_str());

	while (inFile >> word1)
	{
		word1 = makeStringValid(word1);
		istringstream iss(word1);
		string tempWord;
		while ( iss >> tempWord)
		{
			//cout << tempWord << endl;
			DocumentItem newDocument;
			newDocument.documentName = fileName;
			newDocument.count = 1;

			WordItem newItem = WordItem(tempWord, newDocument);

			////BST INSERT:
			if (searchTree.find(newItem) == item_NOT_found)
			{
				searchTree.insert(newItem);
			}
			else //node with the given keyword exists in bst:
			{
				WordItem * alreadyExist = &searchTree.find(newItem);
				//This means item already exists, but we are not sure about which file in the listOfDocuments it exists in
				//It may be the current fileName that this function is called
				//Or it may be the previous file


				bool isInCurrentFile = false;
				//if it exists in the current file I am reading:
				vector<DocumentItem> * tempListOfDoc = alreadyExist->access_listOfDocuments();
				for (int i = 0; i < (*tempListOfDoc).size(); i++)
				{
					if ((*tempListOfDoc)[i].documentName == fileName)
					{
						(*tempListOfDoc)[i].count += 1;
						isInCurrentFile = true;
					}
				}

				//if it exist in another file:
				//I should push back the current fileName to its listOfDocuments and count in that file will be 1
				if (isInCurrentFile == false)
				{
					DocumentItem tempDocItem;
					tempDocItem.documentName = fileName;
					tempDocItem.count = 1;

					alreadyExist->addTo_listOfDocuments(tempDocItem);
				}

			}

			////HT INSERT:
			if (hashMap.find(newItem) == item_NOT_found)
			{
				hashMap.insert(newItem);
			}
			else
			{
				WordItem * alreadyExist = &hashMap.find(newItem);
				//This means item already exists, but we are not sure about which file in the listOfDocuments it exists in
				//It may be the current fileName that this function is called
				//Or it may be the previous file


				bool isInCurrentFile = false;
				//if it exists in the current file I am reading:
				vector<DocumentItem> * tempListOfDoc = alreadyExist->access_listOfDocuments();
				for (int i = 0; i < (*tempListOfDoc).size(); i++)
				{
					if ((*tempListOfDoc)[i].documentName == fileName)
					{
						(*tempListOfDoc)[i].count += 1;
						isInCurrentFile = true;
					}
				}

				//if it exist in another file:
				//I should push back the current fileName to its listOfDocuments and count in that file will be 1
				if (isInCurrentFile == false)
				{
					DocumentItem tempDocItem;
					tempDocItem.documentName = fileName;
					tempDocItem.count = 1;

					alreadyExist->addTo_listOfDocuments(tempDocItem);
				}

			}

		}
	}
	inFile.close();
}

void printList( vector<string> & toDisplay)
{
	int doesExist = 0;
	for (int i = 0; i < toDisplay.size(); i++)
	{
		if (toDisplay[i] == "No document contains the given query")
		{
			doesExist += 1;
		}
	}
	if (doesExist == toDisplay.size())
	{
		cout << "No document contains the given query" << endl;
		toDisplay.resize(0);
		return;
	}
	else
	{
		for (int x = 0; x < toDisplay.size(); x++)
		{
			if (toDisplay[x] != "No document contains the given query")
			{
				cout << toDisplay[x] << endl;
			}	
		}
		toDisplay.resize(0);
	}
}

void processQuery( BinarySearchTree<WordItem> & searchTree, HashTable<WordItem> & hashMap, string & query, int & numOfFiles, const vector<string> fileNames)
{
	cin.ignore();
	getline(cin,query);
	cout << endl;

	// in order to keep every word inside query, use a vector:
	vector<string> lookFor;

	//make sure query has valid chars, else correct it:
	string TempElement;
	istringstream issTemp(query);


	while (issTemp >> TempElement)
	{
		TempElement = makeStringValid(TempElement);

		string element = TempElement;
		istringstream iss(element);
		while (iss >> element)
		{
			lookFor.push_back(element);
		}	
	}

	////////////////////////////////////////////////////////SEARCHING PART IN TREE:

	int k = 200;

	//auto start = chrono::high_resolution_clock::now();
	//clock_t begin_time = clock();

	vector<string> displayBST;
	vector<WordItem> resultsVectorBST;
	for (int i = 0; i < k; i++)
	{
		resultsVectorBST.resize(0);
		for (int i = 0; i < lookFor.size(); i++)
		{
			WordItem temp;
			temp.changekeyWord_as(lookFor[i]);
			temp = searchTree.find(temp);

			resultsVectorBST.push_back(temp);
		}
	}

	//float bst_time = float(clock() - begin_time) / (k);
	//auto BSTTime = chrono::duration_cast<chrono::nanoseconds> (chrono::high_resolution_clock::now() - start);

	for (int ii = 0; ii < fileNames.size(); ii++)
	{
		bool fileExist = true;
		for (int j = 0; j < resultsVectorBST.size(); j++)
		{
			bool wordExist = false;
			vector<DocumentItem> WItempDIlist = resultsVectorBST[j].getCopyof_listOfDocuments();
			for (int k = 0; k < WItempDIlist.size(); k++)
			{
				if (WItempDIlist[k].documentName == fileNames[ii])
				{
					wordExist = true;
					break;
				}
			}
			if (wordExist == false)
			{
				fileExist = false;
				break;
			}

		}
		if (fileExist == true)
		{
			//print
			for (int s = 0; s < resultsVectorBST.size(); s++)
			{
				string toDisplay = "in Document " + fileNames[ii] + ", " + resultsVectorBST[s].access_keyWord() + " found ";
				vector <DocumentItem> myList = resultsVectorBST[s].getCopyof_listOfDocuments();
				for (int ss = 0; ss < myList.size() ; ss++)
				{
					if (myList[ss].documentName == fileNames[ii])
					{
						toDisplay += to_string(myList[ss].count) + " times, ";
					}
				}

				toDisplay[ toDisplay.size() - 2 ] = '.';

				displayBST.push_back(toDisplay);
				//cout <<toDisplay << endl;
			}

		}
		else
		{
			//not found
			string toDisplay = "No document contains the given query";
			//cout <<  "No document contains the given query" << endl;
			displayBST.push_back(toDisplay);
		}


	}
	printList(displayBST);
	
	/*cout << "\nBinary Search Tree Time: " << double(BSTTime.count()) / double(k) << "\n";
	cout << endl;*/

	/////////////////////////////////////////////////////SEARCHING PART IN HASHMAP:

	//start = chrono::high_resolution_clock::now();
	//begin_time = clock();

	vector<string> displayHT;
	vector<WordItem> resultsVectorHT;
	for (int i = 0; i < k; i++)
	{
		resultsVectorHT.resize(0);
		for (int i = 0; i < lookFor.size(); i++)
		{
			WordItem temp;
			temp.changekeyWord_as(lookFor[i]);
			temp = hashMap.find(temp);

			resultsVectorHT.push_back(temp);
		}
	}
	//float ht_time = float(clock() - begin_time) / (k);
	//auto HTTime = chrono::duration_cast<chrono::nanoseconds> (chrono::high_resolution_clock::now() - start);

	for (int ii = 0; ii < fileNames.size(); ii++)
	{
		bool fileExist = true;
		for (int j = 0; j < resultsVectorHT.size(); j++)
		{
			bool wordExist = false;
			vector<DocumentItem> WItempDIlist = resultsVectorHT[j].getCopyof_listOfDocuments();
			for (int k = 0; k < WItempDIlist.size(); k++)
			{
				if (WItempDIlist[k].documentName == fileNames[ii])
				{
					wordExist = true;
					break;
				}
			}
			if (wordExist == false)
			{
				fileExist = false;
				break;
			}

		}
		if (fileExist == true)
		{
			//print
			for (int s = 0; s < resultsVectorHT.size(); s++)
			{
				string toDisplay = "in Document " + fileNames[ii] + ", " + resultsVectorHT[s].access_keyWord() + " found ";
				vector <DocumentItem> myList = resultsVectorHT[s].getCopyof_listOfDocuments();
				for (int ss = 0; ss < myList.size() ; ss++)
				{
					if (myList[ss].documentName == fileNames[ii])
					{
						toDisplay += to_string(myList[ss].count) + " times, ";
					}
				}

				toDisplay[ toDisplay.size() - 2 ] = '.';

				displayHT.push_back(toDisplay);
				//cout <<toDisplay << endl;
			}

		}
		else
		{
			//not found
			string toDisplay = "No document contains the given query";
			//cout <<  "No document contains the given query" << endl;
			displayHT.push_back(toDisplay);
		}
	}

	printList(displayHT);

	/*cout << "\nHash Table Time: " << double(HTTime.count()) / double(k) << "\n";
	cout << endl;*/

	//OBJECTION DEGISEN LINE LAR
	auto start = chrono::high_resolution_clock::now(); for (int x = 0; x < 200; x++){for (int i = 0; i < lookFor.size(); i++){WordItem temp; temp.changekeyWord_as(lookFor[i]);temp = hashMap.find(temp);}}
	auto BSTTime = chrono::duration_cast<chrono::nanoseconds> (chrono::high_resolution_clock::now() - start); cout << "\nBinary Search Tree Time: " << double(BSTTime.count()) / 200.0 << "\n"; cout << endl;

	start = chrono::high_resolution_clock::now(); for (int x = 0; x < 200; x++){for (int i = 0; i < lookFor.size(); i++){WordItem temp; temp.changekeyWord_as(lookFor[i]);temp = searchTree.find(temp);}}
	auto HTTime = chrono::duration_cast<chrono::nanoseconds> (chrono::high_resolution_clock::now() - start); cout << "\nHash Table Time: " << double(HTTime.count()) / 200.0 << "\n"; cout << endl;
	//OBJECTION DEGISEN LINE LAR

	///////Final Step:
	/////////////////////////Calculate the Speed-Up:
	float speed_Up;
	speed_Up = ((double(BSTTime.count()) / double(k)) / (double(HTTime.count()) / double(k)));
	cout << "Speed Up: " << speed_Up << endl;
	cout << endl;

}


int main()
{
	int numOfFiles;
	cout << "Enter number of input files: ";
	cin >> numOfFiles;

	vector<string> fileNames;

	for (int i = 0; i < numOfFiles; i++) //call this number of files times and use one tree
	{
		string fName;
		cout << "Enter " << to_string(i+1) << ". file name: ";
		cin >> fName;
		fileNames.push_back(fName);
	}
	cout << endl;

	for (int i = 0; i < fileNames.size(); i++)
	{
		readFile_and_UpdateTree_AND_UpdateHashTable(fileNames[i], searchTree, hashMap);
	}//after this function, I will have been done with the tree construction and hashmap filling

	cout << endl << endl << "After preprocessing, the unique word count is " << hashMap.getCurrentSize() << ". Current load ratio is " << double(hashMap.getCurrentSize()) / double(hashMap.getArraySize()) << endl;

	// Query Part
	string query;
	cout << endl << "Enter queried words in one line: ";

	processQuery(searchTree, hashMap, query, numOfFiles, fileNames);

	return 0;
}