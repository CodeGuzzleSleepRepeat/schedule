#include "server.h"

server::server(){
    db_users = QSqlDatabase::addDatabase("QSQLITE");
    db_users.setDatabaseName("users.db");
    db_users.open();
    QSqlQuery test_query;
    test_query.exec("CREATE TABLE users (LOGIN varchar(20) PRIMARY KEY, "
                    " PASSWORD varchar(20), NAME varchar(50), PROJECTS integer(10), flag integer);");
    test_query.exec("CREATE TABLE projects (ID integer PRIMARY KEY AUTOINCREMENT NOT NULL, "
                    "HEAD_LOGIN varchar(20), HEAD_NAME varchar(50), DEPARTMENT varchar(100));");
}

int server::create_user(string login, string pass, string name){
    QSqlQuery query;
    query.exec("SELECT * from users WHERE LOGIN = '" + QString::fromStdString(login) + "';");
    if(query.next())
        return -1;
    query.exec("INSERT into users (LOGIN, PASSWORD, NAME) values('" + QString::fromStdString(login) +
               "', '" + QString::fromStdString(pass) + "', '" + QString::fromStdString(name) + "');");
    return 1;
}


int server::log_in(string login, string pass, string &name){
    QSqlQuery query;
    query.exec("SELECT PASSWORD from users WHERE LOGIN ='" + QString::fromStdString(login) + "';");
    if(!query.next()) return -2;
    if (query.value(0).toString().toStdString() == pass){
        query.exec("UPDATE users SET flag = 1 WHERE LOGIN ='" + QString::fromStdString(login) + "';");
        query.exec("SELECT NAME from users WHERE LOGIN ='" + QString::fromStdString(login) + "';");
        query.next();
        name = query.value(0).toString().toStdString();
        return 1;
    }
    return -1;
}

void server::log_out(string login){
    QSqlQuery query;
    query.exec("UPDATE users SET flag = 0 WHERE LOGIN ='" + QString::fromStdString(login) + "';");
}

int server::create_project(string login, string head_name, string proj_name){
    QSqlQuery query;
    query.exec("INSERT into projects (HEAD_LOGIN, HEAD_NAME, DEPARTMENT) values('"
               + QString::fromStdString(login) + "', '" + QString::fromStdString(head_name) + "', '" +
                    QString::fromStdString(proj_name) + "');");
    query.exec("CREATE TABLE " + QString::fromStdString(proj_name) + " (NUMBER integer PRIMARY KEY "
                    "AUTOINCREMENT NOT NULL, "
                    "TEACHER varchar(50), NUM_OF_HOURS integer, SUBJECTS varchar(1000), "
                     "VALUES_SUBJECT varchar(1000));");
    query.exec("CREATE TABLE " + QString::fromStdString(proj_name + "_subjects") +
                    " (NUMBER integer PRIMARY KEY AUTOINCREMENT NOT NULL, SUBJECT varchar(50), "
                    "GROUP_NUM varchar(30));");
    return 1;
}


void server::put_in_subjects(string name, vector<pair<string, string>> &subject){
    QSqlQuery query;
    int size = subject.size();
    for (int i = 0; i < size; i++)
        query.exec("INSERT into " + QString::fromStdString(name + "_subjects") + " (SUBJECT, GROUP_NUM) "
                   " values('" + QString::fromStdString(subject[i].first) + "', '" +
                   QString::fromStdString(subject[i].second) + "');");

}
//send all names, get email, names of good, send ok

//check hours if norm

int server::put_in_teachers(string name, string teacher_name, int hours, vector<int> &values){
    QSqlQuery query;
    QString val = "", proj = "";
    int num_of_proj;
    query.exec("SELECT NUMBER from " + QString::fromStdString(name) + " WHERE TEACHER = '" +
               QString::fromStdString(teacher_name) + "';");
    //if (query.next())
      //  return -1;

    for (auto &el : values)
        val += QString::fromStdString(to_string(el)) + " ";
    query.exec("INSERT into " + QString::fromStdString(name) + " (TEACHER, NUM_OF_HOURS, VALUES_SUBJECT)"
          " values('" + QString::fromStdString(teacher_name) + "', '" +
               QString::fromStdString(to_string(hours)) + "', '" + val + "');");
    query.exec("SELECT ID from projects WHERE DEPARTMENT = '" + QString::fromStdString(name) + "';");
    query.next();
    num_of_proj = query.value(0).toInt();
    cout << "NUm " << num_of_proj << endl;
    proj = QString::fromStdString(to_string(num_of_proj));
    cout << proj.toStdString() << " ";

    query.exec("SELECT PROJECTS from users WHERE TEACHER = '" + QString::fromStdString(teacher_name) + "';");
    if (query.next())
        proj = query.value(0).toString() + " " + proj;
    cout << proj.toStdString() << endl;
    query.exec("UPDATE users set PROJECTS = '" + proj + "' WHERE NAME = '" +
               QString::fromStdString(teacher_name) + "';");
    return 1;
}


int server::put_in_teacher_subjects(string name, string teacher, vector<string> &subjects){
    QSqlQuery query;
    QString val = "";

    for (auto &el : subjects)
        val += QString::fromStdString(el) + " ";
    query.exec("UPDATE " + QString::fromStdString(name) + " set SUBJECTS = '" + val + "' "
                    "WHERE TEACHER = '" + QString::fromStdString(teacher) + "';");
    if (query.isValid())
        return 1;
    return -1;
}

int server::update_teachers(string name, string teacher_name, vector<int>& values){
    QSqlQuery query;
    QString val = "";
    for (auto &el : values)
        val += QString::fromStdString(to_string(el)) + " ";
    query.exec("UPDATE " + QString::fromStdString(name) + " set VALUES_SUBJECT = '" + val + "'"
        " WHERE TEACHER = '" + QString::fromStdString(teacher_name) + "';");
    if (!query.isValid())
        return -1;
    return 1;
}

void server::update_hours(string name, string teacher_name, int hours){
    QSqlQuery query;
    QString tmp = QString::fromStdString(to_string(hours));
    query.exec("UPDATE " + QString::fromStdString(name) + " set NUM_OF_HOURS = '" + tmp + "'"
        " WHERE TEACHER = '" + QString::fromStdString(teacher_name) + "';");
}

void server::check_db(string name){
    QSqlQuery query;
    query.exec("SELECT * from " + QString:: fromStdString(name) + ";");
    while(query.next()){
        for (int i = 0; i < 4; i++)
            cout << query.value(i).toString().toStdString() << " ";
        cout << endl;
    }
    cout << endl;
}

void server::get_projects(string login, vector<string> &projects){
     vector<int> numbers;
     QSqlQuery query;
     query.exec("SELECT PROJECTS from users WHERE LOGIN = '" + QString::fromStdString(login) + "';");

     while (query.next())
         numbers.push_back(query.value(0).toInt());


     for (auto &el : numbers){
        query.exec("SELECT DEPARTMENT from projects WHERE ID = '" +
                   QString::fromStdString(to_string(el)) + "';");
        query.next();
        projects.push_back(query.value(0).toString().toStdString());
     }
}


void server::get_teachers(string project, vector<string> & teachers){
    QSqlQuery query;
    query.exec("SELECT TEACHER from " + QString::fromStdString(project) + ";");
    while (query.next())
        teachers.push_back(query.value(0).toString().toStdString());
}

void server::get_conditions(string project, vector<string> &conditions){
    QSqlQuery query;
    query.exec("SELECT  NUMBER, NUM_OF_HOURS, VALUES_SUBJECT from "
               + QString::fromStdString(project) + ";");
    while (query.next())
        conditions.push_back((query.value(0).toString() + " "
                          + query.value(1).toString() + " " + query.value(2).toString()).toStdString());
}


void server::get_names(string proj, list<vector<int>> &results,
                       map<int, vector<string>> &to_send){
    QSqlQuery query, query2;
    for (auto &el : results){
        query.exec("SELECT TEACHER from " + QString::fromStdString(proj) + " WHERE NUMBER = '" +
                   QString::fromStdString(to_string(el[1])) + "';");
        query2.exec("SELECT SUBJECT, GROUP_NUM from " + QString::fromStdString(proj) + "_subjects WHERE NUMBER = '" +
                    QString::fromStdString(to_string(el[2])) + "';");
        query.next();
        query2.next();
        vector<string> tmp_vec = {to_string(el[0]),
                                  query2.value(0).toString().toStdString(),
                                  query2.value(1).toString().toStdString(),
                                  query.value(0).toString().toStdString()};
        to_send.insert(make_pair(el[0],tmp_vec));
    }
}

void server::get_teacher_info(string name, int hours,
                              set<string> &subjects, vector<vector<string>>&schedule){
    QSqlQuery query;
    vector<string> tmp_vec;
    query.exec("SELECT PROJECTS from users WHERE NAME = '" + QString::fromStdString(name) + "';");
    if (!query.next())
        return;
    int proj = query.value(0).toInt();
    query.exec ("SELECT DEPARTMENT from projects WHERE ID = '" +
                QString::fromStdString(to_string(proj)) + "';");
    if (!query.next())
        return;

    QString dep = query.value(0).toString();
    query.exec("SELECT NUM_OF_HOURS from " + dep + "WHERE TEACHER = '" +
               QString::fromStdString(name) + "';");
    if (!query.next())
        return;
    hours = query.value(0).toInt();

    query.exec("SELECT SUBJECT_VALUES from " + dep + "WHERE TEACHER = '" +
               QString::fromStdString(name) + "';");
    if (!query.next())
        return;
    string tmp = query.value(0).toString().toStdString(), tmp2, subj;
    stringstream ss(tmp);
    while (ss){
        getline(ss, tmp2, ' ');
        tmp_vec.push_back(tmp2);
        getline(ss, tmp2, ' ');
        query.exec("SELECT SUBJECT from " + dep + "_subjects" + "WHERE ID = '" +
                   QString::fromStdString(tmp2) + "';");
        if (!query.next())
            break;
        subj = query.value(0).toString().toStdString();
        tmp_vec.push_back(subj);
        subjects.insert(subj);
        getline(ss, tmp2, ' ');
        tmp_vec.push_back(tmp2);
    }
}

void server::clear(){
    QSqlQuery query;
    query.exec("DROP users;");
}

void parse(string str, vector<string> &res){
    string tmp;
    str = str.substr(1);
    stringstream ss(str);
    while (ss){
        getline(ss, tmp, ' ');
        if (tmp.find_first_of(":") == string::npos)
            res.push_back(tmp);
    }
}

void parse_proj(string str, vector<vector<string>>& res, string &name){
    auto iter1 = str.find("groups"), iter2 = str.find("teachers"), iter0 = str.find("subjects");
    if (max(iter1, iter2) > str.size())
        return;
    name = str.substr(0, str.find_first_of(" "));
    parse(str.substr(iter0 + 10, iter1), res[0]);
    parse(str.substr(iter1 + 7, iter2), res[1]);
    parse(str.substr(iter2 + 8), res[2]);
}

void filter(map<int, vector<string>> &res, string name){
    for (auto iter = res.begin(); iter != res.end(); ++iter)
        if (iter->second[1] != name)
            res.erase(iter);
}

void server::create(string name, vector<vector<string>> &res){
    int size = res[0].size(), size2 = res[1].size(), size3 = res[2].size();
    vector<pair<string, string>> vec;
    vector<int> values;
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size2; j++)
            vec.push_back(make_pair(res[0][i], res[1][j]));
    put_in_subjects(name, vec);
    for (int i = 0; i < size3; i += 2)
        put_in_teachers(name, res[2][i], stoi(res[2][i + 1]), values);
}

int server::get_number(string proj, string str){
    QSqlQuery query;
    query.exec("SELECT NUMBER from " + QString::fromStdString(proj + "_subjects") +
               "WHERE SUBJECT = '" + QString::fromStdString(str) + "';");
    query.next();
    return query.value(0).toInt();
}

void rebuild(map<int, vector<string>> &vec, vector<vector<vector<string>>> &res){
    int size = vec.size(), size2 = vec[0].size();
    vector<vector<string>> tmp;
    for (int i = 0; i < size; i++){
        tmp.push_back(vec[i % 7]);
        if (i % 7 == 6){
            res.push_back(tmp);
            tmp.clear();
        }
    }
}

int server::get_subjects(string name, set<string> &subjects){
    QSqlQuery query;
    query.exec("SELECT SUBJECT from " + QString::fromStdString(name) + ";");
    while (query.next())
        subjects.insert(query.value(0).toString().toStdString());
}

pair<json, int> server::menu(string str){
    cout << "hi" << endl;
    vector<string> res;
    vector<vector<string>> res_vec;
    switch (str[0]) {
    case 'l':{
        parse(str, res);
        if (res.size() < 2){
            cout << "Bad log in request" << endl;
            //send(sock, "404", 3, 0);
            return make_pair(json{{}}, 404);
        }
        string name;
        int hours;
        set<string> subj;
        vector<string> subjects;
        vector<vector<string>> schedule;
        if (log_in(res[0], res[1], name) > 0){
            get_teacher_info(name, hours, subj, schedule);
            for (auto &el : subj)
                subjects.push_back(el);
            auto iter = name.find_first_of(" ");
            json user = json({
                  {"login", res[0]},
                  {"name",  name.substr(0, iter)},
                  {"surname", name.substr(iter + 1)},
                  {"teacher", "true"},
                  {"fullTime", hours},
                  {"subjects", subjects},
                  {"desiredSchedule", schedule}
            });
            cout << user.dump() << endl;
            return make_pair(user, 200);
            //send(sock, "200", 3, 0);
            //send(sock, user.dump().c_str(), user.dump().size(), 0);
        }
        //else send(sock, "404", 3, 0);
        return make_pair(json{{}}, 404);
    }
    case 'r':
        parse(str, res);
        for (auto &el : res)
            for (auto &el2 : el)
                cout << el2 << " ";
        cout << endl;
        if (res.size() < 4){
            cout << "Bad register request" << endl;
            //send(sock, "409", 3, 0);
            return make_pair(json{{}}, 409);
        }
        if (create_user(res[2], res[3], res[0] + " " + res[1]) > 0){
            json user = json({
                  {"name",  res[0]},
                  {"surname", res[1]},
                  {"login", res[2]},
                  {"teacher", "true"}
            });
            cout << user.dump() << endl;
            //send(sock, "201", 3, 0);
            //send(sock, user.dump().c_str(), user.dump().size(), 0);
            return make_pair(user, 201);
        }
        //else send(sock, "409", 3, 0);
        return make_pair(json{{}}, 409);
    case 'p':{
        string p_name;
        parse_proj(str, res_vec, p_name);
        for (auto &el : res_vec)
            for (auto &el2 : el)
                cout << el2 << " ";
        vector<string> subj_vec;
        if (res_vec.size() < 3){
            cout << "Bad put request" << endl;
            //send(sock, "409", 3, 0);
            return make_pair(json{{}}, 409);
        }

        if (create_project("Hitler", "Tesak", p_name) < 0){
            //send(sock, "409", 3, 0);
            return make_pair(json{{}}, 409);
        }
        create(p_name, res_vec);
        //send(sock, "201", 3, 0);
        return make_pair(json{{}}, 201);
    }
    case 'h':{
        parse(str, res);
        vector<int> val;
        if (res.size() < 2){
            cout << "Bad subj_update request" << endl;
            //send(sock, "500", 3, 0);
            return make_pair(json{{}}, 500);
        }

        int i = 2, size = res.size(), j = 0;
        while (i < size){
            val.push_back(stoi(res[i]) + (j / 7) * 7);
            i++;
            if (stoi(res[i]) == 0){
                val.push_back(get_number(res[0], res[i]));
                i++;
            }
            val.push_back(stoi(res[i]));
            i++;
            j++;
        }

        if (update_teachers(res[0], res[1], val) > 0)
            //send(sock, "200", 3, 0);
            return make_pair(json{{}}, 200);
        //else send(sock, "500", 3, 0);
        else return make_pair(json{{}}, 500);
    }
    case 's':{
        parse(str, res);
        vector<string> s(res.begin() + 2, res.end());
        if (res.size() < 2){
            cout << "Bad update subjects request" << endl;
            //send(sock, "500", 3, 0);
            return make_pair(json{{}}, 500);
        }

        if (put_in_teacher_subjects(res[0], res[1], s) > 0)
            //send(sock, "200", 3, 0);
            return make_pair(json{{}}, 200);
        return make_pair(json{{}}, 500);
    }
    case 'a':{
        parse(str, res);
        set<string> subj;
        if (res.size() == 0){
            cout << "Bad subjects request" << endl;
            //send(sock, "404", 3, 0);
            return make_pair(json{{}}, 500);
        }
        if (get_subjects(res[0], subj) > 0){
            json subj = {
                {"subjects", subj}
            };
            //send(sock, subj.dump().c_str(), subj.dump().size(), 0);
            return make_pair(subj, 200);
        }
        //else send(sock, "404", 3, 0);
        return make_pair(json{{}}, 500);;
    }
    case 't':{
        parse(str, res);
        list<vector<int>> t_sched;
        map<int, vector<string>> to_send;
        vector<vector<vector<string>>> result;
        if (res.size() == 0){
            cout << "Bad schedule request" << endl;
            //send(sock, "204", 3, 0);
            return make_pair(json{{}}, 404);
        }

        data * cur_t = get(this, res[0]);
        if (create_schedule(cur_t, this, t_sched) > 0){
            get_names(res[0], t_sched, to_send);
            filter(to_send, res[0]);
            rebuild(to_send, result);
            json tt = json({
                {"timetable", to_send}
            });
            //send(sock, tt.dump().c_str(), tt.dump().size(), 0);
            return make_pair(tt, 200);
        }
        //else send(sock, "404", 3, 0);
        return make_pair(json{{}}, 404);
    }
    case 'f':{
        parse(str, res);
        list<vector<int>> sched;
        map<int, vector<string>> to_send;
        vector<vector<vector<string>>> result;
        if (res.size() == 0){
            cout << "Bad schedule request" << endl;
            //send(sock, "204", 3, 0);
            return make_pair(json{{}}, 404);
        }

        data * cur = get(this, res[0]);
        if (create_schedule(cur, this, sched) > 0){
            get_names(res[0], sched, to_send);
            rebuild(to_send, result);
            json f = json({
                {"timetable", sched}
            });
            //send(sock, f.dump().c_str(), f.dump().size(), 0);
            return make_pair(f, 200);
        }
        //else send(sock, "404", 3, 0);
        return make_pair(json{{}}, 404);
    }
    default:
        cout << "Bad request type" << endl;
        //send(sock, "404", 3, 0);
        return make_pair(json{{}}, 404);
    }


}
/*
int server::send_message(SOCKET &mySocket, string message){
    return send(mySocket, message.c_str(), message.length(), 0);
}
*/
/*void exec(server * s, int cl_socket){
    int size = 0;
    string str;
    char * buf = new char[1000];
    while (true){
        if (cl_socket != 0)
            size = recv(cl_socket, buf, 1000, 0);
        cout << "Size " << size << endl;
        if (size > 0){
            for (int i = 0; i < size; i++)
                str += buf[i];
            cout << "Message " << str << endl;
            s->menu(str, cl_socket);
        }
    }
}

void exec2(server *s, int cl_socket, int size, char * buf){
    string str;
    cout << "Size " << size << endl;
    if (size > 0){
        for (int i = 0; i < size; i++)
            str += buf[i];
        cout << "Message " << str << endl;
        s->menu(str, cl_socket);
        str = "";
    }
    cout << WSAGetLastError() << endl;
}*/
/*
void server::work(){
    WSACleanup();
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);
    mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mySocket == INVALID_SOCKET){
        cout << "Cannot create socket" << endl;
        return;
    }
    sockaddr_in addr;
    int addr_size = sizeof(addr);
    struct addrinfo hints = {0};
    struct addrinfo *res = NULL;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    sockaddr_in faddr;
    faddr.sin_addr.S_un.S_addr = INADDR_ANY;
    faddr.sin_port = htons(8000);
    faddr.sin_family = AF_INET;
    int inf = getaddrinfo(NULL, "8000", &hints, &res);
    if (inf != 0)
        cout << WSAGetLastError() << endl;
    if (bind(mySocket, (struct sockaddr *)&faddr, sizeof(faddr)) == SOCKET_ERROR)
        cout << WSAGetLastError() << endl;
    if (listen(mySocket, 4) != 0)
        cout << WSAGetLastError() << endl;

    int size = 0;
    string str;
    char * buf = new char[1000];
    while (true){
        cl_socket = accept(mySocket, (struct sockaddr*)&addr, &addr_size);
        if (cl_socket != 0)
            size = recv(cl_socket, buf, 1000, 0);
        cout << size << endl;
        for (int i = 0; i < size; i++)
            cout << buf[i];
        //thread th(exec2, this, cl_socket, size, buf);
    }
    //while (true){

       // thread th(exec, this, cl_socket);
      //  system("pause");
    //}
}*/

/*
void server::work(){
    cout << "hi" << endl;
    QHttpRequest * req;
    QHttpResponse* res;
    QHttpServer *srv = new QHttpServer(this);
    connect(srv, SIGNAL(newRequest(QHttpRequest *, QHttpResponse*)),
            this, SLOT(handleRequest(QHttpRequest *, QHttpResponse*)));
    cout << "hehe" << endl;
    srv->listen(8000);
}


void server::handleRequest(QHttpRequest * req, QHttpResponse * resp){
    cout << "hi" << endl;
    pair<json, int> result = menu(req->body().toStdString());
    QByteArray arr;
    for (auto &el : result.first.dump())
        arr.append(el);
    resp->writeHead(result.second);
    resp->write(arr);
    resp->end();
}
*/
void parse(vector<string> &str, vector<vector<int>> &values, vector<int> &hours){
    string tmp;
    int i;
    vector<int> vec(4);
    for (auto &el : str){
        i = 0;
        stringstream ss(el);
        getline(ss, tmp, ' ');
        vec[1] = stoi(tmp);
        getline(ss, tmp, ' ');
        hours.push_back(stoi(tmp));
        while(ss) {
            getline(ss, tmp, ' ');
            if (tmp == "" || tmp == " ")
                break;

            if (i % 3 == 0){
                vec[0] = stoi(tmp);
                i++;
            }
            else if (i % 3 == 1){
                vec[2] = stoi(tmp);
                i++;
            }
            else if (i % 3 == 2){
                vec[3] = stoi(tmp);
                i++;
            }
            if (i % 3 == 0 && i != 0)
                values.push_back(vec);

        }
        i = 0;
    }


}

data * get(server *s, string name){
    data * cur = new data();
    vector<string> cond;
    vector<int> hours;
    vector<vector<int>> values;
    int s1 = 10, s2 = 3, s3 = 7, size, check;
    cur->autofill(s1, s2, s3);
    s->get_conditions(name, cond);
    parse(cond, values, hours);
    size = values.size();
    try{
        for (int i = 0; i < size; i++){
            check = cur->set_values(values[i][0], values[i][1], values[i][2], values[i][3]);
            if (check < 0) throw check;
        }
    }
    catch(int val){
        cout << "Wrong index " << -val << " inputed" << endl;
        return nullptr;
    }
    cur->set_global_values(hours);
    return cur;
}

data * get2(server * s){
    int a, b, c, d, check, s1 = 42, s2 = 40, s3 = 400;
    int * val = new int[s2];
    data * cur = new data();
    cur->autofill(s1, s2, s3);
    vector<int> estims = {-100, -100, -100, -1, -1, -1};
    try{
        for (int i = 0; i < 7000; i++){
            a = rand() % s1;
            b = rand() % s2;
            c = rand() % s3;
            d = rand() % 6;
            check = cur->set_values(a, b, c, estims[d]);
            if (check < 0) throw check;
        }
    }
    catch(int val){
        cout << "Wrong index " << -val << " inputed" << endl;
        return nullptr;
    }
    for (int i = 0; i < s2; i++)
        val[i] = 10;
    //cur->set_global_values(val, s2);
    return cur;
}



int create_schedule(data * cur, server * s, list<vector<int>> &res){
    int estim = -INT_MAX, edge = _EDGE, i = 0;
    cur->generate(res);
    cur->print(res);
    while (estim < edge){
        estim = cur->eval_estim(res);
        cout << i << " Estim " << estim << endl;
        cur->regression(res);
        //system("pause");
        cur->print(res);
        ++i;
        //if (i > num) ++edge;
        if (i > 1.e5)
            return -1;
    }
    cur->print(res);
    //system("pause");
    return 1;
}


///log in : user
/// post change : ok
/// hours : ok
/// get schedule : schedule
///
/// l
/// r
/// p proj
/// s subj
/// h pref
/// t get tt
/// f full

