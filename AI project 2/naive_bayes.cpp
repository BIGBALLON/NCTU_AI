#include <cstring>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iterator>
#include <vector>
#include <map>
#include <cmath>
#include <random>
#include <algorithm>
using namespace std;
string file_name;

#define MAX_VALUE 1024
#define MAX_CLASS 66
#define MAX_CNT 10000
#define MAX_SPLITE 100
vector<string> class_vector;
vector<string> feature_vector;
vector<string> value_vector[MAX_VALUE];
vector<string> class_data[MAX_CLASS][MAX_CNT];
map<string,int> check;
map<string,int> number;
int feature_number = 0;
int class_number = 0;
int total_data = 0;
double deviation[MAX_VALUE] = {0};
map< pair<int,string>, double> probability;

template <class Type>  
Type stringToNum(const string& str)  
{  
    istringstream iss(str);  
    Type num;  
    iss >> num;  
    return num;      
}  

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

string& trim(string &str, string::size_type pos = 0){
    static const string delim = " \t";
    pos = str.find_first_of(delim, pos);
    if (pos == string::npos)
        return str;
    return trim(str.erase(pos, 1));
}


void load_names(){
    string load_file = file_name + ".names";
    ifstream names;

    string class_str;
    string space_str;
    string value_str;

    names.open (load_file);
    getline(names,class_str); // read classes
    if( class_str[class_str.length()-1] == '.' ){
        class_str = class_str.substr(0,class_str.length()-1);
    }
    class_str = trim(class_str);
    class_vector = split(class_str, ',');
    // for(int j = 0; j < class_vector.size(); ++j){
    //     cout << class_vector[j] << endl;
    // }

    feature_number = 0;
    getline(names,space_str); // read space
    while(getline(names,value_str)){
        value_str = value_str.substr(0,value_str.length()-1);
        value_str = trim(value_str);
        vector<string> tmp = split(value_str, ':');
        feature_vector.push_back(tmp[0]);
        if(tmp[1]=="continuous"){

        }else{
            value_vector[feature_number] = split(tmp[1], ',');
        }
        feature_number++;
    }
    for(int j = 0; j < class_vector.size(); ++j){
       check[class_vector[j]] = j;
    }
    names.close();
    class_number = class_vector.size();
}

void debug(){

    for(int j = 0; j < class_vector.size(); ++j){
        cout << class_vector[j] << endl;
    }

    cout << endl;
    for (int i = 0; i < feature_number; ++i){
        for(int j = 0; j < value_vector[i].size(); ++j){
            cout << value_vector[i][j] << endl;
        }  
        cout << endl;
    }

}

void load_data(){
    string load_file = file_name + ".data";
    string raw_str;
    ifstream datas;

    datas.open (load_file);

    total_data = 0;
    while(getline(datas,raw_str)){
        total_data += 1;
        size_t found = raw_str.find_last_of(", ");
        string class_value = raw_str.substr(found+1);

        raw_str = trim(raw_str);
        if (number.find(class_value) == number.end()){
            number[class_value] = 0;
        }else{
            number[class_value]++;
        }
        class_data[ check[class_value] ][number[class_value]] = split(raw_str, ',');
    }

    for (int i = 0; i < feature_number; ++i){
        deviation[i] = -10086;
        if( value_vector[i].size() == 0 ){
            double minn = 99999999999;
            double maxx = -99999999999;
            for (int j = 0; j < class_number; ++j){
                for (int k = 0; class_data[j][k].size(); ++k){
                    double tmp = stringToNum<double>(class_data[j][k][i]);
                    // cout << tmp << endl;
                    if( minn > tmp ){
                        minn = tmp;
                    }
                    if( maxx < tmp ){
                        maxx = tmp;
                    }
                }
            }
            // cout << maxx << " " << minn << endl;
            deviation[i] = maxx - minn;
        }
    }
    datas.close();
}

void do_naive_bayes(){
    string load_file = file_name + ".test";
    string raw_str;
    ifstream datas;

    datas.open (load_file);
    double total_test = 0;
    double total_acc = 0;
    while(getline(datas,raw_str)){
        total_test++;
        raw_str = trim(raw_str);
        vector<string> tmp = split(raw_str, ',');
        int best_class = -1;
        double max_p = -1;
        for (int i = 0; i < class_number; ++i){
            int class_cnt = number[class_vector[i]];
            double p = ((double)class_cnt + 1.0 )/ (total_data + class_number * 1.0);
            // double p = ((double)class_cnt)/ (total_data);
            
            for (int k = 0; k < feature_number; ++k){
                double cnt = 0;
                if(probability.find(make_pair(i,tmp[k])) != probability.end()){
                    p = p * probability[make_pair(i,tmp[k])];
                    continue;
                }
                for (int j = 0; j < class_cnt; ++j){
                    // cout << class_data[i][j][k]  << " " << tmp[k] << endl;
                    if( deviation[k] == -10086 ){
                        if( class_data[i][j][k] == tmp[k] ){
                            cnt++;
                        }
                    }else{
                        double real1 = stringToNum<double>(class_data[i][j][k]);
                        double real2 = stringToNum<double>(tmp[k]);
                        if( fabs(real1 - real2) <= deviation[k] / 100.0 ){
                            cnt++;
                        }
                    }
                }
                double pp = (cnt + 1.0) / (double)(class_cnt + value_vector[k].size());
                // double pp = (cnt) / (double)(class_cnt);
                probability[make_pair(i,tmp[k])] = pp;
                p = p * pp;
            }
            // cout << i << ": " << p << endl;
            if( max_p < p ){
                max_p = p;
                best_class = i;
            }
        }
        if (class_vector[best_class] == tmp.back()){
            total_acc++;
        }
    }
    cout << total_acc / total_test << endl;
}

int main(){
    cin >> file_name;
    load_names();
    // debug();
    load_data();
    do_naive_bayes();
    return 0;
}
