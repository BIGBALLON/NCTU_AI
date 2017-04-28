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


#define MAX_MUTATION    5
#define SEQ_LEN         1000
#define PAT_LEN         15
#define DNA_NUM         4
#define SEQ_CNT         50
#define MAX_SCORE       0x3f3f3f3f
#define QUE_CNT         30

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

/***********************************************/
vector<string> sequence;
vector<string> pattern[SEQ_CNT];
vector<pair<string, int> > pat_que;
vector<ans_node> ans[SEQ_CNT];
vector<ans_node> ans_tmp[SEQ_CNT];

set<string> hash_table;

char itoc[] = { 'T', 'C', 'G', 'A' };


int best_score = 0x3f3f3f3f;
int best_match = 0x3f3f3f3f;
int load_mutation = MAX_MUTATION;
int load_que_cnt = QUE_CNT;
string best_pattern;

/***********************************************/


/***********************************************/
void load_sequence();
void load_genome();
void get_all_pattern();
void gen_queue();
inline int random_select();
inline void random_mutation(string& source);
inline int isequal(string& s1, string& s2, int n);
int search_onece(string pat);
int search_load(string pat);
void output_ans();
/***********************************************/


/***********************************************/
bool cmp_pair(const pair<string, int> a, const pair<string, int> b) { return a.second < b.second; }
bool cmp_str(const string a, const string b) { return a < b; }
/***********************************************/


int start = clock();

int main() {

	srand(time(NULL));
	load_sequence();
	load_genome();
	get_all_pattern();
	gen_queue();

	int iteration = 1;


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
			printf("ITERATION: %d  MIN SCORE: %d  CUR TIME: %f\r", iteration, pat_que[0].second, used_time);
		}
		iteration += 1;
	}

	return 0;
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
		int min_mu = 15;
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
		int min_mu = 15;
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
	source[id] = itoc[n];
}

inline int random_select() {
	return rand() % (min<int>(pat_que.size(), 985 + 350));
}

void gen_queue() {

    if( MAX_MUTATION == 5 ){
        load_mutation = 6;
        load_que_cnt = 10;
    }
	for (int n = 0; n < load_que_cnt; ++n) {
		double used_time = (double)(clock() - start) / CLOCKS_PER_SEC;
		printf("GENERATE QUEEN: %d CUR_TIME: %f\r", n + 1, used_time);
		for (int i = 0; i < pattern[n].size(); ++i) {
			if (hash_table.find(pattern[n][i]) != hash_table.end()) continue;
			hash_table.insert(pattern[n][i]);

			//int score = search_onece(pattern[n][i]);
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
	freopen("./datasets/genome.data", "r", stdin);
	string one_genome;
	for (int n = 0; n < 1000; ++n) {
		cin >> one_genome;
		for (int st = 0; st < SEQ_LEN - PAT_LEN; ++st) {
			hash_table.insert(one_genome.substr(st, PAT_LEN));
		}
	}
}

void load_sequence() {

	freopen( "./datasets/q2.data", "r", stdin );
	//freopen("./ex_datasets/ex6_7_mutates.data", "r", stdin);
	//freopen("./ex_datasets/ex1_5_mutates.data", "r", stdin);
	for (int i = 0; i < SEQ_CNT; ++i) {
		string tmp;
		cin >> tmp;
		sequence.push_back(tmp);
	}
}

void output_ans() {
	freopen("q2_ans.txt", "w", stdout);
}

