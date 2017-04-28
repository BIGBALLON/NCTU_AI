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
#define MAX_MUTATION    7
#define MAX_SCORE       1000

/***********************************************/
vector<string> sequence;
vector<string> pattern[SEQ_CNT];
string check_string;
vector<pair<string,int> > pat_que;
/***********************************************/

/***********************************************/
set<string> hash_table;
char itoc[] = { 'T', 'C', 'G', 'A' };  
string best_pattern;
int best_score = 0x3f3f3f3f;
int best_match = 0x3f3f3f3f;
/***********************************************/


/***********************************************/
void load_sequence();
void load_genome();
void get_all_pattern();
void gen_queue();
inline string random_select();
inline string random_mutation(string str);
int isequal(string s1, string s2, int n);
int search_onece(string pat);
void output_ans();
/***********************************************/


/***********************************************/
bool cmp_pair(const pair<string,int> a, const pair<string ,int> b) { return a.second < b.second; }
bool cmp_str(const string a, const string b) { return a < b; }
/***********************************************/



int start = clock();
    
int main(){

	srand (time(NULL));

	load_sequence();
	load_genome();
	get_all_pattern();
	gen_queue();

    int iteration = 1;
    while( true ){
    	string ran_select_pat = random_select();
    	for( int i = 0; i < MAX_MUTATION; ++i ){
    		string ran_mutation_pat = random_mutation( ran_select_pat );
    		int ran_score = 0; 
	        if( hash_table.find(ran_mutation_pat) != hash_table.end() ){
	        	continue;
			}else{
				hash_table.insert(ran_mutation_pat);
			}
	        ran_score = search_onece(ran_mutation_pat);
			if( ran_score < pat_que[500].second ){
				if( pat_que.size() >= 2000  ) pat_que.pop_back();
				pat_que.push_back( make_pair(ran_mutation_pat,ran_score) );
				sort( pat_que.begin(), pat_que.end(), cmp_pair );
			}
			if( iteration % 100 == 0 ){
				printf( "%d %d %d\r", iteration, pat_que[0].second, hash_table.size() );
			}
			iteration += 1;
		}
    }

	int end = clock();
	double used_time = (double)(end - start) /  CLOCKS_PER_SEC;
	printf( "CUR USED TIME: %f\n", used_time );

	return 0;
}

int isequal(string s1, string s2, int n){
	int cnt = 0;
	for( int i = 0; i < 15; ++i ){
		if( s1[i] != s2[i] ) cnt++;
		if( cnt > n ) return -1;
	}
	return cnt;
}

bool check_answer(string str){
	if( check_string.find(str) != string::npos ){
		return false;
	}
	return true;
}

int search_onece(string pat){

	int total_seq_match = 0;
	int score = 0;
	int total_match = 0;
	for( int i = 0; i < SEQ_CNT; ++i ){
		bool matched = false;
		int min_mu = 15;
		for( int n = 0; n < pattern[i].size(); ++n ){
			int sc = isequal(pattern[i][n], pat, MAX_MUTATION);
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

	if( (total_seq_match == SEQ_CNT) && (best_score > score) && check_answer(pat) ){
		best_score = score;
		best_pattern = pat;
		printf( "\n-----------------------------------------------------------\n" );
		printf( "FIND ANSWER:\n - PATTERN: %s  TOTAL MATCH: %d SCORE: %d - \n", best_pattern.c_str(), total_match, score );
		int end = clock();
		double used_time = (double)(end - start) /  CLOCKS_PER_SEC;
		printf( "CUR USED TIME: %f", used_time );
		printf( "\n-----------------------------------------------------------\n" );
	}
	return score;
}

inline string random_mutation(string source){
	string ran = source;
	int id = rand() % 15;
	int n = rand() % 4;	
	ran[id] = itoc[n];	
	return ran;
}

inline string random_select(){
	int num = rand() % ( 985 + 50 );
	while( num >= pat_que.size() ){
		num = rand() % ( 985 + 50 );
	}
	string ran = pat_que[num].first;
	return ran;
}

void gen_queue(){
	int start = clock();
	for( int i = 0; i < pattern[0].size(); ++i ){
		int score = search_onece(pattern[0][i]);
		pat_que.push_back(make_pair(pattern[0][i],score));
		hash_table.insert(pattern[0][i]);
	}
	sort(pat_que.begin(), pat_que.end(),cmp_pair);
	int end = clock();
	double used_time = (double)(end - start) /  CLOCKS_PER_SEC;
	printf( "used_time: %f\n", used_time );
}

void get_all_pattern(){
	
	for( int n = 0; n < SEQ_CNT; ++n ){
		for( int st = 0; st < SEQ_LEN - PAT_LEN; ++st ){
			pattern[n].push_back(sequence[n].substr(st, PAT_LEN));
		}	
	}
}

void load_genome(){
	freopen( "genome.data", "r", stdin );
	string genome_tmp;
	for( int i = 0; i < 1000; ++i ){
		cin >> genome_tmp;
		check_string = check_string + genome_tmp + ' '; 
	}
}

void load_sequence(){
//	freopen( "q3.data", "r", stdin );
	//  freopen( "q2.data", "r", stdin );
	freopen( "./ex_datasets/ex6_7_mutates.data", "r", stdin );
	for( int i = 0; i < SEQ_CNT; ++i ){
    	string tmp;
    	cin >> tmp;
    	sequence.push_back(tmp);
	}
}

void output_ans(){
	freopen( "q2_ans.txt", "w", stdout );
}

