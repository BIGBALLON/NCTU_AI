#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <set>
using namespace std;

#define SEQ_LEN 1000
#define SEQ_CNT 50
#define PAT_LEN 15

int v15_size = 0;
vector<string> v15;
set<string> hash_table;
vector<string> strs;
vector< pair<string,int> > ans[SEQ_CNT + 5];
vector< pair<string,int> > ans_tmp[SEQ_CNT + 5];
string best_pattern;


void get_common_str15(string s1, string s2);
void find_other_str();
void load_datasets();
void output_ans();


int start = clock();

int main(){

	load_datasets();
    get_common_str15(strs[0],strs[1]);
	find_other_str();
	output_ans();

	return 0;
}

void get_common_str15(string s1, string s2){
    for( int i = 0; i < SEQ_LEN - PAT_LEN; ++i ){
        for( int j = 0; j < SEQ_LEN - PAT_LEN; ++j ){
            string t1 = s1.substr(i, PAT_LEN);
            string t2 = s2.substr(j, PAT_LEN);
            if( t1 == t2 ) v15.push_back(t1);
        }
    }
    v15_size = v15.size();
}

void find_other_str(){
	int best_ans_match = 0x7fffffff;

	for( int c = 0; c < v15_size; ++ c ){
        if (hash_table.find(v15[c]) != hash_table.end()) continue;
		for( int k = 0; k < SEQ_CNT; ++k ) ans_tmp[k].clear();
		int total_match = 0;
		int total_seq_match = 0;

		for( int i = 0; i < SEQ_CNT; ++i ){
			bool matched = false;
			int cur_index = 0;
			while( cur_index < SEQ_LEN - PAT_LEN ){
				size_t found = strs[i].find(v15[c], cur_index);
				if( found == string::npos ) break;
				matched = true;
				total_match++;
				ans_tmp[i].push_back(make_pair(v15[c],found + 1));
				cur_index = found + 1;
			}
			if( !matched ) break;
			if( total_match > best_ans_match ) break;
			total_seq_match++;
		}
		if( total_seq_match == SEQ_CNT && total_match < best_ans_match ){
			best_ans_match = total_match;
			best_pattern = v15[c];
			for( int k = 0; k < SEQ_CNT; ++k ){
				ans[k].assign(ans_tmp[k].begin(), ans_tmp[k].end());
			}
		printf("-----------------------------------------------------------\n");
		printf("- FIND PATTERN: %s ", best_pattern.c_str());
		double used_time = (double)(clock() - start) / CLOCKS_PER_SEC;
		printf("- USED_TIME: %f \n", used_time);
		printf("-----------------------------------------------------------\n");
		}
	}
}

void load_datasets(){

	string tmp;
	freopen( "./datasets/q1.data", "r", stdin );
    for( int i = 0; i < SEQ_CNT; ++i ){
    	cin >> tmp;
    	strs.push_back(tmp);
	}
	freopen("./datasets/genome.data", "r", stdin);
	string one_genome;
	for (int n = 0; n < 1000; ++n) {
		cin >> one_genome;
		for (int st = 0; st < SEQ_LEN - PAT_LEN; ++st) {
			tmp =  one_genome.substr(st, PAT_LEN);;
			hash_table.insert(tmp);
		}
	}
}

void output_ans(){
	freopen( "q1_ans.txt", "w", stdout );
	printf( "pattern: %s\n\n", best_pattern.c_str() );

	for( int k = 0; k < SEQ_CNT; ++k ){
		printf( "S%d:\n", k + 1 );
		for( int i = 0; i < ans[k].size(); ++i ){
			printf( "*{(%s,%d)}\n", ans[k][i].first.c_str(), ans[k][i].second );
		}
		printf("\n");
	}
}

