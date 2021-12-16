#include <iostream>
#include <ncurses.h>
#include <string>
#include <vector>
#include <sqlite3.h> 
using std::cout; using std::cin; using std::endl; using std::cerr;

class field
{
    std::string colname;
    std::string value;

    public:
    field(std::string c, std::string v)
    {
        this->colname = c; 
        this->value = v;
    }
    std::string& get_colname() {return this->colname;}
    std::string& get_value() {return this->value;}
};

class row
{   
    friend class table;
    std::vector<field> fields = {};

    public:
    std::vector<field>& get_fields() {return this->fields;}
    
};

class column
{
    int id;
    std::string name;
    std::string type;
    int notnull;
    std::string dflt_value;
    int pk;
    
    public:
    column(int id, std::string name, std::string type, int notnull, std::string dflt_value, int pk)
    {
        this->id = id;
        this->name = name;
        this->type = type;
        this->notnull = notnull;
        this->dflt_value = dflt_value;
        this->pk = pk;
    }
    std::string& get_name() {return this->name;}
    std::string& get_type() {return this->type;}
    int& get_id() {return this->id;}
    int& get_notnull() {return this->notnull;}
    int& get_pk() {return this->pk;}
    std::string& get_dflt_value() {return this->dflt_value;}



};

class relation
{
    std::string table1, table2, col1, col2, on_update, on_delete, match;
    int id;
    int seq;

    
    public:
    relation(std::string table1, int id, int seq, std::string table2, std::string col1, std::string col2, std::string on_update, std::string on_delete, std::string match)
    {
        this->table1 = table1;
        this->table2 = table2;
        this->col1 = col1;
        this->col2 = col2;
        this->id = id;
        this->seq = seq;
        this->on_update = on_update;
        this->on_delete = on_delete;
        this->match = match;

        
    }
    std::string& get_table1() {return table1;}
    std::string& get_table2() {return table2;}
    std::string& get_col1() {return col1;}
    std::string& get_col2() {return col2;}
    int& get_id() {return id;}
    int& get_seq() {return seq;}
    std::string& get_on_update() {return on_update;}
    std::string& get_on_delete() {return on_delete;}
    std::string& get_match() {return match;}
};

class table
{   
    friend class database;
    friend class sse;
    
    std::string name;
    std::vector<row> rows;
    std::vector<std::string> colnames = {};
    bool are_colnames = false;
    
    std::vector<column> columns = {};
    
    public:

    table(std::string x) {this->name = x;}
    
    std::vector<std::string>& get_colnames() {return this->colnames;}
    std::string& get_name() {return this->name;}
    std::vector<row>& get_rows() {return this->rows;}
    std::vector<column>& get_columns() {return this->columns;}
    
    void columns_done() {this->are_colnames = true;}
    void columns_reset() {this->are_colnames = false;}
    bool& get_col_status() {return this->are_colnames;}

    void add_colname(std::string name)
    {
        if (name.empty()) {name = "-";}
        this->colnames.push_back(name); 
    }
    void add_row() {this->rows.push_back(row());}
    void add_field(field f) {this->rows[rows.size()-1].get_fields().push_back(f);}
    void print_colnames()
    {   
        
        printw("kolumny\n");
        //printw("size = %d", this->colnames.size());
        for (auto &n : this->colnames) {printw("%s ",n.c_str());}
        printw("\n");
        for (auto &r : this->rows)
        {
            printw("\n");

            for (auto &f : r.get_fields()) {printw("%s ", f.get_value().c_str());}
        }
        printw("\n");

    }
    void resetitems()
    {
        are_colnames = false;
        rows = {};
        colnames = {};
        columns = {};
    }
    std::vector<int> maxlengths()
    {
        std::vector<int> maxlengths;
        int sum = 1;
        for (int i = 0; i<columns.size(); ++i)
        {
            int temp = columns[i].get_name().length();
            for (int j = 0; j<rows.size(); ++j)
            {
                if (rows[j].get_fields()[i].get_value().size() > temp)
                {
                    temp = rows[j].get_fields()[i].get_value().length();
                    
                }
                //cout<<rows[j].get_fields()[i].get_value().length()<<" ";
            }
            sum+=temp+1;
            maxlengths.push_back(sum);
            //cout<<endl;
        }
        return maxlengths;
    }
    
};
class database
{   
    friend class table;
    friend class sse;
    
    table *current_table = &tables[0];
    std::vector<table> tables = {};

    std::vector<relation> relations = {};
    
    public:
    
    std::vector<relation>& get_relations() {return relations;}
    table& get_current_table() {return *this->current_table;}
    void set_current_table(std::string t_name) 
    {
        for (auto& t: this->tables)
        {
        if (t_name == t.get_name()) {this->current_table = &t;}
        }
    }
    void set_current_table(int id)
    {   
        
        this->current_table = &(this->tables[id]);
    } 
    bool& get_current_table_arecolnames() {return this->current_table->get_col_status() ;}
    void ct_columns_done() {this->current_table->columns_done(); }
    void print_current_table(table t) {cout<<"wskazuje na "<<this->current_table->get_name().c_str()<<endl;}
    std::vector<table>& gettables() {return this->tables;}

    table& gettable(std::string name)
    {
        
        for (auto &t : this->gettables()) 
        {
            if (t.get_name()==name)
            {
                return t;

            }
        }
        
    }
    table& gettable(int id)
    {
        return tables[id];
    }

    void addtable(table t) {this->tables.push_back(t);}
    
    int callback(int argc, char **argv, char **azColName)
    {
        for (int i = 0; i < argc; i++)
        {   //cout<<"xdd"<<endl;
            this->tables.push_back(table(argv[i]));
        }
        return 0;
    }
    void printtables()
    {
        printw("tablice = %d\n", tables.size());
        //cout<<"tablice = "<<tables.size()<<endl;
        //cout<<tables[0].get_name()<<endl;
        for (auto &t : this->gettables()) 
        {
        
            //cout<<t.get_name()<<endl;
            printw("%s\n", t.get_name().c_str());
        }
    }
    void reset()
    {
        this->current_table = &tables[0];
        tables = {};
        relations = {};
    }
    int tables_length()
    {
        int max = 0;
        for (auto &t : this->tables)
        {
            if (t.get_name().size() > max) {max = t.get_name().size();}
        }
        return max;
    }
    
    int relations_length()
    {
        int max = 0;
        int temp;
        for (auto &r : this->relations)
        {
            temp = r.get_table1().size()+r.get_table2().size()+7;
            if (temp > max) {max = temp;}
            temp = r.get_col1().size()+r.get_col2().size()+7;
            if (temp > max) {max = temp;}
        }
        return max+2;
    }

    std::vector<int> maxlengths()
    {
        std::vector<int> maxlengths;
        int sum = 1;
        for (int i = 0; i<tables.size(); ++i)
        {
            int temp = tables[i].get_name().size();
            for (int j = 0; j<tables[i].get_columns().size(); ++j)
            {
                if (tables[i].get_columns()[j].get_name().size() + tables[i].get_columns()[j].get_type().size() +3 > temp)
                {
                    temp = tables[i].get_columns()[j].get_name().size() + tables[i].get_columns()[j].get_type().size() +3;
                    
                }
                //cout<<rows[j].get_fields()[i].get_value().length()<<" ";
            }
            sum+=temp+1;
            maxlengths.push_back(sum);
            //cout<<endl;
        }
        return maxlengths;
    }
    int max_columns()
    {
        int maxcols = 0;
        for (auto &t : tables)
        {
            if (t.get_columns().size() > maxcols) { maxcols = t.get_columns().size(); }
        }
        return maxcols;
    }
};
database my_db;