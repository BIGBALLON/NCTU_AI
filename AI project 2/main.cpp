#include <cstring>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iterator>
#include <vector>
#include <map>
#include <random>
#include <algorithm>
using namespace std;

#define SPLIT_K 10
#define MAX_CLASS 6666

int class_number = 0;
vector<string> class_data[MAX_CLASS];
map<string,int> check;

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}
string& trim(string &str, string::size_type pos = 0){
    static const string delim = " \t";
    pos = str.find_first_of(delim, pos);
    if (pos == string::npos)
        return str;
    return trim(str.erase(pos, 1));
}

int set_class_num(string file);
bool load_and_split(string file);
std::vector<std::string> split(const std::string &s, char delim);
void init();

int main(){

    string dir = "./dataset/";
    string names[] = {"adult", "car", "isolet", "page-blocks", "winequality"};

    for( int i = 0; i < 5; ++i){
        init();
        class_number = set_class_num(dir + names[i] + ".names");
        load_and_split(dir + names[i] + ".data");
    }

    return 0;
}

void init(){
    class_number = 0;
    check.clear();
    for( int i = 0; i < MAX_CLASS; ++i){
        class_data[i].clear();
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}


int set_class_num(string file){
    ifstream names;
    names.open (file);
    string class_str;
    getline(names,class_str); // read classes
    if( class_str[class_str.length()-1] == '.' ){
        class_str = class_str.substr(0,class_str.length()-1);
    }
    class_str = trim(class_str);
    vector<string> class_vector = split(class_str, ',');
    names.close();

    string pre = file.substr(0,file.find_last_of("."));
    for( int i = 1; i <= SPLIT_K; ++i ){
        stringstream ss;
        ss << i;
        ifstream src(file, std::ios::binary);
        ofstream dest(pre+"_cv"+ ss.str() + ".names", std::ios::binary);
        dest << src.rdbuf();
        src.close();
        dest.close();
    }

    return class_vector.size();
}

bool load_and_split(string file){

    ifstream datas;
    datas.open (file);
    string raw_str;
    int class_cnt = 0;
    int data_cnt = 0;
    while(getline(datas,raw_str)){
        data_cnt++;
        size_t found = raw_str.find_last_of(", ");
        string class_value = raw_str.substr(found+1);
        if(check.find(class_value) == check.end()){
            check[class_value] = class_cnt;
            class_data[class_cnt].push_back(raw_str);
            class_cnt++;
        }else{
            class_data[ check[class_value] ].push_back(raw_str);
        }
    }
    datas.close();

    vector<string> test;
    vector<string> data;
    string pre = file.substr(0,file.find_last_of("."));
    for( int i = 0; i < SPLIT_K; ++i ){
        ofstream output_test;
        ofstream output_data;
        stringstream ss;
        ss << i+1;
        output_test.open(pre+"_cv"+ ss.str() + ".test");
        output_data.open(pre+"_cv"+ ss.str() + ".data");
        test.clear();
        data.clear();
        for(int j = 0; j < class_number; ++j ){
            int cnt = class_data[j].size() / SPLIT_K;
            for( int k = 0; k < cnt; ++k ){
                if( i*cnt+k < class_data[j].size() )
                    test.push_back(class_data[j][i*cnt+k]);
            }
            for( int k = i*cnt-1; k >= 0; --k ){
                data.push_back(class_data[j][k]);
            }
            for( int k = (i+1)*cnt; k < class_data[j].size(); ++k ){
                data.push_back(class_data[j][k]);
            }
        }
        auto engine = std::default_random_engine{};
        std::shuffle(std::begin(test), std::end(test), engine);
        for( int k = 0; k < test.size(); ++k ){
            output_test << test[k] << endl;
        }

        std::shuffle(std::begin(data), std::end(data), engine);
        for( int k = 0; k < data.size(); ++k ){
            output_data << data[k] << endl;
        }
        output_test.close();
        output_data.close();
    }

    return true;
}
