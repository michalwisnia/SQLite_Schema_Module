#include <iostream>
#include <ncurses.h>
#include <string>
#include <vector>
//#include "../backend.h"
//#include "../frontend.h"
#include <sqlite3.h> 
#include "sqlite.h"
#include <numeric>


using std::cout; using std::cin; using std::endl; using std::cerr;

static int callback_tables(void* NotUsed, int argc, char** argv, char** azColName)
    {
        //database* mydb = reinterpret_cast<database*>(NotUsed);
        //return mydb->callback(argc, argv, azColName);

        for (int i = 0; i < argc; i++)
        {
            table t = table(argv[i]);
            my_db.addtable(t);
        }

        return 0;
    }
static int callback_items(void* NotUsed, int argc, char** argv, char** azColName)
    {   
        my_db.get_current_table().add_row();
        for (int i = 0; i < argc; i++)
        {
            if (my_db.get_current_table_arecolnames() == false)
            {   
                //printw("%s", my_db.get_current_table().get_name().c_str());

                my_db.get_current_table().add_colname(azColName[i]? azColName[i] : "NULL" );

                //printw("kn = %s\n",azColName[i]);               
            }
            my_db.get_current_table().add_field(field(azColName[i]? azColName[i] : "NULL",argv[i]? argv[i] : "NULL"));

        }
        
        my_db.ct_columns_done();
        return 0;
    }
static int callback_columns(void* NotUsed, int argc, char** argv, char** azColName)
{
    /*for (int i = 0; i < argc; i++)
    {   
        printw("%s: %s\n", azColName[i], argv[i]);
        
    }
    printw("\n");*/

    my_db.get_current_table().get_columns().push_back(column(atoi(argv[0]), argv[1], argv[2], atoi(argv[3]), argv[4]?  argv[4] : "NULL", atoi(argv[5])));
    return 0;

}
static int callback_relations(void* NotUsed, int argc, char** argv, char** azColName)
{
    for (int i = 0; i < argc; i++)
    {   
        //printw("%s: %s\n", azColName[i], argv[i]);
        
    }
    //printw("\n");

    my_db.get_relations().push_back(relation(argv[0], atoi(argv[1]), atoi(argv[2]), argv[3], argv[4], argv[5], argv[6], argv[7], argv[8]));
    
    return 0;
}
class sse
{
    
    sqlite3 *db;
    bool opened = false;
    bool modified = false;


    public:
    void relations()
    {   
        char* ErrMsg;
        
        std::string get_relations = "SELECT m.name , p.* FROM sqlite_master m JOIN pragma_foreign_key_list(m.name) p ON m.name != p.'table' WHERE m.type = 'table' ORDER BY m.name ;";
        int load_relations = sqlite3_exec(db, get_relations.c_str(), callback_relations, NULL, NULL);
        if (load_relations != SQLITE_OK)
        {
            cerr<<"Editing command didnt work\n"<<endl;
            fprintf(stderr, "SQL error: %s\n", ErrMsg);
            sqlite3_free(ErrMsg);
        }
        
    }


       
    void open(const char *name)
    {   
        
        int correct = 1;
        int opening = sqlite3_open(name, &db);


        char* error_msg;

        
       
        std::string squl = "SELECT * FROM MYTAB;";
        //printw("Tablice: \n");
        std::string command = "SELECT name FROM sqlite_master WHERE type='table'";

        int load_tables = sqlite3_exec(db, command.c_str(), callback_tables, NULL, NULL);     
        
        if (load_tables == SQLITE_OK)
        {
            //cout<<"database displayed successfully\n";
            for (auto &t : my_db.gettables())
            {
                my_db.set_current_table(t.get_name());
                std::string command = "SELECT * FROM ";
                command+=t.get_name();
                //printw("Elementy tablicy %s: \n", table.c_str());
                int load_items = sqlite3_exec(db, command.c_str(), callback_items, NULL, NULL);
                if (load_items != SQLITE_OK)
                {
                    cerr<<"Error loading items\n"<<endl;
                    fprintf(stderr, "SQL error: %s\n", error_msg);
                    sqlite3_free(error_msg);
                }
                command = "PRAGMA table_info(" + t.get_name() +");";
                int load_columns = sqlite3_exec(db, command.c_str(), callback_columns, NULL, NULL);
                if (load_items != SQLITE_OK)
                {
                    cerr<<"Error loading columns\n"<<endl;
                    fprintf(stderr, "SQL error: %s\n", error_msg);
                    sqlite3_free(error_msg);
                }
            }
            
        }
        else 
        {
            cerr<<"error displaying database\n";
            fprintf(stderr, "SQL error: %s\n", error_msg);
            sqlite3_free(error_msg);
        }
        std::string get_relations = "SELECT m.name , p.* FROM sqlite_master m JOIN pragma_foreign_key_list(m.name) p ON m.name != p.'table' WHERE m.type = 'table' ORDER BY m.name ;";
        int load_relations = sqlite3_exec(db, get_relations.c_str(), callback_relations, NULL, NULL);
        if (load_relations != SQLITE_OK)
        {
            cerr<<"Editing command didnt work\n"<<endl;
            fprintf(stderr, "SQL error: %s\n", error_msg);
            sqlite3_free(error_msg);
        }
        my_db.set_current_table(0);
        
    }
    

    void edit_item(std::string table, std::string colname, int rowid, std::string nvalue)
    {
        
        char *zErrMsg;
        std::string command = ("UPDATE " + table + " set '" + colname + "' = '" + nvalue + "' where " + "rowid"  + " = " + std::to_string(rowid) + "; ");
        //printw("%s\n",command.c_str());
        int display_table = sqlite3_exec(db, command.c_str(), NULL, 0, &zErrMsg);
        if (display_table != SQLITE_OK)
        {
            cerr<<"Editing command didnt work\n"<<endl;
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        else
        {
            //cout<<"Editing command was applied\n"<<endl;
        }



    }
    void edit_table(std::string table, std::string nvalue)
    {
        char *zErrMsg;
        std::string command = ("ALTER TABLE '" + table + "' RENAME TO '" + nvalue + "'; ");
        int display_table = sqlite3_exec(db, command.c_str(), callback_items, 0, &zErrMsg);
        if (display_table != SQLITE_OK)
        {
            cerr<<"Editing command didnt work\n"<<endl;
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        else
        {
            //cout<<"Editing command was applied\n"<<endl;
        }
    }
    void delete_table(std::string table)
    {
        char *zErrMsg;
        std::string command = ("DROP TABLE " + table + "; ");
        int display_table = sqlite3_exec(db, command.c_str(), NULL, 0, &zErrMsg);
        if (display_table != SQLITE_OK)
        {
            cerr<<"Editing command didnt work\n"<<endl;
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }

    void add_table()
    {
        char *zErrMsg;
        std::string command = "CREATE TABLE IF NOT EXISTS TABLE_" + std::to_string(static_cast<int>(my_db.gettables().size())) + "(COL_1);";
        //printw("%s\n", command.c_str());
        int display_table = sqlite3_exec(db, command.c_str(), NULL, 0, &zErrMsg);
        if (display_table != SQLITE_OK)
        {
            cerr<<"Editing command didnt work\n"<<endl;
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }

    void edit_colname(std::string table, std::string colname, std::string nvalue)
    {
        char *zErrMsg;
        std::string command = ("ALTER TABLE " + table + " RENAME COLUMN '" + colname + "' to '" + nvalue + "'; ");
        //cout<<command<<endl;
        int display_table = sqlite3_exec(db, command.c_str(), callback_items, 0, &zErrMsg);
        if (display_table != SQLITE_OK)
        {
            cerr<<"Editing command didnt work\n"<<endl;
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        else
        {
            //cout<<"Editing command was applied\n"<<endl;
        }
    }
    void add_column(std::string table)
    {
        char *zErrMsg;
        std::string command = "ALTER TABLE " + table + " ADD COLUMN 'COLUMN_" + std::to_string(static_cast<int>(my_db.get_current_table().get_columns().size())) + "' TEXT;";
        int display_table = sqlite3_exec(db, command.c_str(), NULL, 0, &zErrMsg);
        if (display_table != SQLITE_OK)
        {
            cerr<<"Editing command didnt work\n"<<endl;
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }
    void delete_column(std::string table, std::string colname)
    {
        char *zErrMsg;
        std::string new_columns;
        for (auto c: my_db.get_current_table().get_columns())
        {
            if (c.get_name() != colname)
            {
                new_columns += c.get_name();
                new_columns += ",";
            }
        }
        new_columns = new_columns.substr(0, new_columns.size()-1);
        move(30,0);
        std::string command = "BEGIN TRANSACTION; CREATE TEMPORARY TABLE t1_backup(" + new_columns + "); INSERT INTO t1_backup SELECT " + new_columns + " FROM " + table + " ; DROP TABLE " + table + "; CREATE TABLE " + table + "(" + new_columns + "); INSERT INTO "+ table + " SELECT " + new_columns + " FROM t1_backup; DROP TABLE t1_backup; COMMIT;" ;
        //cout<<command<<endl;
        int display_table = sqlite3_exec(db, command.c_str(), NULL, 0, &zErrMsg);
        if (display_table != SQLITE_OK)
        {
            cerr<<"Editing command didnt work\n"<<endl;
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }
    void add_row(std::string table)
    {
        char *zErrMsg;
        std::string command = "INSERT INTO " + table + " DEFAULT VALUES;";
        int display_table = sqlite3_exec(db, command.c_str(), NULL, 0, &zErrMsg);
        if (display_table != SQLITE_OK)
        {
            cerr<<"Editing command didnt work\n"<<endl;
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }
    void delete_row(std::string table, int rowid)
    {
        char *zErrMsg;
        std::string command = "DELETE FROM " + table + " where rowid = " + std::to_string(rowid) + " ";
        int display_table = sqlite3_exec(db, command.c_str(), NULL, 0, &zErrMsg);
        if (display_table != SQLITE_OK)
        {
            cerr<<"Editing command didnt work\n"<<endl;
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }
    void add_relation(std::string table1, std::string column1, std::string table2, std::string column2)
    {
        char *zErrMsg;
        std::string columns = "";
        for (auto c : my_db.gettable(table1).get_columns())
        {
            columns += c.get_name();
            columns += ",";
        }
        columns = columns.substr(0, columns.size()-1);
        std::string command = "BEGIN TRANSACTION; CREATE TEMPORARY TABLE t1_backup(" + columns + "); INSERT INTO t1_backup SELECT " + columns + " FROM " + table1 + " ; DROP TABLE '" + table1 + "'; CREATE TABLE '" + table1 + "'(" + columns + ", FOREIGN KEY (" + column1 + ")" + " REFERENCES " + table2 + " (" + column2 + ") );" + "INSERT INTO "+ table1 + " SELECT " + columns + " FROM t1_backup; DROP TABLE t1_backup; COMMIT;";
        //cout<<command<<endl;
        int display_table = sqlite3_exec(db, command.c_str(), NULL, 0, &zErrMsg);
        if (display_table != SQLITE_OK)
        {
            cerr<<"Editing command didnt work\n"<<endl;
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }
    void delete_relation(std::string table1, std::string column1, std::string table2, std::string column2)
    {
        char *zErrMsg;
        std::string columns = "";
        for (auto c : my_db.gettable(table1).get_columns())
        {
            columns += c.get_name();
            columns += c.get_type();
            columns += ",";
        }
        columns = columns.substr(0, columns.size()-1);
        std::string command = "BEGIN TRANSACTION; CREATE TEMPORARY TABLE t1_backup(" + columns + "); INSERT INTO t1_backup SELECT " + columns + " FROM " + table1 + " ; DROP TABLE '" + table1 + "'; CREATE TABLE '" + table1 + "'(" + columns + ");" + "INSERT INTO "+ table1 + " SELECT " + columns + " FROM t1_backup; DROP TABLE t1_backup; COMMIT;";
        //cout<<command<<endl;
        int display_table = sqlite3_exec(db, command.c_str(), NULL, 0, &zErrMsg);
        if (display_table != SQLITE_OK)
        {
            cerr<<"Editing command didnt work\n"<<endl;
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }
   
};

int main(int argc, char *argv[])
{   
    if (argc < 2)
    {
        cout<<"Podaj nazwę pliku";
        return -1;
        
    }
    initscr();
    raw();
    noecho();
    curs_set(0);
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    
    int mode = 1;
    
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    
    int w_start_y;
    int w_start_x;
    sse editor;
    char* db_filename = argv[1];
    editor.open(db_filename);
    int key;
    WINDOW * database_window;
    WINDOW * table_pad;
    bool exit = false;
    bool edit = false;
    std::string buffer;
    int cur_table = 0;
    do{
        
        move(1, xMax/2-11);
        printw("SQLITE-SCHEMA-EDITOR\n");
    switch (mode)
    {

        case 0:
        {
            move(2, xMax/2-19);
            printw("ENTER A NAME OF SQLITE DATABASE FILE\n");
            refresh();           
            break;
        }


        case 1:
        {   
            //cout<<rc<<"\n";
            //editor.display_items("MYTAB");
            //editor.display();
            //editor.edit_item("MYTAB","PO","2","nowawartosc");
            //editor.display_items("MYTAB");

            
            int tables_sy = 0;
            int tables_sx = 0;
            
            do {
                getmaxyx(stdscr, yMax, xMax);    
                move(6, xMax/2-7);
                printw("DATABASE %s", db_filename);
                move(7, xMax/2-7);
                printw("%d TABLES:", my_db.gettables().size());
                int menu_y = 1;
                int menu_x = xMax - 22;
                move(menu_y++, menu_x);
                printw("ENTER - view table");
                move(menu_y++, menu_x);
                printw("e - rename table");
                move(menu_y++, menu_x);
                printw("a - add table");
                move(menu_y++, menu_x);
                printw("DELETE - delete table");
                move(menu_y++, menu_x);
                printw("Q - quit table");
                move(menu_y++, menu_x);
                printw("TAB - relations mode");   

                //WINDOW * database_window = newwin(yMax-10, xMax-10, w_start_y, w_start_x);
                database_window = newpad(my_db.gettables().size()+2, my_db.tables_length()+2);
                box(database_window,0,0);
                keypad(database_window, true);    
                int tables_y = 1;
                my_db.set_current_table(0);
                int i = 0;
                w_start_y = 8;
                w_start_x = xMax/2 - my_db.tables_length()/2;
                for (auto &t : my_db.gettables()) 
                {   
                    
                    if (i==cur_table) {wattron(database_window, A_STANDOUT);}
                    wmove(database_window, tables_y, 1);
                    if (i==cur_table && edit==true)
                    {
                        
                        
                        wprintw(database_window, "%s", buffer.c_str());
                        //for (int s = buffer.size(); s < t.get_name().size(); ++s) {wprintw(database_window, " ");}
                    }
                    else
                    {                  
                        wprintw(database_window, "%s", t.get_name().c_str());
                    }
                    
                    wattroff(database_window, A_STANDOUT);

                    ++tables_y; ++i;
                }
                refresh();
                
                prefresh(database_window, tables_sy, tables_sx, w_start_y, w_start_x, yMax-2, xMax-2);
                key = wgetch(database_window);
                
                if (edit==true)
                {
                    switch (key)
                    {
                    case KEY_DC:
                    {
                        edit = false;
                        buffer.clear();
                        cur_table--;
                        break;
                    }
                    case KEY_BACKSPACE:
                    {
			            if (buffer.length() > 0) {
                        buffer = buffer.substr(0, buffer.size() - 1);}
                        break;
                    }
                    case 10:
                    {
                        my_db.reset();
                        editor.edit_table(my_db.gettables()[cur_table].get_name(), buffer);
                        editor.open(db_filename);
                        edit = false;
                        buffer.clear();
                        break;
                    }
                    default:
                    {
                        buffer+=key;
                        break;
                    }
                    }
                    
                }
                else
                {
                    switch(key)
                    {
                    case KEY_UP:
                    {
                        cur_table--;
                        if (cur_table < 0) {cur_table++;}
                        if (cur_table < tables_sy) {--tables_sy;}
                        break;
                    }
                    case KEY_DOWN:
                    {
                        cur_table++;
                        if (cur_table == my_db.gettables().size()) {cur_table--;}
                        if ((cur_table >= (yMax-2-w_start_y)) && (tables_sy <= my_db.gettables().size()-(yMax-2-w_start_y))) {++tables_sy;}
                        break;
                    }
                    case 10:
                    {
                        my_db.set_current_table(cur_table);
                        mode = 2;
                        break;
                    }
                    case 'Q':
                    {
                        exit=true;
                        break;
                    }
                    case 'e':
                    {
                        edit = true;
                        break;
                    }
                    case KEY_DC:
                    {
                        editor.delete_table(my_db.gettables()[cur_table].get_name());
                        my_db.reset();
                        editor.open(db_filename);
                        break;

                    }
                    case 'a':
                    {
                        
                        editor.add_table();
                        my_db.reset();
                        editor.open(db_filename);
                        break;
                    }
                    case '\t':
                    {
                        mode = 3;   
                        break;
                    }
                    default:
                    {
                        break;
                    }

                    }  
                }
                
                i=0;
                move(2,1);
                clrtobot();
                wclear(database_window);
                refresh();
                
                
            } while ((mode == 1) && (exit == false));
            
            break;
        }
        
        case 2:
        {
            std::string mode2_header;
            mode2_header = "TABLE " + my_db.get_current_table().get_name() + " FROM DATABASE " + db_filename;           
            int cur_row=0;
            int cur_col=0;
            bool info = false;
            int table_sy = 0;
            int table_sx = 0;
            int table_startcol = 0;
            refresh();
            
            do 
            {   
                if (info == true)
                {
                    move(2,2);
                    printw("COLUMN DETAILS:\n  type: - %s\n  id = %s\n  notnull = %s\n  dflt_value = %s\n  pk = %s", my_db.get_current_table().get_columns()[cur_col].get_type().c_str(), std::to_string(my_db.get_current_table().get_columns()[cur_col].get_id()).c_str(), std::to_string(my_db.get_current_table().get_columns()[cur_col].get_notnull()).c_str(), my_db.get_current_table().get_columns()[cur_col].get_dflt_value().c_str(), std::to_string(my_db.get_current_table().get_columns()[cur_col].get_pk()).c_str() );
                }

                getmaxyx(stdscr, yMax, xMax);  
                move(6, xMax/2-11);
                printw("%s", mode2_header.c_str());
                int menu_y = 1;
                int menu_x = xMax-22;
                if (edit == true)
                {
                    move(menu_y++, menu_x);
                    printw("DELETE - Quit editing");
                    move(menu_y++, menu_x);
                    printw("ENTER - Finish editing");
                }
                else
                {
                    move(menu_y++, menu_x);
                    printw("e - edit item");
                    move(menu_y++, menu_x);
                    printw("i - view add. info");
                    move(menu_y++, menu_x);
                    printw("a - add row");
                    move(menu_y++, menu_x);
                    printw("A - add column");
                    move(menu_y++, menu_x);
                    if (cur_row == 0) {printw("DELETE - delete column");}
                    else printw("DELETE - delete row\n");
                    move(menu_y++, menu_x);
                    printw("Q - quit table");
                    move(menu_y++, menu_x);
                    printw("TAB - relations mode");                  
                }
                
                w_start_x = 2;
                w_start_y = 8;
                
                std::vector<int> columns = my_db.get_current_table().maxlengths();
                auto vec = columns.begin();
                table_pad = newpad(my_db.get_current_table().get_rows().size()+3, columns.back());
                box(table_pad,0,0);
                keypad(table_pad, true);
                int cur_y =  1;
                int y = 0; int x = 0;
                int cur_x = 1;
                wmove(table_pad,cur_y,cur_x);
                wattron(table_pad, COLOR_PAIR(1));
                for (auto &c : my_db.get_current_table().get_columns())
                {   

                    
                    //wprintw(table_pad," %d,%d ", cur_y, cur_x);
                    if (y == cur_row && x == cur_col) {wattron(table_pad, A_STANDOUT);}
                    if (y == cur_row && x == cur_col && edit == true)
                    {
                        wprintw(table_pad, "%s", buffer.c_str());
                    }
                    else
                    {
                        wprintw(table_pad, "%s",c.get_name().c_str());
                    }
                    
                    cur_x = *vec;
                    vec++;
                    wmove(table_pad, cur_y, cur_x);
                    wattroff(table_pad, A_STANDOUT);
                    x++;
                }
                wattroff(table_pad, COLOR_PAIR(1));
                y++;
                x=0;
                
                wmove(table_pad, ++cur_y, 1);


                for(auto &r : my_db.get_current_table().get_rows())
                {   
                    vec = columns.begin();
                    cur_x = 1;
                    for (auto &f : r.get_fields())
                    {   
                        wmove(table_pad, cur_y, cur_x);
                        //cout<<"return = "<<wm<<" "<<cur_y<<" "<<cur_x<<endl;
                        
                        //wprintw(table_pad,"%d,%d ", cur_y, cur_x);
                        
                        //wprintw(table_pad, "");
                        if (y == cur_row && x == cur_col) {wattron(table_pad, A_STANDOUT);}
                        if (y == cur_row && x == cur_col && edit == true)
                        {
                            wprintw(table_pad, "%s", buffer.c_str());
                              
                        }
                        else
                        {
                            wprintw(table_pad, "%s", f.get_value().c_str());                   
                        }
                        cur_x = *vec;
                        vec++;
                          
                        wattroff(table_pad, A_STANDOUT);
                        x++;
                    }

                    wmove(table_pad, ++cur_y, w_start_x+1);
                    y++;
                    x=0;
                }

                if (table_startcol > 0) {table_sx = columns[table_startcol-1];}
                else table_sx = 0;
                refresh();
                wrefresh(table_pad);
                prefresh(table_pad, table_sy, table_sx, w_start_y, w_start_x, yMax-2, xMax-2);
                
                key = wgetch(table_pad);

                if (edit==true)
                {
                    switch (key)
                    {
                    case KEY_DC:
                    {
                        edit = false;
                        buffer.clear();
                        break;
                    }
                    case KEY_BACKSPACE:
			        {    
                        if (buffer.length() > 0) {
                        buffer = buffer.substr(0, buffer.size() - 1);}
                        break;
                    }
                    case 10:
                    {
                        if (cur_row == 0)
                        {
                            editor.edit_colname(my_db.get_current_table().get_name(), my_db.get_current_table().get_columns()[cur_col].get_name(), buffer);
                        }
                        else
                        {

                            editor.edit_item(my_db.get_current_table().get_name(), my_db.get_current_table().get_columns()[cur_col].get_name(), cur_row, buffer);

                        }
                        
                        my_db.reset();
                        editor.open(db_filename);
                        my_db.set_current_table(cur_table);
                        edit = false;
                        buffer.clear();
                        break;
                    }
                    default:
                    {
                        buffer+=key;
                        break;
                    }

                    }
                }
                else
                {
                    switch (key)
                    {
                    case KEY_UP:
                    {
                        cur_row--;
                        if (cur_row < 0) {cur_row++; }

                        if (cur_row < table_sy) {--table_sy;}
                        break;
                    }
                    case KEY_DOWN:
                    {
                        cur_row++;
                        if (cur_row >= my_db.get_current_table().get_rows().size()+1) {cur_row--; }

                        if ((cur_row >= (yMax-2-w_start_y)) && (table_sy <= (my_db.get_current_table().get_rows().size()+1-(yMax-2-w_start_y)))) {++table_sy;}
                        break;
                    }
                    case KEY_LEFT:
                    {
                        cur_col--;
                        if (cur_col < 0) {cur_col++; }

                        while (columns[cur_col-1] < table_sx) 
                        {
                            table_startcol--; 
                            table_sx = columns[table_startcol-1];
                        }
                        break;
                    }
                    case KEY_RIGHT:
                    {
                        cur_col++;
                        if (cur_col >= my_db.get_current_table().get_columns().size()) {cur_col--;}
                        while (columns[cur_col] > table_sx + xMax-2-w_start_x)
                        {
                            table_startcol++; 
                            table_sx = columns[table_startcol-1];
                        }


                        break;
                    }
                    case 'i':
                    {
                        info = !info;
                        break;
                    }
                    case 'a':
                    {
                        editor.add_row(my_db.get_current_table().get_name());
                        my_db.reset();
                        editor.open(db_filename);
                        my_db.set_current_table(cur_table); 
                        break;
                    }
                    case 'A':
                    {

                        editor.add_column(my_db.get_current_table().get_name());
                        my_db.reset();
                        editor.open(db_filename);
                        my_db.set_current_table(cur_table); 
                        break;
                    }
                    case KEY_DC:
                    {
                        if (cur_row == 0)
                        {
                            editor.delete_column(my_db.get_current_table().get_name(), my_db.get_current_table().get_columns()[cur_col].get_name());
                            cur_col--;
                        }
                        else
                        {
                            editor.delete_row(my_db.get_current_table().get_name(), cur_row);
                            cur_row--;
                        }
                        my_db.reset();
                        editor.open(db_filename);
                        my_db.set_current_table(cur_table); 
                        
                        break;
                    }
                    case '\t':
                    {
                        mode = 3;
                        break;
                    }
                    case 'Q':
                    {
                        mode = 1;
                        break;
                    }
                    case 'e':
                    {
                        edit = true;
                        break;
                    }   
                    default:
                    {
                        break;
                    }
                    
                    }
                }
                move(2,1);
                clrtobot();
                wclear(table_pad);
                wrefresh(table_pad);
                refresh();
                
            } while ((mode == 2) && (exit == false));

            

            break;
        }

        case 3:
        {
            
            int maxrel;
            int cur_rel = 0;
            int rel_id;
            int start_rel = 0;
            int start_x = 0;
            bool info = false;
            
            do
            {                
                getmaxyx(stdscr, yMax, xMax);  

                if (info == true)
                {
                move(2,2);
                printw("RELATION DETAILS:\n  id: - %s\n  seq = %s\n  on_update = %s\n  on_delete = %s\n  match = %s", std::to_string(my_db.get_relations()[cur_rel].get_id()).c_str(), std::to_string(my_db.get_relations()[cur_rel].get_seq()).c_str(), my_db.get_relations()[cur_rel].get_on_update().c_str(), my_db.get_relations()[cur_rel].get_on_delete().c_str(), my_db.get_relations()[cur_rel].get_match().c_str());
                }

                move(6, xMax/2- 15);
                printw("RELATIONS FROM DATABASE %s", db_filename);
                move(7, xMax/2 - 7);
                printw("%d RELATIONS:", my_db.get_relations().size());
                
                WINDOW * relations_pad = newpad(my_db.get_relations().size()*3+1, my_db.relations_length());
                box(relations_pad,0,0);
                int curpos = 1;
                int menu_y = 1; 
                int menu_x = xMax-25;
                move(menu_y++, menu_x);
                printw("i - view add. info");
                move(menu_y++, menu_x);
                printw("a - add new relation");
                move(menu_y++, menu_x);
                printw("DELETE - delete relation");
                move(menu_y++, menu_x);
                printw("Q - quit mode");
                move(menu_y++, menu_x);
                printw("TAB - tables mode");     

                w_start_y = 8;
                w_start_x = xMax/2 - my_db.relations_length()/2;     
                refresh();
                wrefresh(relations_pad);
                keypad(relations_pad, true);
                rel_id = 0;
                for (auto &r : my_db.get_relations())
                {   
                    if (rel_id == cur_rel) {wattron(relations_pad, A_STANDOUT);}

                    wattron(relations_pad, COLOR_PAIR(1));
                    wmove(relations_pad, curpos, 1);
                    wprintw(relations_pad, "%s", r.get_table1().c_str());
                    int tab2pos = r.get_table1().size()+7;
                    if (r.get_col1().size()+r.get_col2().size()+7 > r.get_table1().size()+r.get_table2().size()) { tab2pos += (r.get_col1().size()+r.get_col2().size() - r.get_table1().size() - r.get_table2().size()) ;}

                    
                    wmove(relations_pad, curpos, tab2pos);
                    wprintw(relations_pad, "%s", r.get_table2().c_str());
                    if (r.get_col1().size()+r.get_col2().size()+7 < r.get_table1().size()+r.get_table2().size()) {wmove(relations_pad, ++curpos, (r.get_table1().size()+r.get_table2().size() - r.get_col1().size() - r.get_col2().size())/2) ;}
                    else wmove(relations_pad, ++curpos, 1);
                    wattroff(relations_pad, COLOR_PAIR(1));
                    wprintw(relations_pad, "%s-------%s", r.get_col1().c_str(), r.get_col2().c_str());
                    wattroff(relations_pad, A_STANDOUT);
                    curpos += 2;
                    rel_id++;
                }
                refresh();
                int start_y = start_rel *3;
                prefresh(relations_pad, start_y, start_x, w_start_y, w_start_x, yMax-2, xMax-2);
                key = wgetch(relations_pad);
                
                switch (key)
                {
                case KEY_UP:
                    {
                        cur_rel--;
                        if (cur_rel < 0) {cur_rel++;}
                        if (cur_rel < start_rel) {--start_rel;}
                        break;
                    }
                    case KEY_DOWN:
                    {
                        cur_rel++;
                        if (cur_rel >= my_db.get_relations().size()) {cur_rel--;}
                        
                        if ((cur_rel*3 >= (yMax-2-w_start_y)) && (start_rel*3 <= my_db.get_relations().size()*3+1-(yMax-2-w_start_y))) {++start_rel;}
                        break;
                    } 
                    case KEY_LEFT:
                    {
                        start_x--;
                        if (start_x < 0) {start_x++;}

                        break;
                    }
                    case KEY_RIGHT:
                    {
                        if (my_db.relations_length() > start_x + xMax-6) {start_x++;}
                        
                        break;
                    }
                    
                case KEY_DC:
                    {

                        editor.delete_relation(my_db.get_relations()[cur_rel].get_table1(), my_db.get_relations()[cur_rel].get_col1(), my_db.get_relations()[cur_rel].get_table2(), my_db.get_relations()[cur_rel].get_col2());
                        my_db.reset();
                        editor.open(db_filename);
                        mode = 3;
                        cur_rel--;
                        break;
                    }   
                case 'Q':
                    exit = true;
                    break;
                case '\t':
                    mode = 1;
                    break;
                case 'a':
                    mode = 4;
                    break;
                case 'i':
                    info = !info;
                    break;
                default:
                    break;
                }
                
                move(2,1);
                clrtobot();
                wclear(database_window);
                refresh();
            } while ((mode == 3) && (exit == false));


            break;
        }

        case 4:
        {
            
            int maxrel;
            
            int rel_id;
            int start_rel = 0;
            int rel_x;

            int cur_row=1;
            int cur_col=0;
            bool r = false;
            int table_sy = 0;
            int table_sx = 0;
            int table_startcol = 0;
            w_start_x = 2;
            w_start_y = 7;
            
            int temp_rx, temp_ry;
        
            
            
            do
            {   
                getmaxyx(stdscr, yMax, xMax);  
                move(6, xMax/2- 15);
                printw("RELATIONS FROM DATABASE %s", db_filename);

                int menu_y = 1; 
                int menu_x = xMax-30;
                if (r == true)
                {
                    move(menu_y++, menu_x);
                    printw("ENTER - set new relation");
                    move(menu_y++, menu_x);
                    printw("BACKSPACE - undo parent column");

                }
                else
                {
                    move(menu_y++, menu_x);
                    printw("r - set parent column");
                }
                move(menu_y++, menu_x);
                printw("Q - quit mode");
                move(menu_y++, menu_x);
                printw("TAB - tables mode");          

                int wx = 1;
                int wy = 1;
                std::vector<int> columns = my_db.maxlengths();
                auto vec = columns.begin();
                int col_id = 0;
               
                WINDOW * relations_pad = newpad(my_db.max_columns()+3, columns.back());
                box(relations_pad,0,0);
                keypad(relations_pad, true);
                for (auto &t : my_db.gettables())
                {   
                    wy = 1;
                    wattron(relations_pad, COLOR_PAIR(1));
                    wmove(relations_pad, wy, wx);
                    
                    wprintw(relations_pad, "%s", t.get_name().c_str());
                    
                    wattroff(relations_pad, COLOR_PAIR(1));
                    for (auto &c : t.get_columns())
                    {
                        ++wy;
                        wmove(relations_pad, wy, wx);
                        if (wy-1 == cur_row && col_id == cur_col) {wattron(relations_pad, A_STANDOUT);}

                        if (r == true && wy-1 == temp_ry && col_id == temp_rx) {wattron(relations_pad, A_BLINK);}

                        wprintw(relations_pad, "%s [%s]", c.get_name().c_str(), c.get_type().c_str());

                        wattroff(relations_pad, A_STANDOUT);
                        wattroff(relations_pad, A_BLINK);
                        
                    }
                    wx = *vec;
                    vec++;
                    col_id++;

                }
                refresh();
                prefresh(relations_pad, table_sy, table_sx, w_start_y, w_start_x, yMax-2, xMax-2);
                key = wgetch(relations_pad);
                

                switch (key)
                {
                    case KEY_UP:
                    {
                        cur_row--;
                        if (cur_row < 1) {cur_row++; }

                        if (cur_row < table_sy) {--table_sy;}
                        break;
                    }
                    case KEY_DOWN:
                    {
                        cur_row++;
                        if (cur_row > my_db.gettables()[cur_col].get_columns().size()) {cur_row--; }

                        if ((cur_row >= (yMax-2-w_start_y)) && (table_sy <= (my_db.gettables()[cur_col].get_columns().size()-(yMax-2-w_start_y)))) {++table_sy;}
                        break;
                    }
                    case KEY_LEFT:
                    {
                        cur_col--;
                        if (cur_col < 0) {cur_col++; }
                        
                        if (cur_row > my_db.gettables()[cur_col].get_columns().size()) {cur_row = my_db.gettables()[cur_col].get_columns().size(); }
                        while (columns[cur_col-1] < table_sx) 
                        {
                            table_startcol--; 
                            table_sx = columns[table_startcol-1];
                        }
                        break;
                    }
                    case KEY_RIGHT:
                    {
                        cur_col++;
                        if (cur_col >= my_db.gettables().size()) {cur_col--;}

                        if (cur_row > my_db.gettables()[cur_col].get_columns().size()) {cur_row = my_db.gettables()[cur_col].get_columns().size(); }

                        while (columns[cur_col] > table_sx + xMax-2-w_start_x)
                        {
                            table_startcol++; 
                            table_sx = columns[table_startcol-1];
                        }
                        

                        break;
                    }
                    case 'r':
                    {
                        r=true;
                        temp_rx = cur_col;
                        temp_ry = cur_row;

                        break;
                    }
                    case KEY_BACKSPACE:
                    {
                        r=false;

                        break;
                    }
                    case 10:
                    {
                        if (r == true)
                        {
                            editor.add_relation(my_db.gettables()[temp_rx].get_name(), my_db.gettables()[temp_rx].get_columns()[temp_ry-1].get_name(), my_db.gettables()[cur_col].get_name(), my_db.gettables()[cur_col].get_columns()[cur_row-1].get_name());
                        }
                        r=false;
                        my_db.reset();
                        editor.open(db_filename);
                        mode = 3;
                        break;
                    }
                    

                    case '\t':
                    {
                        mode = 1;
                        break;
                    }
                    case 'Q':
                    {
                        mode = 3;
                        break;
                    }
                    default:
                    {
                        break;
                    }
                    
                }
                move(2,1);
                clrtobot();
                wclear(database_window);
                refresh();
            
            } while ((mode == 4) && (exit == false));
        }
    
    }
    clear();
    
    
    
    }while (exit==false);

    
    
    endwin();
    
    return 0;
}

