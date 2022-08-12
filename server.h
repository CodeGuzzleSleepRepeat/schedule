#ifndef SERVER_H
#define SERVER_H
#include <QCoreApplication>
#include <QtSql/QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <iostream>
#include <vector>
#include <string>
#include <QString>
#include "json.hpp"
#include "objects.h"
#include <set>
#include <sstream>
#include <qhttpserverfwd.h>
#include <qhttpserver.h>
#include <qhttprequest.h>
#include <qhttpresponse.h>
//#include <QHostAddress>
#include <QByteArray>
#include "C:/Qt/Qt5.8.0/Tools/sqlite/sqlite/sqlite3.h"

#define _EDGE 0

using json = nlohmann::json;
using namespace std;

class server : public QObject{
    Q_OBJECT
    QSqlDatabase db_users;
private slots:
    void handleRequest(QHttpRequest*, QHttpResponse*);
public:
    server();
    int create_user(string, string, string);
    void create(string, vector<vector<string>>&);
    int log_in(string, string, string&);
    void log_out(string);
    int create_project(string, string, string);
    void put_in_subjects(string, vector<pair<string, string>> &);
    int put_in_teachers(string, string, int, vector<int> &);
    int put_in_teacher_subjects(string, string, vector<string> &);
    int update_teachers(string, string, vector<int> &);
    int get_number(string, string);
    void update_hours(string, string, int);
    void check_db(string);
    int get_subjects(string, set<string> &);
    void get_teacher_info(string, int, set<string> &, vector<vector<string>>&);
    void get_projects(string, vector<string> &);
    void get_teachers(string, vector<string>&);
    void get_conditions(string, vector<string>&);
    void get_names(string, list<vector<int>> &, map<int, vector<string>>&);
    void clear();
    //int send_message(SOCKET&, string);
    pair<json, int> menu(string);
    void work();
};



void parse(vector<string> &str, vector<vector<int>> &values, vector<int> &hours);
data * get(server *s, string name);
int create_schedule(data * cur, server * s, list<vector<int>>&);

#endif // SERVER_H
