#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include "strutils.h"
#include "randgen.h"
#include <vector>
using namespace std;

#define CENTROID 5 //Number of clusters
#define DIM 10
#define REAL 5 //i.e. each REAL'th iteration we get the real queue situation from each the server

double cent[CENTROID][DIM]; // Coordinates of the clusters
int procesingPower [] = {30, 25, 10, 10, 15};// relative proc. power of cload server taken from official web sites and using benchmark tests.

bool proceed = true;

int nextQueueValue (int minIterValue, int currQueueValue, int step)//step is time to process a queue
{
	RandGen gen;
	int iterations = minIterValue/step;
	for(int i = 0; i<iterations; i++)
	{
		int number = gen.RandInt(-1,1);
		currQueueValue += number*step;
		if(currQueueValue < 0)
			currQueueValue = 0;
	}
	return currQueueValue;
}

string true_Test_Label(int ** & RTT_Test, int *procesingPower, int i)
	//returns the real value label, i.e. the one with minimum RTT+queue+processing for this iteration, C1 to C5
{
	int minIterValue = 6000; int minIndex = 0;
	for (int j = 0; j<5; j++)
	{

		int currServer = RTT_Test[i][j]+RTT_Test[i][j+5]+procesingPower[j];
		if(currServer<minIterValue)
		{
			minIterValue = currServer;
			minIndex = j;
		}
	}
	return "C"+itoa(minIndex+1);//C1 to C5
}

int findIterationTime(int ** & RTT_Test, int serverProcPower, int j_SelectedServer, int i)
	//finds the iteration time RTT+queue+processing for the selected server (it might have been selected wrongly)
{
	int time = RTT_Test[i][j_SelectedServer] + RTT_Test[i][j_SelectedServer+5] + serverProcPower;
	return time;
}

int findSelectedSever(string label)
{
	if(label=="C1")
		return 1;
	if(label=="C2")
		return 2;
	if(label=="C3")
		return 3;
	if(label=="C4")
		return 4;
	if(label=="C5")
		return 5;
}


void initializeCent( int maxTrainColumn[DIM], int minTrainColumn[DIM] ) // Places them randomly (Doesn't work well ? )
{
	RandGen myRand;
	for ( int i = 0; i < CENTROID ; i++ )
	{
		for ( int k = 0; k < DIM ; k++ )
		{
			cent[i][k] = myRand.RandInt( minTrainColumn[k] , maxTrainColumn[k] ) ;
		}
	}

} 

void initializeCent2( int ** & RTT_Test ) // Places them at a random point ( Works best )
{
	RandGen myRand;
	int pointID;
	for ( int i = 0; i < CENTROID ; i++ )
	{
		pointID = myRand.RandInt( 0, 19999 );
		for ( int k = 0; k < DIM ; k++ )
		{
			cent[i][k] = RTT_Test[pointID][k] ;
		}
	}

}

double calDist(unsigned int centID, unsigned int pointID, int ** & RTT) // Bad method -- Doesn't work
{
	double dist = 0;
	double a;
	for (unsigned int i= 0; i< DIM; i++)
	{
		a =  pow( ( cent[centID][i] - RTT[pointID][i] ) , 2); // (x1-x2) ^2
		dist += a;
	}

	dist = sqrt(dist); // Square root

	return dist;
}

int calDist2(unsigned int centID, unsigned int pointID, int ** & RTT) // Works excellent ( %90+ sucess rate )
{
	int RTTVal = RTT[pointID][centID];
	int QueVal = RTT[pointID][centID + 5];
	int PP = procesingPower[centID];

	return RTTVal + QueVal + PP;
	
}

bool moveCent(vector<vector<unsigned int>> & cGroups, unsigned int groupCount[DIM], int ** & RTT)
{
	cout << "Move" << endl;
	int sum;
	int a;
	unsigned int pointID;
	int average;
	bool moved = false; 
	for ( unsigned int i=0; i< CENTROID; i++) // Calculate average coordinate for each cluster
	{
		for ( unsigned int k= 0; k < DIM ; k++) // Calculate the average for each dimension
		{
			sum = 0;
			for ( unsigned int j = 0; j  < groupCount[i]; j++)
			{
				pointID = cGroups[i][j]; //Identify point
				a = RTT[pointID][k]; // Cordinate value
				sum += a;
			}
			average = sum / groupCount[i]; // Divide the sum by the number of elements in the group to get the average coordinate
			if ( moved == false && cent[i][k] != average )
			{
				moved = true;
			}
			cent[i][k]= average; // Assign average coordinate to the centroid
		}
	}
	cout << "Moved cents"<< endl; // For test purposes
	return moved;
}

void assignPoints(int ** & RTT)
{
	vector<vector<unsigned int>> cGroups(CENTROID); //Where the points will be assigned clusters
	unsigned int groupCount[CENTROID]; // Stores the number of elements in each group

	while ( proceed )
	{
		cout << "Assign"<< endl;
		double min ; // minimum distance
		double dis; // comparison distance
		unsigned int asCen; // Assigned centroid
		vector<vector<unsigned int>> cGroups(CENTROID); //Where the points will be assigned clusters
		for ( int i = 0; i < CENTROID; i++) // Reset group count
		{
			groupCount[i] = 0;
			cGroups[i].clear();
		}

		for ( unsigned int i = 0; i < 20000; i++) // For each point
		{
			min = DBL_MAX;
			for ( unsigned int k=0; k < CENTROID; k++) // Compare distanced with each centroid
			{
				dis = calDist2(k,i,RTT);
				if ( dis < min ) // Closer centroid has been found
				{
					min = dis;
					asCen = k;
				}
			}
			// Closest centroid has been assigned, now assigning it to the group
			cGroups[asCen].push_back(i); // Put the point inside the group
			groupCount[asCen]++; // Increase group count
		}
		proceed = moveCent(cGroups, groupCount, RTT);
	}
}

string kMeansPredict ( int index, int ** & RTT_Predict ) // Finds the closest centroid to the point
{
	double min = DBL_MAX; // minimum distance
	double dis; // comparison distance
	unsigned int asCen; // Assigned centroid

	for ( unsigned int k=0; k < CENTROID; k++) // Compare distances with each centroid
	{
		dis = calDist2(k, index ,RTT_Predict);
		if ( dis < min ) // Closer centroid has been found
		{
			min = dis;
			asCen = k;
		}
	}

	if ( asCen == 0 )
		return "C1";
	if ( asCen == 1 )
		return "C2";
	if ( asCen == 2 )
		return "C3";
	if ( asCen == 3 )
		return "C4";
	if ( asCen == 4 )
		return "C5";
}

int main()
{
	//int procesingPower [] = {30, 25, 10, 10, 15};// relative proc. power of cload server taken from official web sites and using benchmark tests.

	//READING RTT AND QUEUE TRAIN VALUES, THEY ARE STORED IN THE RTT[20000][10] MATRIX
	int **RTT = new int*[20000];
	for (int i = 0; i < 20000; i++)
	{
		RTT[i] = new int[10];
	}
	int *minIterValue = new int[20000];//mininmum value of the iteration
	string * iterationClass = new string [20000];//train labeling
	string data; 
	string filename[] = {"www.asiacloudserver.com.txt","www.cloudcentral.com.au.txt","www.rackspace.com.txt","www.rsaweb.co.za.txt","www.vps.net.txt",
		"asiacloudQueueTrain-30.txt" ,"cloudcentarQueueTrain-25.txt","rackspaceQueueTrain-10.txt","rsaQueueTrain-10.txt","vpsnetQueueTrain-15.txt"};
	ifstream file[10];
	for (int i =0 ;i <10; i++)
	{
		cout<<endl<<"Reading data from: "<<filename[i]<<endl;
		int count = 0;
		file[i].open(filename[i].c_str());
		while (file[i] >> data)
		{
			RTT[count][i] = atoi(data);
			count++;
		}
		cout << "Total "<<count<<" data read."<<endl;
	}

	// NO NEED TO READ LABELS BECAUSE K MEANS IS UNSUPERVISED LEARNING

	//READING RTT (TRUE) TEST VALUES, THEY ARE STORED IN THE RTT_Test[20000][10] MATRIX
	//QUEUE LENGTHS WILL BE DETERMINED DURING THE RUNTIME

	int **RTT_Test = new int*[1000];
	for (int i = 0; i < 1000; i++)
	{
		RTT_Test[i] = new int[10];
	}
	string filenameTest[] = {"www.asiacloudserver.comTest.txt","www.cloudcentral.com.auTest.txt","www.rackspace.comTest.txt","www.rsaweb.co.zaTest.txt","www.vps.netTest.txt"};
	ifstream fileTest[5];
	for (int i =0 ;i <5; i++)
	{
		cout<<endl<<"Reading data from: "<<filenameTest[i]<<endl;
		int count = 0;
		fileTest[i].open(filenameTest[i].c_str());
		while (fileTest[i] >> data)
		{
			RTT_Test[count][i] = atoi(data);
			count++;
		}
		cout << "Total "<<count<<" data read."<<endl;
	}

	//READING RTT PREDICTED VALUES, THEY ARE STORED IN THE RTT_PREDICT[20000][10] MATRIX
	//QUEUE LENGTHS WILL BE DETERMINED DURING THE RUNTIME

	int **RTT_Predict = new int*[1000];
	for (int i = 0; i < 1000; i++)
	{
		RTT_Predict[i] = new int[10];
	}
	string filenamePredict[] = {"www.asiacloudserver.comPredict.txt","www.cloudcentral.com.auPredict.txt","www.rackspace.comPredict.txt","www.rsaweb.co.zaPredict.txt","www.vps.netPredict.txt"};
	ifstream filePredict[5];
	for (int i =0 ;i <5; i++)
	{
		cout<<endl<<"Reading data from: "<<filenamePredict[i]<<endl;
		int count = 0;
		filePredict[i].open(filenamePredict[i].c_str());
		while (filePredict[i] >> data)
		{
			RTT_Predict[count][i] = atoi(data);
			count++;
		}
		cout << "Total "<<count<<" data read."<<endl;
	}

	//FINDING MIN AND MAX VALUES FOR TRAIN DATA FOR EACH COLUMN (NEEDED FOR NORMALIZATION)
	int maxTrainColumn[10] = {0,0,0,0,0,0,0,0,0,0}; 
	int minTrainColumn[10]={1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,};
	int distanceColumn[10];
	int minDistance=20000;
	//all above values are initial 
	for(int i=0; i<20000;i++)
	{
		for(int j=0;j<10;j++)
		{
			if(RTT[i][j]<minTrainColumn[j])
			{
				minTrainColumn[j] = RTT[i][j];
			}
			if(RTT[i][j]>maxTrainColumn[j])
			{
				maxTrainColumn[j] = RTT[i][j];
			}
		}
	}

	for(int i=0;i<10;i++)
	{
		distanceColumn[i] = maxTrainColumn[i] - minTrainColumn[i];
		if(distanceColumn[i]<minDistance)
			minDistance = distanceColumn[i];
	}
	//INITAL VALUES FOR TEST AND PREDICT FOR THE QUEUES RTT'S ARE THE SAME (THIS IS OUR ASSUMPTION)
	RTT_Test[0][5]=RTT_Predict[0][5] = 300; //initial values for the asiacloud queue, step = 30
	RTT_Test[0][6]=RTT_Predict[0][6] = 225; //initial values for the cloudcentar queue, step = 25
	RTT_Test[0][7]=RTT_Predict[0][7] = 120; //initial values for the rackspace queue, step = 10
	RTT_Test[0][8]=RTT_Predict[0][8] = 130; //initial values for the rsa queue, step = 10
	RTT_Test[0][9]=RTT_Predict[0][9] = 275; //initial values for the vpsnet queue, step = 15

	// K-means Start

	string kMeansLabel[1000];
	string trueTestLabel[1000];

	initializeCent2( RTT );
	assignPoints(RTT);

	for ( int i = 0; i < 1000; i++)
	{
		kMeansLabel[i] = kMeansPredict( i , RTT_Predict); // Give label according to group
		trueTestLabel[i] = true_Test_Label(RTT_Test, procesingPower, i);
		//we work in accordance to kMeans since there is where we'll send our data
		int selectedServer = findSelectedSever(kMeansLabel[i]);
		//selectedServer is from 1 to 5, thus in procesingPower[selectedServer-1]; 
		int iterationTime = findIterationTime(RTT_Test, procesingPower[selectedServer-1], selectedServer, i);
		//now finding the next and predicted queue values for the next iteration
		if(i<999)
		{
			for(int j=5; j<10;j++)
			{
				//here we have the real queue value for each server in the next iteration
				RTT_Test[i+1][j] = nextQueueValue(iterationTime, RTT_Test[i][j], procesingPower[selectedServer-1]);
				//here we have the predicted queue value for each server in the next iteration
				//RTT_Predict[i+1][j] = nextQueueValue(iterationTime, RTT_Test[i][j], procesingPower[selectedServer-1]);
				//ANOTHER APPROACH GOES HERE
				//for each iteration we build our predictions base on the initial value for the queues
				if(selectedServer ==(j-4))
					//we know the exact value of the queue_size only for the selected server for this iteration
				{	
					RTT_Predict[i+1][j] = nextQueueValue(iterationTime, RTT_Test[i][j], procesingPower[selectedServer-1]);
				}
				else
				{
					//for other unselected ones, we will continue to predict
					RTT_Predict[i+1][j] = nextQueueValue(iterationTime, RTT_Predict[i][j], procesingPower[selectedServer-1]);
				}

				if((i%REAL)==0)
				{
					RTT_Predict[i+1][j] = nextQueueValue(iterationTime, RTT_Test[i][j], procesingPower[selectedServer-1]);
				}
				//THE Approach FINISHES HERE, i.e. each REAL'th iteration we get the real queue situation from each the server

				//now normalizing the RTT_Predict values 
				//RTT_Predict_Normalized[i][j] =((double)RTT_Predict[i][j] - (double)minTrainColumn[j])*((double)minDistance/(double)distanceColumn[j]);
			}//for(int j=5; j<10;j++)
		}//if(i<999)
	}//for(int i=0;i<1000;i++)
	int sameLabels = 0;
	for(int i=0;i<1000;i++)
	{
		//cout<<kNNLabel[i]<<"\t"<<trueTestLabel[i]<<endl;			
		if(kMeansLabel[i]==trueTestLabel[i])
			sameLabels++;
		/*if((i%100)==0)
			system("pause");*/
	}

	cout<<"Number of same labels is: "<<sameLabels<<" out of 1000."<<endl;

	int kNNConfusionMatrix[5][5] = {0};

	for(int k=0; k<1000;k++)
	{
		int i=0, j=0;
		if (trueTestLabel[k]=="C1")
			i=0;
		if (trueTestLabel[k]=="C2")
			i=1;
		if (trueTestLabel[k]=="C3")
			i=2;
		if (trueTestLabel[k]=="C4")
			i=3;
		if (trueTestLabel[k]=="C5")
			i=4;

		if (kMeansLabel[k]=="C1")
			j=0;
		if (kMeansLabel[k]=="C2")
			j=1;
		if (kMeansLabel[k]=="C3")
			j=2;
		if (kMeansLabel[k]=="C4")
			j=3;
		if (kMeansLabel[k]=="C5")
			j=4;
		kNNConfusionMatrix[i][j]++;
	}

	cout<<"SAVING ALL THE DATA, ie QUEUE TESTS (REAL TEST VALUES), PREDICTS\n, TRUE AND PREDICTED LABELINGS  AS WELL AS CONFUSION MATRIX"<<endl;
	//SAVING ALL THE DATA, ie QUEUE TESTS (REAL TEST VALUES), PREDICTS, LABELINGS (TRUE AND PREDICTED) AND CONFUSION MATRIX 
	string realTestLabeling = "realTestLabeling-kNN.txt";
	ofstream output(realTestLabeling.c_str());
	for (int i = 0; i < 1000; i++)
	{
		output<<trueTestLabel[i]<<endl;
	}
	string predictedTestLabeling = "predictedTestLabeling-kNN.txt";
	ofstream output2(predictedTestLabeling.c_str());
	for (int i = 0; i < 1000; i++)
	{
		output2<<kMeansLabel[i]<<endl;
	}
	string confusionMatrix ="kNNConfusionMatrix";
	ofstream output3(confusionMatrix.c_str());
	for(int i=0;i<5;i++)
	{
		for (int j = 0; j < 5; j++)
		{
			output3<<kNNConfusionMatrix[i][j]<<"\t";
		}
		output3<<endl;
	}

	string queueOutputNames[5] = {"asiacloudQueueTest-30.txt","cloudcentarQueueTest-25.txt","rackspaceQueueTest-10.txt","rsaQueueTest-10.txt","vpsnetQueueTest-15.txt"};
	ofstream queueOutputFiles [5];
	for(int i = 0;i <5; i++)
	{
		queueOutputFiles[i].open((queueOutputNames[i].c_str()));
		for(int j=0;j<1000;j++)
		{
			queueOutputFiles[i]<<RTT_Test[j][i+5]<<endl;
		}		
	}

	string queuePredictNames[5] = {"asiacloudQueuePredict-30.txt","cloudcentarQueuePredict-25.txt","rackspaceQueuePredict-10.txt","rsaQueuePredict-10.txt","vpsnetQueuePredict-15.txt"};
	ofstream queuePredictFiles [5];
	for(int i = 0;i <5; i++)
	{
		queuePredictFiles[i].open((queuePredictNames[i].c_str()));
		for(int j=0;j<1000;j++)
		{
			queuePredictFiles[i]<<RTT_Predict[j][i+5]<<endl;
		}		
	}

	cout<<"Printing confusion matrix:"<<endl;
	for(int i=0;i<5;i++)
	{
		for (int j = 0; j < 5; j++)
		{
			cout<<kNNConfusionMatrix[i][j]<<"\t";
		}
		cout<<endl;
	}





	cout << "Program finished."<<endl;
	system ("pause");
	return 0;
}