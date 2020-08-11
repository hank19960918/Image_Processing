#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <queue>
#include <tuple>
#include<stack>
using namespace cv;
using namespace std;

struct NodePoint
{
	int r, c;//也可以在Node中加一個int pre屬性,然後做一個全域性的nodes,就不用pre[][]陣列了.
	NodePoint(int r, int c) :r(r), c(c) {}
	NodePoint() {}
}pre[20][20];
queue<NodePoint> Q;

void readImage(unsigned char *image, string path, int height, int width)
{
	const char *s = path.c_str();
	FILE *f = fopen(s, "rb");
	fread(image, sizeof(unsigned char), height * width, f);
	fclose(f);
}

void showImage(Mat imageName, string name, unsigned char *image)
{
	namedWindow(name, WINDOW_AUTOSIZE);
	imshow(name, imageName);
	delete[] image;
	waitKey(0);
}

void saveImage(unsigned char *image, string name, int height, int width)
{
	const char *s = name.c_str();
	FILE *f = fopen(s, "wb");
	fwrite(image, sizeof(unsigned char), height * width, f);
	fclose(f);
}

int findMaze(int dr[], int dc[], int vis[20][20], int dist[20][20], unsigned char testImage[20][20], unsigned char outputImage[20][20], int pixelValue1, int pixelValue2, int pixelValue3, int findWay)
{
	while (!Q.empty())
	{
		NodePoint node = Q.front(); Q.pop();
		int r = node.r, c = node.c;
		for (int d = 0; d < findWay; d++)
		{
			int nr = r + dr[d];
			int nc = c + dc[d];
			if ((nr >= 0 && nr < 20 && nc >= 0 && nc < 20 && vis[nr][nc] == 0 && testImage[nr][nc] == pixelValue1) || (nr >= 0 && nr < 20 && nc >= 0 && nc < 20 && vis[nr][nc] == 0 && testImage[nr][nc] == pixelValue2) || (nr >= 0 && nr < 20 && nc >= 0 && nc < 20 && vis[nr][nc] == 0 && testImage[nr][nc] == pixelValue3))
			{
				vis[nr][nc] = 1;
				Q.push(NodePoint(nr, nc));
				dist[nr][nc] = 1 + dist[r][c];
				pre[nr][nc] = NodePoint(r, c);
				if (nr == 19 && nc == 19) break;
			}
		}
	}
	stack<NodePoint> S;
	int cur_r = 19, cur_c = 19, count = 0;
	while (true)  //�o�̤]�i�H���}�@�ӼƲհO���C
	{
		S.push(NodePoint(cur_r, cur_c));
		if (cur_r == 0 && cur_c == 0) break;
		int r = cur_r, c = cur_c;
		cur_r = pre[r][c].r;
		cur_c = pre[r][c].c;
	}
	while (!S.empty())
	{
		NodePoint node = S.top(); S.pop();
		outputImage[node.r][node.c] = 255;
		count++;
	}
	return count;
}



int findMpath(int dr[], int dc[], int vis[20][20], int dist[20][20], unsigned char testImage[20][20], unsigned char outputImage[20][20], int pixelValue1, int pixelValue2, int pixelValue3)
{
	int xNode = 0, yNode = 0;
	int flag = 0;
	while (!Q.empty())
	{
		NodePoint node = Q.front(); Q.pop();
		int r = node.r, c = node.c;
		for (int d = 0; d < 4; d++)
		{
			int nr = r + dr[d];
			int nc = c + dc[d];
			if ((nr >= 0 && nr < 20 && nc >= 0 && nc < 20 && vis[nr][nc] == 0 && testImage[nr][nc] == pixelValue1) || (nr >= 0 && nr < 20 && nc >= 0 && nc < 20 && vis[nr][nc] == 0 && testImage[nr][nc] == pixelValue2) || (nr >= 0 && nr < 20 && nc >= 0 && nc < 20 && vis[nr][nc] == 0 && testImage[nr][nc] == pixelValue3))
			{
				vis[nr][nc] = 1;
				Q.push(NodePoint(nr, nc));
				dist[nr][nc] = 1 + dist[r][c];
				pre[nr][nc] = NodePoint(r, c);
				if (nr == 19 && nc == 19) break;
			}
		}
		if (flag == 1)
		{
			for (int d = 4; d < 8; d++)
			{
				int nr = xNode + dr[d];
				int nc = yNode + dc[d];
				if ((nr >= 0 && nr < 20 && nc >= 0 && nc < 20 && vis[nr][nc] == 0 && testImage[nr][nc] == 85))
				{
					vis[nr][nc] = 1;
					Q.push(NodePoint(nr, nc));
					dist[nr][nc] = 2 + dist[xNode][yNode];
					pre[nr][nc] = NodePoint(xNode, yNode);
					if (nr == 19 && nc == 19) break;
				}
			}
		}
		flag = 1;
		xNode = r, yNode = c;
	}
	stack<NodePoint> Try;
	int current_r = 19, current_c = 19, currentNumber = dist[current_r][current_c];
	cout << currentNumber << endl;
	int next_r = 0, next_c = 0;
	Try.push(NodePoint(current_r, current_c));
	while (true)
	{
		cout << "current_r: " << current_r << " " << "current_c: " << current_c << "　" << currentNumber << endl;;
		if ((current_r == 0 && current_c == 0) || currentNumber == 0) break;

		if (dist[current_r + 1][current_c] == currentNumber - 1)
		{
			next_r = current_r + 1, next_c = current_c;
			currentNumber = currentNumber - 1;
		}
		else if (dist[current_r - 1][current_c] == currentNumber - 1)
		{
			next_r = current_r - 1, next_c = current_c;
			currentNumber = currentNumber - 1;
		}
		else if (dist[current_r][current_c + 1] == currentNumber - 1)
		{
			next_r = current_r, next_c = current_c + 1;
			currentNumber = currentNumber - 1;
		}
		else if (dist[current_r][current_c - 1] == currentNumber - 1)
		{
			next_r = current_r, next_c = current_c - 1;
			currentNumber = currentNumber - 1;
		}

		else if (dist[current_r - 1][current_c + 1] == currentNumber - 2)
		{
			next_r = current_r - 1, next_c = current_c + 1;
			currentNumber = currentNumber - 2;
		}
		else if (dist[current_r + 1][current_c + 1] == currentNumber - 2)
		{
			next_r = current_r + 1, next_c = current_c + 1;
			currentNumber = currentNumber - 2;
		}
		else if (dist[current_r - 1][current_c - 1] == currentNumber - 2)
		{
			next_r = current_r - 1, next_c = current_c - 1;
			currentNumber = currentNumber - 2;
		}
		else if (dist[current_r + 1][current_c - 1] == currentNumber - 2)
		{
			next_r = current_r + 1, next_c = current_c - 1;
			currentNumber = currentNumber - 2;
		}


		current_r = next_r, current_c = next_c;
		Try.push(NodePoint(next_r, next_c));
	}
	int count = 0;
	while (!Try.empty())
	{
		NodePoint node = Try.top(); Try.pop();
		outputImage[node.r][node.c] = 255;
		count++;
	}
	return count;
}


int main()
{
	while (true)
	{
		cout << "enter 1 to show the shorest path in pixel 85" << endl;
		cout << "enter 2 to show the shorest path in pixel 85 & 175" << endl;
		cout << "enter 3 to show the shorest path in pixel 85 & 175 & 255" << endl;
		int choose = 0;
		cin >> choose;
		int height = 20, width = 20, count = 0;
		int vis[20][20], dist[20][20];

		int dr[] = { -1,1,0,0, -1, 1, 1, -1 }, dc[] = { 0,0,-1,1, 1, 1, -1, -1 };
		unsigned char testImage[20][20];
		unsigned char outputImage[20][20] = { 0 };
		FILE *f = fopen("maze_20.raw", "rb");
		fread(testImage, sizeof(unsigned char), height * width, f);
		fread(outputImage, sizeof(unsigned char), height * width, f);
		Mat Maze(height, width, CV_8U, testImage);
		fclose(f);
		while (!Q.empty()) Q.pop();
		memset(vis, 0, sizeof(vis));
		dist[0][0] = 0;
		vis[0][0] = 1;
		Q.push(NodePoint(0, 0));
		if (choose == 1)
		{
			cout << "enter 1 to show the path in 4 path" << endl;
			cout << "enter 2 to show the path in 8 path" << endl;
			cout << "enter 3 to show the path in M path" << endl;
			cin >> choose;
			if (choose == 1)
			{
				int count = 0;
				count = findMaze(dr, dc, vis, dist, testImage, outputImage, 85, 85, 85, 4);
				Mat Maze85(20, 20, CV_8U, outputImage);
				imwrite("Maze85FourPath.png", Maze85);
				cout << "the step in pixel 85 is : " << count << " " << "steps";
			}
			else if (choose == 2)
			{
				int count = 0;
				count = findMaze(dr, dc, vis, dist, testImage, outputImage, 85, 85, 85, 8);
				Mat Maze85(20, 20, CV_8U, outputImage);
				imwrite("Maze85EightPath.png", Maze85);
				cout << "the step in pixel 85 is : " << count << " " << "steps";
			}
			else if (choose == 3)
			{
				int count = 0;
				count = findMpath(dr, dc, vis, dist, testImage, outputImage, 85, 85, 85);
				Mat Maze85(20, 20, CV_8U, outputImage);
				imwrite("Maze85MPath.png", Maze85);
				cout << "the step in pixel 85  is : " << count << " " << "steps";
			}
		}
		else if (choose == 2)
		{
			cout << "enter 1 to show the path in 4 path" << endl;
			cout << "enter 2 to show the path in 8 path" << endl;
			cout << "enter 3 to show the path in M path" << endl;
			cin >> choose;
			if (choose == 1)
			{
				int count = 0;
				count = findMaze(dr, dc, vis, dist, testImage, outputImage, 85, 170, 85, 4);
				Mat Maze85175(20, 20, CV_8U, outputImage);
				imwrite("Maze175FourPath.png", Maze85175);
				cout << "the step in pixel 85 & 175 is : " << count << " " << "steps";
			}
			else if (choose == 2)
			{

				int count = 0;
				count = findMaze(dr, dc, vis, dist, testImage, outputImage, 85, 170, 85, 8);
				Mat Maze85(20, 20, CV_8U, outputImage);
				imwrite("Maze85175EightPath.png", Maze85);
				cout << "the step in pixel 85 & 170 is : " << count << " " << "steps";
			}
			else if (choose == 3)
			{
				int count = 0;
				count = findMpath(dr, dc, vis, dist, testImage, outputImage, 85, 170, 85);
				Mat Maze85(20, 20, CV_8U, outputImage);
				imwrite("Maze85175MPath.png", Maze85);
				cout << "the step in pixel 85 & 170 is : " << count << " " << "steps";
			}
		}
		else if (choose == 3)
		{
			cout << "enter 1 to show the path in 4 path" << endl;
			cout << "enter 2 to show the path in 8 path" << endl;
			cout << "enter 3 to show the path in M path" << endl;
			cin >> choose;
			if (choose == 1)
			{
				int count = 0;
				count = findMaze(dr, dc, vis, dist, testImage, outputImage, 85, 170, 255, 4);
				Mat Maze85175255(20, 20, CV_8U, outputImage);
				imwrite("Maze255FourPath.png", Maze85175255);
				cout << "the step in pixel 85 & 170 & 255 is : " << count << " " << "steps";
			}
			else if (choose == 2)
			{
				int count = 0;
				count = findMaze(dr, dc, vis, dist, testImage, outputImage, 85, 170, 255, 8);
				Mat Maze85175255(20, 20, CV_8U, outputImage);
				imwrite("Maze255EightPath.png", Maze85175255);
				cout << "the step in pixel 85 & 170 & 255 is : " << count << " " << "steps";
			}
			else if (choose == 3)
			{
				int count = 0;
				count = findMpath(dr, dc, vis, dist, testImage, outputImage, 85, 170, 255);
				Mat Maze85(20, 20, CV_8U, outputImage);
				imwrite("Maze85175255MPath.png", Maze85);
				cout << "the step in pixel 85 & 170 is : " << count << " " << "steps";
			}
		}
	}
	system("pause");

	return 0;
}