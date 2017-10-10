#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include "strutils.h"
#include "randgen.h"
using namespace std;

//burayi galiba direk kullanacağım aynisini
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

string kNN_Label(int ** RTT_Train, int **RTT_Predict, int i, string * trainLabel)
//finds 1-NN neighbour and returns its label, C1 to C5
{
	long double minDistance=100000.0; //we could equate it to MAX_INT too.
	string label="C1";
	double currentDistance;
	//using euclidian distance
	for(int k =0; k<20000;k++)
	{
		long double featureSum=0;
		long double featureSubtract=0;
		for(int j= 0;j<10;j++)
		{
			featureSum += ((double)RTT_Train[k][j]+(double)RTT_Predict[i][j]);		//???????????????????????????
			featureSubtract += abs((double)RTT_Train[k][j]-(double)RTT_Predict[i][j]);
		}
		currentDistance = featureSubtract/featureSum;
		if(currentDistance<minDistance)
		{
			minDistance = currentDistance;
			label = trainLabel[k];
		}

	}	
	return label;
}

//pertty clear to me
string true_Test_Label(int ** RTT_Test, int *procesingPower, int i)
//returns the real value label, i.e. the one with minimum RTT+queue+processing for this iteration, C1 to C5
{
	int minIterValue = 6000; int minIndex = 0;
	for (int j = 0; j<5; j++)
	{
		int currServer = RTT_Test[i][j]+RTT_Test[i][j+5]+procesingPower[j];
		//her RTT'yi aynı serverin queue value'sıyla topla/processing power ekle.
		if(currServer<minIterValue)
		{
			minIterValue = currServer;
			minIndex = j;
		}
	}
	return "C"+itoa(minIndex+1);//C1 to C5
}

int findIterationTime(int **RTT_Test, int serverProcPower, int j_SelectedServer, int i)
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

int main()
{
	int procesingPower [] = {30, 25, 10, 10, 15};// relative proc. power of cload server taken from official web sites and using benchmark tests.
	
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

	//READING TRAIN LABELINGS, STORED IN trainLabeling[20000] VECTOR
	string trainLabeling[20000];
	string labelingFileName = "trainLabeling.txt";
	ifstream trainLabelingfile;
	trainLabelingfile.open(labelingFileName.c_str());
	int count = 0;
	cout<<endl<<"Reading data from: "<<labelingFileName<<endl;
	while(trainLabelingfile>>data)
	{
		trainLabeling[count] = data;
		count++;
	}

	cout << "Total "<<count<<" data read."<<endl;

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
	int minTrainColumn[10]={1000,1000,1000,1000,1000,1000,1000,1000,1000,1000};
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
	///
	//  ??????????????????????
	///
	for(int i=0;i<10;i++)
	{
		distanceColumn[i] = maxTrainColumn[i] - minTrainColumn[i];
		if(distanceColumn[i]<minDistance)
			minDistance = distanceColumn[i];
	}
	/*cout<<"Printing max, min, dist. and minDist values"<<endl;
	for (int i = 0; i < 10; i++)
	{
		cout<<"min["<<i<<"]="<<minTrainColumn[i]<<"\tmax["<<i<<"]="<<maxTrainColumn[i]<<"\tdist["<<i<<"]="<<distanceColumn[i]<<endl;
	}
	cout<<"Minum from all the columns is: "<<minDistance<<endl;*/
	
	/*****************************NORMALIZATION STARTS HERE*****************************************************
	
	//NORMALIZING TRAIN VALUES
	double **RTT_Train_Normalized = new double*[20000];
	for (int i = 0; i < 20000; i++)
	{
		RTT_Train_Normalized[i] = new double[10];
	}
	for (int i = 0; i < 20000; i++)
	{
		for(int j=0;j<10;j++)//till 10
			RTT_Train_Normalized[i][j] =((double)RTT[i][j] - (double)minTrainColumn[j])*((double)minDistance/(double)distanceColumn[j]); 
	}
	/*cout<<"Printing normalized values:"<<endl;
	system("pause");

	for (int i = 0; i < 20000; i++)
	{
		if((i%500)==0)
			system("pause");
		for(int j=0;j<10;j++)
		{
			cout<<RTT_Train_Normalized[i][j]<<"\t";

		}
		cout<<endl;

	}

	//NORMALIZING PREDICT VALUES
	double **RTT_Predict_Normalized = new double*[1000];
	for (int i = 0; i < 1000; i++)
	{
		RTT_Predict_Normalized[i] = new double[10];
	}
	for (int i = 0; i < 1000; i++)
	{
		for(int j=0;j<5;j++)//till 5
			RTT_Predict_Normalized[i][j] =((double)RTT_Predict[i][j] - (double)minTrainColumn[j])*((double)minDistance/(double)distanceColumn[j]); 
	}
	******************************NORMALIZATION FINISHES HERE************************************************************/
	
	//INITAL VALUES FOR TEST AND PREDICT FOR THE QUEUES RTT'S ARE THE SAME (THIS IS OUR ASSUMPTION)
	RTT_Test[0][5]=RTT_Predict[0][5] = 300; //initial values for the asiacloud queue, step = 30
	RTT_Test[0][6]=RTT_Predict[0][6] = 225; //initial values for the cloudcentar queue, step = 25
	RTT_Test[0][7]=RTT_Predict[0][7] = 120; //initial values for the rackspace queue, step = 10
	RTT_Test[0][8]=RTT_Predict[0][8] = 130; //initial values for the rsa queue, step = 10
	RTT_Test[0][9]=RTT_Predict[0][9] = 275; //initial values for the vpsnet queue, step = 15
	
	//normalizing those 5 queue values for the predicted set
	//for(int j= 5; j<10;j++)
	//{	
	//	RTT_Predict_Normalized[0][j] =((double)RTT_Predict[0][j] - (double)minTrainColumn[j])*((double)minDistance/(double)distanceColumn[j]);
	//}
	//FINALLY, FINDIG THE TRUE AND kNN PREDICTED LABELS (CLASSES-SERVERS) FOR EACH ITERATION
	string kNNLabel[1000];
	string trueTestLabel[1000];
	
	for(int i=0;i<1000;i++)
	{
		kNNLabel[i] = kNN_Label(RTT, RTT_Predict, i, trainLabeling);
		trueTestLabel[i] = true_Test_Label(RTT_Test, procesingPower, i);
		//we work in accordance to kNN since there is where we'll send our data
		int selectedServer = findSelectedSever(kNNLabel[i]);
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

				if((i%5)==0)
				{
					RTT_Predict[i+1][j] = nextQueueValue(iterationTime, RTT_Test[i][j], procesingPower[selectedServer-1]);
				}
				//THE Approach FINISHES HERE, i.e. each 5th iteration we get the real queue situation from each the server

				//now normalizing the RTT_Predict values 
				//RTT_Predict_Normalized[i][j] =((double)RTT_Predict[i][j] - (double)minTrainColumn[j])*((double)minDistance/(double)distanceColumn[j]);
			}//for(int j=5; j<10;j++)
		}//if(i<999)
	}//for(int i=0;i<1000;i++)

	/*
		kinda will copy this code below as it is if Im allowed.
	*/

	/*cout<<"Printing labels: "<<endl;
	system("pause");*/
	int sameLabels = 0;
	for(int i=0;i<1000;i++)
	{
		//cout<<kNNLabel[i]<<"\t"<<trueTestLabel[i]<<endl;			
		if(kNNLabel[i]==trueTestLabel[i])
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

		if (kNNLabel[k]=="C1")
			j=0;
		if (kNNLabel[k]=="C2")
			j=1;
		if (kNNLabel[k]=="C3")
			j=2;
		if (kNNLabel[k]=="C4")
			j=3;
		if (kNNLabel[k]=="C5")
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
		output2<<kNNLabel[i]<<endl;
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
