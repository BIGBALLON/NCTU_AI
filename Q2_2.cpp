#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <ctime>
#include <queue>
#include <set>
using namespace std;

#define SEQ_LEN         1000
#define PAT_LEN         15
#define SEQ_CNT         50
#define MAX_MUTATION    5
#define MAX_SCORE       1000
/***********************************************/
vector<string> sequence;
vector<string> pattern;
vector<pair<string,int> > pat_que;
/***********************************************/

set<string> hash_table;

string best_pattern;

char itoc[] = { 'T', 'C', 'G', 'A' };  
void output_ans();
void get_all_pattern();
void load_sequence();
inline string random_select();
int isequal(string s1, string s2, int n);
int search_onece(string pat);
void gen_queue();

bool cmp(const pair<string,int> a, const pair<string ,int> b) { return a.second < b.second; }


int main(){

	srand (time(NULL));
    freopen( "q2.data", "r", stdin );
	//freopen( "./ex_datasets/ex2_5_mutates.data", "r", stdin );
	
    int start = clock();

	load_sequence();
	get_all_pattern();
	gen_queue();

    while( true ){
    	for( int i = 0; i < MAX_MUTATION; ++i ){
    		int ran_score = 0; 
    		string ran = random_select();
	        if( hash_table.find(ran) != hash_table.end() ){
	        	continue;
			}else{
				hash_table.insert(ran);
			}
	        ran_score = search_onece(ran);
			if( ran_score < pat_que[500].second ){
				if( pat_que.size() >= 2000  ) pat_que.pop_back();
				pat_que.push_back( make_pair(ran,ran_score) );
			}
			if( iteration % 10 == 0 ){
				sort(pat_que.begin(), pat_que.end(),cmp);
			}
			if( iteration % 50 == 0 ){
				printf( "%d %d %d\r", iteration, pat_que[0].second, hash_table.size() );
			}
			iteration += 1;
		}
    }

	int end = clock();
	double used_time = (double)(end - start) /  CLOCKS_PER_SEC;
	printf( "used_time: %f", used_time );


	return 0;
}

void gen_queue(){
	int start = clock();
	for( int i = 0; i < pattern.size(); ++i ){
		int score = search_onece(pattern[i]);
		pat_que.push_back(make_pair(pattern[i],score));
	}
	sort(pat_que.begin(), pat_que.end(),cmp);
	int end = clock();
	double used_time = (double)(end - start) /  CLOCKS_PER_SEC;
	printf( "used_time: %f\n", used_time );
}

int isequal(string s1, string s2, int n){
	int cnt = 0;
	for( int i = 0; i < 15; ++i ){
		if( s1[i] != s2[i] ) cnt++;
		if( cnt > n ) return -1;
	}
	return cnt;
}

int search_onece(string pat){

	int total_seq_match = 0;
	int score = 0;
	int total_match = 0;
	for( int i = 0; i < SEQ_CNT; ++i ){
		bool matched = false;
		int min_mu = 15;
		for( int id = 0; id < SEQ_LEN - 15; ++id ){
			string t = sequence[i].substr(id, 15);
			int sc = isequal(t, pat, MAX_MUTATION);
			if( sc != -1 ){
				matched = true;
				if( sc < min_mu ) min_mu = sc;
				total_match++;
			}
		}
		if( matched ){
			total_seq_match++;	
			score += min_mu;
		} 
		else{
			score += MAX_SCORE;
		}
	}

	if( total_seq_match == SEQ_CNT ){
		best_pattern = pat;
		printf( "FIND ANSWER:\n - PATTERN: %s  TOTAL MATCH: %d - \n", best_pattern.c_str(), total_match );
	}
	return score;
}

inline string random_select(){
	int num = rand() % 985;
	string ran = pattern[num];
	int id = rand() % 15;
	int n = rand() % 4;	
	ran[id] = itoc[n];	
	return ran;
}

void get_all_pattern(){
	for( int st = 0; st < SEQ_LEN - PAT_LEN; ++st ){
		pattern.push_back(sequence[0].substr(st, PAT_LEN));
	}
}

void load_sequence(){
	for( int i = 0; i < SEQ_CNT; ++i ){
    	string tmp;
    	cin >> tmp;
    	sequence.push_back(tmp);
	}
}

void output_ans(){
	freopen( "q2_ans.txt", "w", stdout );
}

