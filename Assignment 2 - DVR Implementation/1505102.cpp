#include<bits/stdc++.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

#define INF 99999
//#define NULL 0

struct Node
{
public:
	string destination;
	string nextHop;
	int cost;
};

vector <Node> routingTable;
vector <string> neighbour;
string myIP;

bool findIP(string ip)
{
	for(int i = 0; i < routingTable.size(); i++)
	{
		if(routingTable[i].destination == ip) return true;
	}

	return false;
}

int findPlace(string ip)
{
	for(int i = 0; i < routingTable.size(); i++)
	{
		if(routingTable[i].destination == ip) return i;
	}
}

bool findNeighbour(string ip)
{
	for(int i = 0; i < neighbour.size(); i++)
	{
		if(neighbour[i] == ip) return true;
	}

	return false;
}

void printRoutingTable()
{
	cout << "Destination\tNextHop\tCost" << endl;

	for(int i = 0; i < routingTable.size(); i++)
	{
		cout << routingTable[i].destination << "\t" << routingTable[i].nextHop << "\t" << routingTable[i].cost << endl;
	}
}

string tostr()
{
	string str = "from " + myIP;

	for(int i = 0; i < routingTable.size(); i++)
	{
		str += " " + routingTable[i].destination;
		str += " " + routingTable[i].nextHop;
		str += " " + to_string(routingTable[i].cost);
	}

	return str;
}

void myRoute(string topoFile, string myIP)
{
	string ip1, ip2, line;
	int c, n;
	Node temp;

	ifstream file(topoFile);

	//cout << "myIP: " << myIP << endl;

	while(file >> ip1 >> ip2 >> c)
	{
		if(ip1 == myIP)
		{
			if(findIP(ip2)) //if already in routing table
			{
				n = findPlace(ip2); //where in routing table

				if(c < routingTable[n].cost)
				{
					if(!findNeighbour(ip2)) neighbour.push_back(ip2); //pushing in neighbour
					routingTable[n].cost = c; 
					routingTable[n].nextHop = ip2;
				}
			}

			else
			{
				if(!findNeighbour(ip2)) neighbour.push_back(ip2); //pushing in neighbour
				temp.destination = ip2;
				temp.nextHop = ip2;
				temp.cost = c;
				routingTable.push_back(temp); 
			}
		}

		else if(ip2 == myIP)
		{
			if(findIP(ip1)) //if already in routing table
			{
				n = findPlace(ip1); //where in routing table

				if(c < routingTable[n].cost)
				{
					if(!findNeighbour(ip1)) neighbour.push_back(ip1); //pushing in neighbour
					routingTable[n].cost = c; 
					routingTable[n].nextHop = ip1;
				}
			}

			else
			{
				if(!findNeighbour(ip1)) neighbour.push_back(ip1); //pushing in neighbour
				temp.destination = ip1;
				temp.nextHop = ip1;
				temp.cost = c;
				routingTable.push_back(temp); 
			}
		}

		else
		{
			if(!findIP(ip1))
			{
				temp.destination = ip1;
				temp.nextHop = "0";
				temp.cost = INF;

				routingTable.push_back(temp);
			}

			if(!findIP(ip2))
			{
				temp.destination = ip2;
				temp.nextHop = "0";
				temp.cost = INF;

				routingTable.push_back(temp);
			}
		}
	}

	printRoutingTable();
}

void sendNeighbour()
{
	string str = tostr();

	int sockfd;
	int bytes_received;
    struct sockaddr_in router_address;
    router_address.sin_family = AF_INET;
    router_address.sin_port = htons(4747);
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    for(int i = 0; i < neighbour.size(); i++)
    {
	    inet_pton(AF_INET,neighbour[i].c_str(),&router_address.sin_addr);
	    sendto(sockfd, str.c_str(), 1024, 0, (struct sockaddr*) &router_address, sizeof(sockaddr_in));
	}
}

void receive()
{
	char buffer[1024];
	string str = "";

	int sockfd;
	int bytes_received;
	socklen_t addrlen;
	struct sockaddr_in client_address;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	client_address.sin_family = AF_INET;
	client_address.sin_port = htons(4747);
	client_address.sin_addr.s_addr = inet_addr(myIP.c_str());

	while(true)
	{
		bytes_received = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &client_address, &addrlen);
		str = buffer;
	}
}

int main(int argc, char *argv[])
{
	if(argc != 3){
		printf("\nargument error\n");
		exit(1);
	}
	myIP = argv[1];
	string topoFile = argv[2];
	
	myRoute(topoFile, myIP);
	//cout << "check" << endl;
	return 0;
}