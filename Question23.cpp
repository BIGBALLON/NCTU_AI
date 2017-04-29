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
#define DNA_NUM         4
#define SEQ_CNT         50
#define MAX_SCORE       0x3f3f3f3f
#define QUE_CNT         20


/***********************************************************
** Define QUESTION_2 to solve question 2
** Define QUESTION_3 to solve question 3
************************************************************/
#define QUESTION_2
//#define QUESTION_3


/***********************************************************
** Set up file path & load_mutation & load_que_cnt
************************************************************/
#ifdef QUESTION_2
#define MAX_MUTATION    5
char output_file[] = "q2_ans.txt";
char input_file[] = "./datasets/q2.data";
char genome_file[] = "./datasets/genome.data";

int load_mutation = 6;
int load_que_cnt = 10;
#endif // QUESTION_2

#ifdef QUESTION_3
#define MAX_MUTATION    7
char output_file[] = "q3_ans.txt";
char input_file[] = "./datasets/q3.data";
char genome_file[] = "./datasets/genome.data";
int load_mutation = MAX_MUTATION;
int load_que_cnt = QUE_CNT;
#endif //QUESTION_3


/***********************************************************
** sequence     ->   Store all the seq
** pattern      ->   Store all the pat
** pat_que      ->   Genetic Algorithm's queue
** hash_table   ->   Avoiding repeated states
** trans_table  ->   Mutation table
************************************************************/
vector<string> sequence;
vector<string> pattern[SEQ_CNT];
vector<pair<string, int> > pat_que;
set<string> hash_table;
char trans_table[] = { 'T', 'C', 'G', 'A' };


/***********************************************************
** Define ans_node to store answer
************************************************************/
typedef struct ANS_NODE {
	string ans_pat;
	int start_id;
	int mutation_cnt;
	bool is_min_mutation;
	ANS_NODE(string _ans_pat, int _start_id, int _mutation_cnt) {
		ans_pat = _ans_pat;
		start_id = _start_id;
		mutation_cnt = _mutation_cnt;
		is_min_mutation = false;
	}
}ans_node;


/***********************************************************
** Define all the functions
************************************************************/
void load_sequence();
void load_genome();
void get_all_pattern();
void gen_queue();
void output_ans();
int search_onece(string pat);
int search_load(string pat);
inline int random_select();
inline int isequal(string& s1, string& s2, int n);
inline void random_mutation(string& source);
inline bool check_stop();
bool cmp_pair(const pair<string, int> a, const pair<string, int> b) { return a.second < b.second; }


/***********************************************************
** iteration    ->   Store GA's iteration
** start        ->   Store program's start time
** best_score   ->   Store minimum mutation numbers
** best_pattern ->   Store best pattern
************************************************************/
static int iteration = 1;
static int pre_iteration = iteration;
static int start = clock();
static int best_score = MAX_SCORE;
static string best_pattern;



int main() {

    srand(time(NULL));
	load_sequence();
	load_genome();
	get_all_pattern();
	gen_queue();

	while (true) {

	    string ran_select_pat = pat_que[random_select()].first;
		for (int i = 0; i < MAX_MUTATION; ++i) {
			random_mutation(ran_select_pat);
			int ran_score = 0;
			if (hash_table.find(ran_select_pat) != hash_table.end()) continue;
			hash_table.insert(ran_select_pat);
			ran_score = search_onece(ran_select_pat);
			pat_que.push_back(make_pair(ran_select_pat, ran_score));
			break;
		}

		if (iteration % 1000 == 0) {
			sort(pat_que.begin(), pat_que.end(), cmp_pair);
			int len = pat_que.size();
			while (--len > 50000) {
				pat_que.pop_back();
			}
			double used_time = (double)(clock() - start) / CLOCKS_PER_SEC;
			printf("ITERATION: %d   MIN MUTATION: %d   CUR TIME: %f\r", iteration, pat_que[0].second, used_time);

            /*
            ** need to stop, but it's a special judge.
            ** so if you want to make sure get the optimal answer,
            ** just run more iterations
            ** [ when you find the answer you want, let the program stop ]
            */

            if(check_stop()) break;
		}
		iteration += 1;
	}

    output_ans();
	return 0;
}

inline bool check_stop(){
#ifdef QUESTION_3
    if( iteration - pre_iteration > 5000 && best_score <= 211 ){
        return true;
    }
#endif // QUESTION_3

#ifdef QUESTION_2
    if( iteration - pre_iteration > 10000 && best_score <= 165 ){
        return true;
    }
#endif // QUESTION_2

    return false;
}

inline int isequal(string& s1, string& s2, int n) {
	int cnt = 0;
	for (int i = 0; i < PAT_LEN; ++i) {
		if (s1[i] != s2[i]) cnt++;
		if (cnt > n) return -1;
	}
	return cnt;
}

int search_onece(string pat) {

	int score = 0;
	for (int i = 0; i < SEQ_CNT; ++i) {
		bool matched = false;
		int min_mu = PAT_LEN;
		for (int n = 0; n < pattern[i].size(); ++n) {
			int sc = isequal(pattern[i][n], pat, MAX_MUTATION);
			if (sc != -1) {
				matched = true;
				if( sc < min_mu ) min_mu = sc;
			}
		}
		if (!matched)	return MAX_SCORE;
		score += min_mu;
	}

	if ( best_score > score ) {
		best_score = score;
		best_pattern = pat;
		pre_iteration = iteration;
		printf("\n-----------------------------------------------------------\n");
		printf("- FIND PATTERN: %s \n- SCORE: %d \n", best_pattern.c_str(), score);
		int end = clock();
		double used_time = (double)(end - start) / CLOCKS_PER_SEC;
		printf("- CUR TIME: %f", used_time);
		printf("\n-----------------------------------------------------------\n");
	}
	return score;
}

int search_load(string pat) {
	int score = 0;
	for (int i = 0; i < SEQ_CNT; ++i) {
		bool matched = false;
		int min_mu = PAT_LEN;
		for (int n = 0; n < pattern[i].size(); ++n) {
			int sc = isequal(pattern[i][n], pat, load_mutation);
			if (sc != -1) {
				matched = true;
				if( sc < min_mu ) min_mu = sc;
			}
		}
		if (!matched)	return MAX_SCORE;
		score += min_mu;
	}
	return score;
}

inline void random_mutation(string& source) {
	int id = rand() % PAT_LEN;
	int n = rand() % DNA_NUM;
	source[id] = trans_table[n];
}

inline int random_select() {
	return rand() % (min<int>(pat_que.size(), SEQ_LEN + 550));
}

void gen_queue() {

	for (int n = 0; n < load_que_cnt; ++n) {
		double used_time = (double)(clock() - start) / CLOCKS_PER_SEC;
		printf("GENERATE QUEEN: %d CUR TIME: %f\r", n + 1, used_time);
		for (int i = 0; i < pattern[n].size(); ++i) {
			if (hash_table.find(pattern[n][i]) != hash_table.end()) continue;
			hash_table.insert(pattern[n][i]);

			int score = search_load(pattern[n][i]);
			if (score == MAX_SCORE) score /= 2;
			pat_que.push_back(make_pair(pattern[n][i], score));
		}
	}
	sort(pat_que.begin(), pat_que.end(), cmp_pair);
	int end = clock();
	double used_time = (double)(end - start) / CLOCKS_PER_SEC;
	printf("\nGENERATE USED TIME: %f\n", used_time);
	printf("-----------------------------------------------------------\n");
}

void get_all_pattern() {

	for (int n = 0; n < SEQ_CNT; ++n) {
		for (int st = 0; st < SEQ_LEN - PAT_LEN; ++st) {
			pattern[n].push_back(sequence[n].substr(st, PAT_LEN));
		}
	}
}

void load_genome() {
	freopen(genome_file, "r", stdin);
	string one_genome;
	for (int n = 0; n < 1000; ++n) {
		cin >> one_genome;
		for (int st = 0; st < SEQ_LEN - PAT_LEN; ++st) {
			hash_table.insert(one_genome.substr(st, PAT_LEN));
		}
	}
}

void load_sequence() {

	freopen(input_file, "r", stdin);

	//freopen( "./datasets/q2.data", "r", stdin );
	//freopen("./ex_datasets/ex3_5_mutates.data", "r", stdin);

	for (int i = 0; i < SEQ_CNT; ++i) {
		string tmp;
		cin >> tmp;
		sequence.push_back(tmp);
	}
}

void output_ans() {

    printf( "\nSAVE ANSWER.\n");
	freopen(output_file, "w", stdout);

    vector<ans_node> output[SEQ_CNT];

    for (int i = 0; i < SEQ_CNT; ++i) {
		int min_mu = 15;
		for (int n = 0; n < pattern[i].size(); ++n) {
			int sc = isequal(pattern[i][n], best_pattern, MAX_MUTATION);
			if (sc != -1) {
                if( min_mu > sc ) min_mu = sc;
                output[i].push_back( ans_node(pattern[i][n],n+1,sc ));
			}
		}
		int index = 0;
		int start = 1001;
		for(int j = 0; j < output[i].size(); ++j){
            if(output[i][j].mutation_cnt == min_mu && output[i][j].start_id < start ){
                index = j;
                start = output[i][j].start_id;
            }
		}
		output[i][index].is_min_mutation = true;
	}

	printf( "pattern: %s\n\n", best_pattern.c_str() );

	for( int k = 0; k < SEQ_CNT; ++k ){
		printf( "S%d:\n", k + 1 );
		for( int i = 0; i < output[k].size(); ++i ){
            if( output[k][i].is_min_mutation ){
                printf( "*" );
            }else printf( " " );
			printf( "{(%s,%d)}\n", output[k][i].ans_pat.c_str(), output[k][i].start_id );
		}
		printf("\n");
	}
}
