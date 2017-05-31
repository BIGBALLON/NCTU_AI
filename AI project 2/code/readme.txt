
- cv_tools.cpp      k-fold stratified CV tool 的 code
- naive_bayes.cpp   naive bayes 的 code

- compile.sh 对 cv_tools.cpp 和 naive_bayes.cpp 进行compile
    - 或者可以直接使用 g++ -std=c++11 -O3 cv_tools.cpp -o cv_tool
                       g++ -std=c++11 -O3 naive_bayes.cpp -o naive_bayes
                       对两个文件进行编译
    - 请注意，一定要使用C++11進行compile


- 0_CV_run.sh 使用 k-fold stratified CV tool 对5种data set进行切割
    - 其中默认k=10，如要修改，请在code中将 SPLIT_K 改为其他数值
    - 使用时请将该sh文件，以及cv_tools.exe 和 dataset文件放在同一目录下
- 0_NB_run.sh 使用naive bayes 对 5种data set进行分类
    - 使用时请将该sh文件，以及naive_bayes.exe 和 dataset文件放在同一目录下
- 0_C45_run.sh 使用C4.5 对 5种data set进行分类
    - 使用时请将该sh文件，以及C45.exe 和 dataset文件放在同一目录下