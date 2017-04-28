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
#define MAX_SCORE       0x3f3f3f3f

typedef struct ANS_NODE{
	string ans_pat;
	int start_id;
	int mutation_cnt;
	bool is_min_mutation;
	ANS_NODE( string _ans_pat, int _start_id, int _mutation_cnt ){
		ans_pat = _ans_pat;
		start_id = _start_id;
		mutation_cnt = _mutation_cnt;
		is_min_mutation = false;
	}
}ans_node;

/***********************************************/
vector<string> sequence;
vector<string> pattern[SEQ_CNT];
string check_string;
vector<pair<string,int> > pat_que;
vector<ans_node> ans[SEQ_CNT];
vector<ans_node> ans_tmp[SEQ_CNT];

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
inline int random_select();
inline void random_mutation(string& source);
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

//	srand (time(NULL));
	load_sequence();
	load_genome();
	get_all_pattern();
	gen_queue();

    int iteration = 1;
    
    
    while( true ){
    	string ran_select_pat = pat_que[random_select()].first;
    	for( int i = 0; i < MAX_MUTATION; ++i ){
    		random_mutation( ran_select_pat );
    		int ran_score = 0; 
	        if( hash_table.find(ran_select_pat) != hash_table.end() ) continue;		
			hash_table.insert(ran_select_pat);
			ran_score = search_onece(ran_select_pat);
			pat_que.push_back( make_pair(ran_select_pat,ran_score) );
			break;
		}
        
		if( iteration % 1000 == 0 ){
			sort( pat_que.begin(), pat_que.end(), cmp_pair );
			int len = pat_que.size();
			while( --len > 50000 ){
				pat_que.pop_back();	
			} 
		}
		if( iteration % 100 == 0 ){
			printf( "%d %d %d\r", iteration, pat_que[0].second, hash_table.size() );
		}
		
		iteration += 1;
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
	int max_mu = 0;
	static int cur_max_mutation = MAX_MUTATION;
	memset( ans_tmp, 0, sizeof(ans_tmp) );

	for( int i = 0; i < SEQ_CNT; ++i ){
		bool matched = false;
		int min_mu = 15;
		for( int n = 0; n < pattern[i].size(); ++n ){
			int sc = isequal(pattern[i][n], pat, cur_max_mutation);
			if( sc != -1 ){
				matched = true;
				if( sc < min_mu ) min_mu = sc;
				if( sc > max_mu ) max_mu = sc;
				total_match++;
				ans_tmp[i].push_back(  ans_node( pattern[i][n], n + 1, sc ) );
			}
		}
		if( matched ){
			total_seq_match++;	
			score += min_mu;
		} 
		else{
			return MAX_SCORE;
		}
		for( int n = 0; n < ans_tmp[i].size(); ++n ){
			ans_tmp[i][n].mutation_cnt = min_mu;
			ans_tmp[i][n].is_min_mutation = true;
		}
	}
	
	if( (max_mu <= MAX_MUTATION) && (best_score > score) && check_answer(pat) ){
		cur_max_mutation = MAX_MUTATION;
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

inline void random_mutation(string& source){
	int id = rand() % 15;
	int n = rand() % 4;	
	source[id] = itoc[n];	
}

inline int random_select(){
	return rand() % ( min<int>(pat_que.size(),985 + 300) );
}

void gen_queue(){
	
	int start = clock();
	for( int n = 0; n < SEQ_CNT; ++n ){
		printf( "LONDING SEQUENCE: %d\r", n );
		for( int i = 0; i < pattern[n].size(); ++i ){
			int score = search_onece(pattern[n][i]);
			if( score == MAX_SCORE ) score /= 2;
			pat_que.push_back(make_pair(pattern[n][i],score));
			
	        if( hash_table.find(pattern[n][i]) != hash_table.end() ){
	        	continue;
			}else{
				hash_table.insert(pattern[n][i]);
			}
		}
	}
	sort(pat_que.begin(), pat_que.end(),cmp_pair);
	int end = clock();
	double used_time = (double)(end - start) / CLOCKS_PER_SEC;
	printf( "\nUSED TIME: %f\n", used_time );
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
	freopen( "./ex_datasets/ex3_5_mutates.data", "r", stdin );
	for( int i = 0; i < SEQ_CNT; ++i ){
    	string tmp;
    	cin >> tmp;
    	sequence.push_back(tmp);
	}
}

void output_ans(){
	freopen( "q2_ans.txt", "w", stdout );
}

