#ifndef OBJECTS_H
#define OBJECTS_H
#include <iostream>
#include <list>
#include <vector>
#include <set>
using namespace std;


class functor_teacher{
public:
    bool operator()(pair<int, int>, pair<int, int>) const;
};

class functor{
public:
    bool operator ()(const vector<int>&, const vector<int>&) const;
};

class data{
    int * val;
    int size1;
    int size2;
    int size3;
    int ***stats;
    multiset<vector<int>, functor> best;
    set<int> subjects;
    set<pair<int, int>, functor_teacher> teachers;
public:
    data();
    data(int ***);
    void autofill(int, int, int);
    void set_global_values(vector<int>&);
    int set_values(int, int, int, int);
    void generate(list<vector<int>> &);
    set<int>::iterator put_in_subjects(int);
    bool put_in_teachers(int, int);
    void regression(list<vector<int>>&);
    bool try_regress(list<vector<int>>&, list<vector<int>>::iterator,
                     list<vector<int>>::iterator, int, int, int);
    void check_changes(list<vector<int>>&, list<vector<int>>::iterator,
                       list<vector<int>>::iterator);
    int eval_estim(list<vector<int>> &);
    void print(list<vector<int>> &);
    void print_stats();
};

class func{
public:
    func(){}
    virtual int count(list<vector<int>>&) = 0;
};

class func1 : public func{
    int val;
public:
    func1(int);
    int count(list<vector<int>>&);
};


#endif // OBJECTS_H
