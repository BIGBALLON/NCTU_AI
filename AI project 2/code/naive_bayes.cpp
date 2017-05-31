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

#define MAX_VALUE    1024
#define MAX_CLASS    100
#define MAX_CNT      10000
#define MAX_SPLITE   100
const double pi    = 3.141592653; 

static string file_name;
static int feature_number = 0;
static int class_number = 0;
static int total_data = 0;

// some vectors and maps to save data
vector<string> class_vector;                        // a vector to save all class
vector<string> feature_vector;                      // a vector to save all features
vector<string> value_vector[MAX_VALUE];             // vectors to save all values of the each features
vector<string> class_data[MAX_CLASS][MAX_CNT];      // vectors to save all the training data
map<string,int> index;                              // a map to mapping class string to index number
map<string,int> number;                             // a map to mapping class string to count number

// discretization
int continuous[MAX_VALUE];
double mean[MAX_CLASS][MAX_VALUE];
double variance[MAX_CLASS][MAX_VALUE];

// save the probability to save time
map< pair<int, pair<int,string> >, double> probability;

// functions to solve problems
template <class Type> Type str_to_num(const string& str);                         // convert string to number
template<typename Out> void split(const std::string &s, char delim, Out result);  
vector<string> split(const string &s, char delim);                                // split string by char delim
string& trim(string &str, string::size_type pos = 0){                             // delete all the space in one line
    string delim = " \t";
    pos = str.find_first_of(delim, pos);
    if (pos == string::npos)
        return str;
    return trim(str.erase(pos, 1));
}

void debug();                                        // print detail information
void load_names();                                   // load xx.names and save features
void load_data();                                    // load xx.data and split data for naive_bayes & calculate mean and variance
void do_naive_bayes();                               // load xx.test and run naive bayes classification algorithm

// main function
int main(int argc, char const *argv[]){

    if( argc == 2 ){
        file_name = argv[1];    
    }else{
        cout << "please input file name: ";
        cin >> file_name;
    }

    load_names();
    load_data();
    do_naive_bayes();

    return 0;
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

    feature_number = 0;
    getline(names,space_str); // read space
    while(getline(names,value_str)){
        value_str = value_str.substr(0,value_str.length()-1);
        value_str = trim(value_str);
        if(value_str == "") continue;
        vector<string> tmp = split(value_str, ':');
        feature_vector.push_back(tmp[0]);
        if(tmp[1]=="continuous"){ // just ignore 

        }else{
            value_vector[feature_number] = split(tmp[1], ',');
        }
        feature_number++;
    }
    for(int j = 0; j < class_vector.size(); ++j){
       index[class_vector[j]] = j;
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
        class_data[ index[class_value] ][number[class_value]] = split(raw_str, ',');
    }

    for (int i = 0; i < feature_number; ++i){
        continuous[i] = 0;
        if( value_vector[i].size() == 0 ){
            continuous[i] = 1;
            for (int j = 0; j < class_number; ++j){
                double mean_tmp = 0;
                int class_cnt = number[class_vector[j]];
                for (int k = 0; k < class_cnt; ++k){
                    mean_tmp += str_to_num<double>(class_data[j][k][i]);
                }
                mean_tmp /= class_cnt;
                mean[j][i] = mean_tmp;
            }
        }
    }

    for (int i = 0; i < feature_number; ++i){
        if( continuous[i] ){
            for (int j = 0; j < class_number; ++j){
                int class_cnt = number[class_vector[j]];

                double mean_tmp = mean[j][i];
                double variance_tmp = 0;
                for (int k = 0; k < class_cnt; ++k){
                    variance_tmp += (str_to_num<double>(class_data[j][k][i]) - mean[j][i])
                                    *(str_to_num<double>(class_data[j][k][i]) - mean[j][i]);
                }
                variance_tmp /= class_cnt;
                variance[j][i] = variance_tmp;
    
                // cout << "mean :" << mean[j][i] << "  ";
                // cout << "variance :" << variance[j][i] << endl;
            }
        }
    }
    datas.close();
}

void do_naive_bayes(){
    string load_file = file_name + ".test";
    string raw_str;

    ifstream datas;
    datas.open (load_file);

    double total_test = 0.0;
    double total_acc = 0.0;
    while(getline(datas,raw_str)){
        total_test++;
        raw_str = trim(raw_str);
        vector<string> tmp = split(raw_str, ','); // split the data by , & save the featrues in vector tmp
        int best_class = -1.0;
        double max_p = -1.0;
        for (int i = 0; i < class_number; ++i){
            int class_cnt = number[class_vector[i]];
            // double p = ((double)class_cnt + 1.0 )/ (total_data + class_number * 1.0); // using laplace correction
            double p = ((double)class_cnt)/ ((double)total_data);
            
            for (int k = 0; k < feature_number; ++k){
                if( continuous[k] == 0 ){
                    double cnt = 0;
                    if(probability.find(make_pair(i, make_pair(k,tmp[k]))) != probability.end()){
                        p = p * probability[make_pair(i, make_pair(k,tmp[k]))];
                        continue;
                    }
                    for (int j = 0; j < class_cnt; ++j){
                        if( class_data[i][j][k] == tmp[k] ){
                            cnt++;
                        }
                    }
                    double pp = (cnt + 1.0) / ((double)class_cnt + value_vector[k].size()); // using laplace correction
                    probability[make_pair(i, make_pair(k,tmp[k]))] = pp;
                    p = p * pp;
                }else{
                    double x = str_to_num<double>(tmp[k]);
                    double pp = 1.0/(sqrt(pi * 2 * variance[i][k]))              // using normal distribution to cal probability
                                * exp(( -(x-mean[i][k]) * (x-mean[i][k])) / ( 2 * variance[i][k] )  );
                    p = p * pp;

                }
            }
            if( max_p < p ){
                max_p = p;
                best_class = i;
            }
        }
        if (class_vector[best_class] == tmp.back()){
            total_acc++;
        }
    }
    cout << "acc: " << total_acc / total_test << endl;
}

template <class Type>  
Type str_to_num(const string& str)  {  
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

vector<string> split(const string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

