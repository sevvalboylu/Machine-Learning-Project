/*
		Version 2.2, complete linkage / original distance formula.
		Sevval Boylu- 15.08.2017
*/

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include "strutils.h"
#include "randgen.h"
using namespace std;

struct node
{
	string label;
	int count;
	node *next;

	node::node()
		:label(""), count(0), next(NULL)
	{}
	node::node(string a, int b, node * n)
		:label(a),count(b),next(n)
	{}
};

//clustering function
int divCluster(int **RTT_Predict, int *clusters)
{
	//The main algorithm of divisive clustering works in this function, the rest of the code is kind of kNN code copied/ edited just a bit.
	static int count_clusters=1;

	//first we find the farthest two points, then we divide the cluster into two by adding near points to each cluster
	double min_distance=DBL_MAX; int index1=0, index2=0, m_index1, m_index2;
	double max_cons=0;
	if(count_clusters < 5)
	{
		for(int i=0; i<1000; i++)
		{
			for(int j=i+1; j<1000; j++)
			{
				if(clusters[i]==clusters[j])
				{
					long double sums=0, differences=0;
					for(int k=0; k<10;k++)
					{
						sums += ( (double)RTT_Predict[i][k] + (double)RTT_Predict[j][k]);
						differences += abs( (double)RTT_Predict[i][k] - (double)RTT_Predict[j][k]);
					}
					double distance= differences/sums;
					if(distance<min_distance)
					{
						min_distance=distance;
						index1=i; index2=j;
					}
				}
			}
			//now that we got the consecutive two points, we try to find the max distance between two consecutive points.
			if(min_distance>max_cons)
			{
				max_cons=min_distance;
				m_index1=index1; m_index2=index2;
			}
		}

		//NOW WE HAVE THE CONSECUTIVE TWO POINTS WHICH ARE THE FARTHEST. ITS TIME TO SPLIT THE CLUSTER INTO TWO.
		//ALSO WE HAVE THE INDEXES OF THESE POINTS.
		for(int m=0; m<1000; m++)
		{
			//Calculates the distance if the point belongs to the same cluster
			if(clusters[m]== clusters[index1] && clusters[m]== clusters[index2])
			{
				long double dist1=0, dist2=0; 
				long double sum1=0,sum2=0, diff1=0,diff2=0;
				for(int k=0; k<10;k++)
				{
					sum1+= (RTT_Predict[m][k]+RTT_Predict[index1][k]);
					diff1+= abs(RTT_Predict[m][k]-RTT_Predict[index1][k]);

					sum2+= (RTT_Predict[m][k]+RTT_Predict[index2][k]);
					diff2+= abs(RTT_Predict[m][k]-RTT_Predict[index2][k]);
				}
			
				dist1=diff1/sum1;
				dist2=diff2/sum2;

				if(dist1<dist2)
				{
					clusters[m]=count_clusters+1;
				}
			}
		}
		count_clusters++;
		divCluster(RTT_Predict, clusters);
	}
	return *clusters;
}


//labelling function
string label(int **RTT_Predict, int *clusters, int *procPow, string * labels)
{
	node *rankings = new node[5];

	//string *labels=new string[1000];
	for(int i=0; i<1000; i++)
	{
		int min_time=10000, minIndex=6;
		for(int k=0; k<5; k++)
		{
			int time= RTT_Predict[i][k]+ RTT_Predict[i][k+5]+ procPow[k];
			if(time<min_time)
			{
				min_time=time;
				minIndex=k;
			}
		}
		labels[i]= "C"+itoa(minIndex+1);
	}

	string predictions[5];
	
	for(int j=1; j<6; j++)
	{
		int count[5]={0};
		for(int i=0; i<1000;i++)
		{
			if(clusters[i]==j)
			{
				if(labels[i]=="C1")
					count[0]++;
				if(labels[i]=="C2")
					count[1]++;
				if(labels[i]=="C3")
					count[2]++;
				if(labels[i]=="C4")
					count[3]++;
				if(labels[i]=="C5")
					count[4]++;
			}
		}
		
		node *head;
		head=new node("C1",count[0],NULL);

		//this for loop is for ranking the counts of the servers in one cluster
		for(int i=1; i<5; i++)
		{
			node *ptr, *ptr2;
			if(count[i] >= head->count)
			{
				ptr=head;
				head=new node("C"+itoa(i+1),count[i],ptr);
			}
			else
			{
				if(i>1)
				{
					ptr=head; ptr2=head->next;
					while(ptr2->next!=NULL && ptr2->count > count[i])
					{
						ptr=ptr->next;
						ptr2=ptr2->next;
					}

					ptr->next=new node("C"+itoa(i+1), count[i],ptr2);
				}
				else
				{
					head->next=new node("C"+itoa(i+1), count[i], NULL);
				}
			}
		}
		
		rankings[j-1]= *head;
		string max_label=head->label;
		if(j>1)
		{
			for(int i=0; i<j-1; i++)
			{
				if(max_label== predictions[i])
				{
					head=head->next;
					max_label=head->label;
					i=-1; //to reset the loop, since we need to check again.
				}
			}
		}

		//register the max_label and its count to the predictions matrix in order to make changes afterwards
		predictions[j-1]=max_label;
		cout << j<< " "<< max_label<< endl;
		//now we have the label of our clusters, we will change the points' labels to that label
		for(int k=0; k<1000; k++)
		{
			if(clusters[k]==j)
			{
				labels[k]=max_label;
			}
		}
	}
	return *labels;
}

//true labels function(which I used directly)
string true_Test_Label(int ** RTT_Test, int *procesingPower, int i)
{
	int minIterValue=INT_MAX; int minIndex=0;
	for(int j=0 ; j<5; j++)
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
	int procesingPower[] = {30, 25, 10, 10, 15};// relative proc. power of cload server taken from official web sites and using benchmark tests.
	string data;
	//First read the data
	/*
	int **RTT= new int* [20000]; //memory for training data
	for (int i = 0; i < 20000; i++)
	{
		RTT[i] = new int[10];
	}
	string filename[] = {"www.asiacloudserver.com.txt","www.cloudcentral.com.au.txt","www.rackspace.com.txt","www.rsaweb.co.za.txt","www.vps.net.txt",
		                 "asiacloudQueueTrain-30.txt" ,"cloudcentarQueueTrain-25.txt","rackspaceQueueTrain-10.txt","rsaQueueTrain-10.txt","vpsnetQueueTrain-15.txt"};
	ifstream file[10];
	
	for(int i=0; i<10; i++)
	{
		int count=0;
		cout<<"Reading data from: "<< filename[i]<<endl;
		file[i].open(filename[i].c_str());
		while(file[i]>>data)
		{
			RTT[count][i]=atoi(data);
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
	*/
	//Reading RTT test values from txt files.
	int **RTT_Test= new int *[1000];
	for(int i=0; i<1000;i++)
	{
		RTT_Test[i]=new int[10];
	}
	
	string filename_Test[]={"www.asiacloudserver.comTest.txt","www.cloudcentral.com.auTest.txt","www.rackspace.comTest.txt","www.rsaweb.co.zaTest.txt","www.vps.netTest.txt"};
	ifstream fileTest[5];
	for(int i=0; i<5; i++)
	{
		cout<< "Reading data from: " << filename_Test[i]<< endl;
		int count=0;
		fileTest[i].open(filename_Test[i].c_str());
		while(fileTest[i]>>data)
		{
			RTT_Test[count][i]=atoi(data);
			count++;
		}
		cout<<"Total "<<count<<	" data read." <<endl;
	}
	
	//Reading RTT predict values again from txt files.
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

	
	//INITAL VALUES FOR TEST AND PREDICT FOR THE QUEUES RTT'S ARE THE SAME (THIS IS OUR ASSUMPTION)
	RTT_Test[0][5]=RTT_Predict[0][5] = 300; //initial values for the asiacloud queue, step = 30
	RTT_Test[0][6]=RTT_Predict[0][6] = 225; //initial values for the cloudcentar queue, step = 25
	RTT_Test[0][7]=RTT_Predict[0][7] = 120; //initial values for the rackspace queue, step = 10
	RTT_Test[0][8]=RTT_Predict[0][8] = 130; //initial values for the rsa queue, step = 10
	RTT_Test[0][9]=RTT_Predict[0][9] = 275; //initial values for the vpsnet queue, step = 15


	/*		
		MY CODE HERE -Sevval
	*/

	int clusters[1000];
	string trueTestLabel[1000];
	string div_label[1000];

	//initial cluster is same for all of them i.e We start with a big one cluster
	for(int h=0; h<1000; h++)
	{
		clusters[h]=1;
	}
	
	//QUEUE VALUES SHOULD BE FILLED BEFORE THE CLUSTERING FUNCTION CALL, SINCE WE USE THEM FOR CLUSTERING.
	
	// I just use this part of code in order to get the queue values for the Test(TRUE) values
	for(int i=0; i<1000; i++)
	{
		trueTestLabel[i]=true_Test_Label(RTT_Test, procesingPower,i);
		int selectedServer= findSelectedSever(trueTestLabel[i]);
		
		/// Iteration time comes out as a dummy number
		
		int iterationTime= RTT_Test[i][selectedServer]+ RTT_Test[i][selectedServer+5]+procesingPower[selectedServer-1];

		if(i<999)
		{
			for(int j=5; j<10; j++)
			{
				//here we have the real queue value for each server in the next iteration
				RTT_Test[i+1][j] = nextQueueValue(iterationTime, RTT_Test[i][j], procesingPower[selectedServer-1]);
			}
		}
	}

	for(int i=1; i<1000; i++)
	{
		//here we have the real queue value for each server in the next iteration
		RandGen random;
		int min_time=INT_MAX, minindex;
		//find the minimum time in order to use it for predicting the queue values.
		for(int j=0; j<5; j++)
		{
			int time= RTT_Predict[i-1][j]+ RTT_Predict[i-1][j+5]+procesingPower[j];
			if(time< min_time)
			{
				min_time=time;
				minindex=j;
			}
		}
		int step=procesingPower[minindex];
		int iterations=min_time/step;

		for(int h=5; h<10;h++)
		{
			int currentQ=RTT_Predict[i-1][h];
			for(int k=0; k<iterations; k++)
			{
				int number=random.RandInt(-1,1);
				currentQ+= number*step;
				if(currentQ<0)
					currentQ=0;			
			}

			RTT_Predict[i][h]=currentQ;
		}
		if(i%5==0)
		{
			for(int g=5;g<10;g++)
			{
				RTT_Predict[i][g]=RTT_Test[i][g];
			}
		}

	}
	
	//clustering function call
	divCluster(RTT_Predict, clusters);
	label(RTT_Predict,clusters,procesingPower,div_label);

	
	//
	// From this point on its the same as kNN code.
	//
	int sameLabels=0;
	for(int i=0; i<1000; i++)
	{
		//cout<< div_label[i] <<"\t" << trueTestLabel[i] << endl;
		if(div_label[i]== trueTestLabel[i])
			sameLabels++;
	}

	cout<< "Number of same labels is "<< sameLabels << " out of 1000 labels."<<endl;
	
	int divCluster_ConfMatrix[5][5] = {0};

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

		if (div_label[k]=="C1")
			j=0;
		if (div_label[k]=="C2")
			j=1;
		if (div_label[k]=="C3")
			j=2;
		if (div_label[k]=="C4")
			j=3;
		if (div_label[k]=="C5")
			j=4;
		divCluster_ConfMatrix[i][j]++;
	}

	cout<<"SAVING ALL THE DATA, ie QUEUE TESTS (REAL TEST VALUES), PREDICTS\n, TRUE AND PREDICTED LABELINGS  AS WELL AS CONFUSION MATRIX"<<endl;
	//SAVING ALL THE DATA, ie QUEUE TESTS (REAL TEST VALUES), PREDICTS, LABELINGS (TRUE AND PREDICTED) AND CONFUSION MATRIX 
	string realTestLabeling = "realTestLabeling-kNN.txt";
	ofstream output(realTestLabeling.c_str());
	for (int i = 0; i < 1000; i++)
	{
		output<<trueTestLabel[i]<<endl;
	}
	string predictedTestLabeling = "predictedTestLabeling-div_Clustering.txt";
	ofstream output2(predictedTestLabeling.c_str());
	for (int i = 0; i < 1000; i++)
	{
		output2<<div_label[i]<<endl;
	}
	string confusionMatrix ="divisiveClusteringConfusionMatrix.txt";
	ofstream output3(confusionMatrix.c_str());
	for(int i=0;i<5;i++)
	{
		for (int j = 0; j < 5; j++)
		{
			output3<<divCluster_ConfMatrix[i][j]<<"\t";
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
			cout<<divCluster_ConfMatrix[i][j]<<"\t";
		}
		cout<<endl;
	}
	
	cout << "Program finished."<<endl;
	cin.ignore();
	cin.get();
	return 0;
}
