#include "objects.h"


bool functor_teacher::operator()(pair<int, int> p1, pair<int, int> p2) const{
    if (p1.first == p2.first)
        return p1.second < p2.second;
    return p1.first < p2.first;
}


bool functor::operator()(const vector<int>& vec1, const vector<int>& vec2) const{
    return vec1[vec1.size() - 1] >= vec2[vec2.size() - 1];
}

data::data(){
    stats = nullptr;
    size1 = 0;
    size2 = 0;
    size3 = 0;
}

data::data(int *** s){
    stats = s;
}


void data::autofill(int s1, int s2, int s3){
    size1 = s1;
    size2 = s2;
    size3 = s3;
    stats = new int **[s1];
    for (int i = 0; i < s1; i++)
        stats[i] = new int *[s2];
    for (int i = 0; i < s1; i++)
        for (int j = 0; j < s2; j++)
            stats[i][j] = new int[s3];
    for (int i = 0; i < s1; i++)
        for (int j = 0; j < s2; j++)
            for (int k = 0; k < s3; k++)
               stats[i][j][k] = 0;
    val = new int[size2];
}

void data::set_global_values(vector<int> &v){
    int num = v.size();
    if (num != size2){
        cout << "Wrong num of global params" << endl;
        return;
    }
    for (int i = 0; i < num; i++)
        val[i] = v[i];
}

int data::set_values(int a, int b, int c, int d){
    if (a >= size1)
        return -1;
    if (b >= size2)
        return -2;
    if (c >= size3)
        return -3;
    vector<int> tmp = {a, b, c, d};
    best.insert(tmp);
    stats[a][b][c] = d;
    return 0;
}

void swap_el(int &el, int &el2){
    int tmp = el;
    el = el2;
    el2 = tmp;
}

set<int>::iterator data::put_in_subjects(int num){
    auto p = subjects.insert(num);
    if (p.second)
        return p.first;
    return subjects.end();
}

bool data::put_in_teachers(int num1, int num2){
    return !teachers.insert(make_pair(num1, num2)).second;
}

void data::generate(list<vector<int>> &res){
    int r1, r2, c = 0;
    vector<int> counter(size2);
    const vector<int> max({1, 1, 1, 100});
    const vector<int> min({1, 1, 1, 1});
    auto lb = best.lower_bound(max), ub = best.lower_bound(min);
    try{
        for (auto iter = lb; iter != ub; ++iter){
            if (counter[(*iter)[1]] == val[(*iter)[1]])
                continue;
            ++counter[(*iter)[1]];
            auto it = put_in_subjects((*iter)[2]);
            if (it == subjects.end())
                continue;
            if (put_in_teachers((*iter)[0], (*iter)[1])){
                subjects.erase(it);
                continue;
            }
            if (stats[(*iter)[0]][(*iter)[1]][(*iter)[2]] < 0)
                res.push_front({(*iter)[0], (*iter)[1], (*iter)[2]});
            else res.push_back({(*iter)[0], (*iter)[1], (*iter)[2]});
            ++c;
            if (res.size() < c) throw 0;
        }
    }
    catch(int){
        cout << "Can`t push into list while generating" << endl;
        return;
    }


    for (int i = 0; i < size3; i++){
        if (put_in_subjects(i) == subjects.end())
            continue;
        do{
            r1 = rand() % size1;
            r2 = rand() % size2;
        }
        while (put_in_teachers(r1, r2));
        if (stats[r1][r2][i] < 0)
            res.push_front({r1, r2, i});
        else res.push_back({r1, r2, i});
    }
}



void data::regression(list<vector<int>> &res){
    int j = 0;
    for (auto iter = res.begin(); iter != res.end(); ++iter){
        if (stats[(*iter)[0]][(*iter)[1]][(*iter)[2]] >= 0)
            break;
        bool flag = false;
        for (auto iter2 = res.begin(); iter2 != res.end(); ++iter2){
            if (iter == iter2) continue;
            for (int i = 0; i < 3; i++){
                flag = try_regress(res, iter, iter2, i == 1, i == 2, i == 3);
                if (flag)
                    break;
            }
            if (flag)
                break;
        }
    }
}


bool data::try_regress(list<vector<int>> &res, list<vector<int>>::iterator el1,
                 list<vector<int>>::iterator el2, int i, int j, int k){
    try{
        if ((*el1)[0] > size1 || (*el1)[1] > size2 || (*el1)[2] > size3) throw 1;
        if ((*el2)[0] > size1 || (*el2)[1] > size2 || (*el2)[2] > size3) throw 2;
        if (abs(i) > 1 || abs(j) > 1 || abs(k) > 1) throw "Fuck";
    }
    catch(int val){
        cout << "Wrong indices in el " << val << " in regression" << endl;
        return true;
    }
    catch(const char *){
        cout << "Bad parameters in regression " << endl;
        return true;
    }

    if (*(*(*(stats + (*el1)[0] * i + (*el2)[0] * (1 - i)) + (*el1)[1] * j + (*el2)[1] * (1 - j)) +
        + (*el1)[2] * k + (*el2)[2] * (1 - k)) + *(*(*(stats + (*el2)[0] * i + (*el1)[0] * (1 - i)) +
        (*el2)[1] * j + (*el1)[1] * (1 - j)) + (*el2)[2] * k + (*el1)[2] * (1 - k)) >
            stats[(*el1)[0]][(*el1)[1]][(*el1)[2]] + stats[(*el2)[0]][(*el2)[1]][(*el2)[2]]){
        swap_el((*el1)[2], (*el2)[2]);
        check_changes(res, el1, el2);
        return true;
    }
    return false;
}

void data::check_changes(list<vector<int>> &res, list<vector<int>>::iterator el1,
                   list<vector<int>>::iterator el2){
    vector<int> tmp(3);
    if (stats[(*el1)[0]][(*el1)[1]][(*el1)[2]] >= 0){
        for (int i = 0; i < 3; i++)
            tmp[i] = (*el1)[i];
        res.erase(el1);
        res.push_back(tmp);
    }
    for (int i = 0; i < 3; i++)
        tmp[i] = (*el2)[i];
    res.erase(el2);
    if (stats[tmp[0]][tmp[1]][tmp[2]] < 0)
        res.push_front(tmp);
    else res.push_back(tmp);
}

int data::eval_estim(list<vector<int>> &res){
    int sum = 0;
    vector<func *> funcs;
    func * tmp = new func1(10);
    funcs.push_back(tmp);
    try{
        for (auto &el : res){
            if (el[0] > size1 || el[1] > size2 || el[2] > size3) throw 0;
            sum += stats[el[0]][el[1]][el[2]];
        }
    }
    catch(int){
        cout << "Wrong indices in eval" << endl;
        return -10000;
    }
    for (auto &functor : funcs)
        sum += functor->count(res);
    return sum;
}

void data::print(list<vector<int>> &res){
    for (auto &el : res){
        for (auto &el2 : el)
            cout << el2 << " ";
        cout << stats[el[0]][el[1]][el[2]] << endl;
    }
}

void data::print_stats(){
    cout << "Stats " << endl;
    for (int i = 0; i < size1; i++){
        for (int j = 0; j < size2; j++){
            for (int k = 0; k < size3; k++)
                cout << stats[i][j][k] << " ";
            cout << endl;
        }
        cout << endl << endl;
    }
}




func1::func1(int n){
    val = n;
}

int func1::count(list<vector<int>>& res){
    int sum = 0;
    for (auto &el : res){
        if (el[0] == 1)
            sum++;
    }
    if (sum > val)
        return -10;
    if (sum == val)
        return 10;
    return 0;
}


