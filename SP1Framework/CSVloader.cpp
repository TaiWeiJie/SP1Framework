#include "CSVloader.h"
#include "game.h"
#include "Framework\console.h"
//#include "Map.h"
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <sstream>
//
//void LoadFromCSV(string filename, Map& map)
//{
//	string map1 = filename;
//
//	int x = 0;
//	int y = 0;
//	//unsigned short data[80][25];
//	string line;
//	ifstream myfile(map1);
//	if (myfile.is_open())
//	{
//		getline(myfile, line);
//		while (getline(myfile, line))
//		{
//			stringstream ss(line);
//			for (int c; ss >> c;)
//			{
//				if (ss.peek() == ',')
//					ss.ignore();
//
//
//				map.mapArray[x][y] = c;
//				x++;
//			}
//			if (x < 80)
//			{
//				x++;
//			}
//			else if (x >= 80)
//				x = 0;
//			y += 1;
//		}
//		myfile.close();
//
//	}
//}
