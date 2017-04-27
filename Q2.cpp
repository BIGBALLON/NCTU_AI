#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <ctime>
using namespace std;

#define SEQ_LEN 1000
#define SEQ_CNT 50

int v15_size = 0;
int v15_5_size = 0;
vector<string> v15;
vector<string> v15_5;


vector<string> strs;
vector< pair<string,int> > ans[SEQ_CNT + 5];
vector< pair<string,int> > ans_tmp[SEQ_CNT + 5];
string best_pattern;

char trans[] = {'T','C','G','A'};
void transform(string s, int id, int n);
inline bool cmp(string s1, string s2){
	return s1 < s2;
}
bool isequal(string s1, string s2, int n);
int get_common_str15(string s1, string s2);
int get_common_str15_5();
void find_other_str();
void find_other_str_5();
void output_ans();

int main(){

    freopen( "q2.data", "r", stdin );
    
    int start = clock();
    
    for( int i = 0; i < 50; ++i ){
    	string tmp;
    	cin >> tmp;
    	strs.push_back(tmp);
	}
	
    v15_5_size = get_common_str15_5();
	find_other_str_5();
	
	int end = clock();
	double used_time = (double)(end - start) /  CLOCKS_PER_SEC;
	printf( "used_time: %f", used_time );
	
	output_ans();
	
	return 0;
}


bool isequal(string s1, string s2, int n){
	int cnt = 0;
	for( int i = 0; i < 15; ++i ){
		if( s1[i] != s2[i] ) cnt++;
		if( cnt > n ) return false;
	}
	return true;
}

int get_common_str15_5(){
	vector<string> tmp;
	for( int n = 1; n < 50; ++n ){
	    for( int i = 0; i < SEQ_LEN - 15; ++i ){
	        for( int j = 0; j < SEQ_LEN - 15; ++j ){
	            string t1 = strs[0].substr(i, 15);
	            string t2 = strs[n].substr(j, 15);
	            if( isequal(t1, t2, 5) ){
	                tmp.push_back(t1);
	                tmp.push_back(t2);
	            }
	        }   
	    }	
	}
	int size = tmp.size();
	printf( "before unique: %d\n", size );
	sort(tmp.begin(),tmp.end(),cmp);
	v15_5.push_back(tmp[0]);
	for( int i = 1; i < size; ++i  ){
		while( tmp[i] == tmp[i-1] ){
			++i;
		}
		v15_5.push_back(tmp[i]);
	}
	size = v15_5.size();
	printf( "after unique: %d\n", size );
    return size;
}

void transform(string& s, int id, int n){
	s[id] = trans[n];
}

void find_other_str_5(){
	
	printf( "start search:\n");
	int best_ans_match = 0x7fffffff;
	
	for( int c = 0; c < v15_5_size; ++ c ){
		
		for( int k = 0; k < SEQ_CNT; ++k ){
			ans_tmp[k].clear();
		} 
		
		int total_match = 0;
		int total_seq_match = 0;
		
		for( int i = 0; i < SEQ_CNT; ++i ){	
			bool matched = false;
			for( int id = 0; id < SEQ_LEN - 15; ++id ){
				string t = strs[i].substr(id, 15);
				if( isequal(t, v15_5[c], 5) ){
					ans_tmp[i].push_back(make_pair(t, id + 1));
					total_match++;
					matched = true;
				}
			}
			if( !matched ) break; 						//herusitic
			if( total_match > best_ans_match ) break;	//herusitic
			total_seq_match++;
//			printf( "%d\n", total_seq_match );
		}	
		
		if( total_seq_match == SEQ_CNT && total_match < best_ans_match ){
			best_ans_match = total_match;
			best_pattern = v15_5[c];
			for( int k = 0; k < SEQ_CNT; ++k ){
				ans[k].assign(ans_tmp[k].begin(), ans_tmp[k].end());
			} 	
			
			printf( "find new answer: %s %d\n", best_pattern.c_str(), best_ans_match );
		}
	}
}

int get_common_str15(string s1, string s2){
    for( int i = 0; i < SEQ_LEN - 15; ++i ){
        for( int j = 0; j < SEQ_LEN - 15; ++j ){
            string t1 = s1.substr(i, 15);
            string t2 = s2.substr(j, 15);
            if( t1 == t2 ){
                v15.push_back(t1);
            }
        }   
    }
    return v15.size();
}

void find_other_str(){
	int best_ans_match = 0x7fffffff;
	
	for( int c = 0; c < v15_size; ++ c ){
		
		for( int k = 0; k < SEQ_CNT; ++k ){
			ans_tmp[k].clear();
		} 
		
		int total_match = 0;
		int total_seq_match = 0;
		
		for( int i = 0; i < SEQ_CNT; ++i ){	
			bool matched = false;
			int cur_index = 0;
			while( cur_index < SEQ_LEN - 15 ){
				size_t found = strs[i].find(v15[c], cur_index);
				if( found == string::npos ) break;
				matched = true;
				total_match++;
				ans_tmp[i].push_back(make_pair(v15[c],found + 1));
				cur_index = found + 1;
				//cout << cur_index << endl;
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
		}
	}
}

void output_ans(){
	
	freopen( "q2_ans.txt", "w", stdout );
	printf( "pattern: %s\n\n", best_pattern.c_str() );
	
	for( int k = 0; k < SEQ_CNT; ++k ){
		
		printf( "S%d:\n", k + 1 );
		for( int i = 0; i < ans[k].size(); ++i ){
			printf( " {(%s,%d)}\n", ans[k][i].first.c_str(), ans[k][i].second );
		}
		printf("\n");
	} 	
}

