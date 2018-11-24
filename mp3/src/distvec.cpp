#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <climits>

using namespace std;

// topo
int nodeNum=20;
int topo[21][21];	// topo[nodeNum+1][nodeNum+1]
pair<int, int> forward_table[21][21];	// pair: next_id, cost

// nodes
set<int> nodes;

// message
typedef struct message{
	int src_id;
	int dst_id;
	string info;
	message(int src_id, int dst_id, string info): src_id(src_id), dst_id(dst_id), info(info){}
}message;
vector<message> msgs;

// output file
ofstream fpOut("output.txt");

// function
void read_in_msgs(char** argv);
void read_in(ifstream &topofile);
void init_forward_table();
void build_forward_table();
void send_msg();

int main(int argc, char** argv)
{
    //printf("Number of arguments: %d", argc);
    if (argc != 4)
    {
        printf("Usage: ./distvec topofile messagefile changesfile\n");
        return -1;
    }

    // read in messages
    read_in_msgs(argv);

    // read in data
	ifstream topofile(argv[1]);
    read_in(topofile);

    // initialization
   	init_forward_table();

    // Bellman-Ford Equation
   	build_forward_table();

   	// send messages
   	send_msg();

   	int src_id, dst_id, cost;

   	// change
   	ifstream changesfile(argv[3]);
   	while(changesfile >> src_id >> dst_id >> cost)
   	{
   		topo[src_id][dst_id] = cost;
   		topo[dst_id][src_id] = cost;
   		init_forward_table();
   		build_forward_table();
   		send_msg();
   	}

   	fpOut.close();

    return 0;
}

void send_msg()
{
	int src_id, dst_id, temp_id;
	for(int i=0; i<msgs.size(); i++)
	{
		src_id = msgs[i].src_id;
		dst_id = msgs[i].dst_id;
		temp_id = src_id;
		
		fpOut<<"from "<<src_id<<" to "<<dst_id<<" cost ";
		if(forward_table[src_id][dst_id].second<0)
		{
			fpOut<<"infinite hops unreachable ";
		}
		else if(forward_table[src_id][dst_id].second==0)
		{
			fpOut<<forward_table[src_id][dst_id].second<<" hops ";
		}
		else
		{
			fpOut<<forward_table[src_id][dst_id].second<<" hops ";
			while(temp_id!=dst_id)
			{
				fpOut<<temp_id<<" ";
				temp_id = forward_table[temp_id][dst_id].first;
			}
		}
		fpOut<<"message "<<msgs[i].info<<endl;
	}
	fpOut<<endl;
}

void build_forward_table()
{
	int src_id, dst_id;
	int min_id, min_cost, next_id, cur_cost, temp_id, temp_cost;
	int nodes_num = nodes.size();


	for(int it=1; it<=nodes_num; it++)
	{
		for(set<int>::iterator i=nodes.begin(); i!=nodes.end(); i++)
		{
			src_id = *i;
			for(set<int>::iterator j=nodes.begin(); j!=nodes.end(); j++)
			{
				dst_id = *j;
				next_id = forward_table[src_id][dst_id].first;
				cur_cost = forward_table[src_id][dst_id].second;
				min_id = next_id;
				min_cost = cur_cost;
				for(set<int>::iterator k=nodes.begin(); k!=nodes.end(); k++)
				{
					temp_id = *k;
					if(topo[src_id][temp_id]>=0 && forward_table[temp_id][dst_id].second>=0)
					{
						temp_cost = topo[src_id][temp_id]+forward_table[temp_id][dst_id].second;
						if(min_cost<0 || temp_cost<min_cost )
						{
							min_id = temp_id;
							min_cost = temp_cost;
						}
					}
				}
				forward_table[src_id][dst_id] = make_pair(min_id, min_cost);
			}	
		}
	}

	for(set<int>::iterator i=nodes.begin(); i!=nodes.end(); i++)
	{
		src_id = *i;
		//cout<<"##"<<src_id<<"##"<<endl;
		for(set<int>::iterator j=nodes.begin(); j!=nodes.end(); j++)
		{
			dst_id = *j;
			fpOut<<dst_id<<" "<<forward_table[src_id][dst_id].first<<" "<<forward_table[dst_id][src_id].second<<endl;
		}
	}
}


void init_forward_table()
{
	int src_id, dst_id;

	// traversing each node as src node to init forwarding table 
	for(set<int>::iterator i=nodes.begin(); i!=nodes.end(); i++)
	{
		src_id = *i;
		for(set<int>::iterator j=nodes.begin(); j!=nodes.end(); j++)
		{
			dst_id = *j;
			if(topo[src_id][dst_id]>=0)
			{
				forward_table[src_id][dst_id] = make_pair(dst_id, topo[src_id][dst_id]);
			}
			else
			{
				forward_table[src_id][dst_id] = make_pair(-999, topo[src_id][dst_id]);
			}
		}
	}
}

void read_in(ifstream &topofile)
{
	int src_id, dst_id, cost;

    for(int i=1; i<=nodeNum; i++)
    {
    	for(int j=1; j<=nodeNum; j++)
    	{
    		topo[i][j] = -999;
    		if(i==j)
    		{
    			topo[i][j] = 0;
    		}
    	}
    }

	// initialize graph
	while(topofile >> src_id >> dst_id >> cost)
	{
		topo[src_id][dst_id] = cost;
		topo[dst_id][src_id] = cost;
		nodes.insert(src_id);
		nodes.insert(dst_id);
	}
}

void read_in_msgs(char** argv)
{
	int src_id, dst_id;

    ifstream messagefile(argv[2]);

	string line, info;
	while(getline(messagefile, line))
	{
		if(line != "")
		{
			stringstream line_ss(line);
			line_ss >> src_id >> dst_id;
			getline(line_ss, info);
			message msg(src_id, dst_id, info.substr(1));
			msgs.push_back(msg);
		}
	}
}
