#include <iostream>
#include <ncurses.h>
#include <string>
#include <vector>
//#include "../backend.h"
//#include "../frontend.h"
#include <sqlite3.h> 
#include "sqlite.h"

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

                my_db.get_current_table().add_colname(azColName[i]);

                //printw("kn = %s\n",azColName[i]);               
            }
            my_db.get_current_table().add_field(field(azColName[i],argv[i]));

        }
        
        my_db.ct_columns_done();
        return 0;
    }
static int cor_callback_items(void* NotUsed, int argc, char** argv, char** azColName)
{   
    return 0;
}
static int callback_relations(void* NotUsed, int argc, char** argv, char** azColName)
{
    /*for (int i = 0; i < argc; i++)
    {   
        printw("%s: %s\n", azColName[i], argv[i]);
        
    }
    printw("\n");*/

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
                    cerr<<"Error displaying table\n"<<endl;
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
        
        
    }


    void edit_item(std::string table, std::string colname, std::string condition1, std::string condition2, std::string nvalue)
    {
        char *zErrMsg;
        std::string command = ("UPDATE " + table + " set " + colname + " = '" + nvalue + "' where " + condition1 + " = " + condition2 + "; ");
        //command+=("SELECT * from " + table + ";");
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
        std::string command = ("ALTER TABLE " + table + " RENAME TO " + nvalue + "; ");
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
    void edit_colname(std::string table, std::string colname, std::string nvalue)
    {
        char *zErrMsg;
        std::string command = ("ALTER TABLE " + table + " RENAME COLUMN " + colname + " to " + nvalue + "; ");
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
   
};

int main()
{   
    initscr();
    raw();
    noecho();
    curs_set(0);
    int yMax, xMax;
    
    int mode =  1;
    
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    
    int w_start_y = 7;
    int w_start_x = 5; 
    sse editor;
    std::string db_filename = "MY_DB.db";
    editor.open(db_filename.c_str());
    int key;
    
    bool exit = false;
    bool edit = false;
    std::string buffer;
    int cur_table = 0;
    do{
        
        move(0, xMax/2-11);
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

            
            int choice;
            
            do {
                getmaxyx(stdscr, yMax, xMax);    
                move(6, xMax/2-11);
                printw("DATABASE <XXX>");
                WINDOW * database_window = newwin(yMax-10, xMax-10, w_start_y, w_start_x);
                box(database_window,0,0);
                refresh();
                wrefresh(database_window);
                keypad(database_window, true);

                move(w_start_y+1,w_start_x+1);
                int cur_y = w_start_y+2;
                printw("%d TABLES:", my_db.gettables().size());
                move(cur_y++,w_start_x+1);
                printw("-----------");
                int tables_y = cur_y;
                my_db.set_current_table(0);
                int i = 0;
                for (auto &t : my_db.gettables()) 
                {   
                    
                    if (i==cur_table) {attron(A_STANDOUT);}
                    move(tables_y,w_start_x+1);
                    if (i==cur_table && edit==true)
                    {
                        
                        printw("%s", buffer.c_str());
                        for (int s = buffer.size(); s < t.get_name().size(); ++s) {printw(" ");}
                    }
                    else
                    {                  
                        printw("%s", t.get_name().c_str());
                    }
                    attroff(A_STANDOUT);

                    ++tables_y; ++i;
                }
                refresh();
                key = wgetch(database_window);
                
                if (edit==true)
                {
                    switch (key)
                    {
                    case KEY_DC:
                        edit = false;
                        buffer.clear();
                        break;
                    case KEY_BACKSPACE:
			            if (buffer.length() > 0) {
                        buffer = buffer.substr(0, buffer.size() - 1);}
                        break;
                    case 10:
                        my_db.reset();
                        editor.edit_table(my_db.gettables()[cur_table].get_name(), buffer);
                        editor.open(db_filename.c_str());
                        edit = false;
                        buffer.clear();
                        break;
                    default:
                        buffer+=key;
                        break;
                    }
                }
                else
                {
                    switch(key)
                    {
                    case KEY_UP:
                        cur_table--;
                        if (cur_table < 0) {cur_table++;}
                        break;
                    case KEY_DOWN:
                        cur_table++;
                        if (cur_table == my_db.gettables().size()) {cur_table--;}
                        break;
                    case 10:
                        my_db.set_current_table(cur_table);
                        mode = 2;
                        break;
                    case 'Q':
                        {exit=true;}
                        break;
                    case 'e':
                        edit = true;
                        break;
                    case '\t':
                        mode = 3;   
                        break;
                    deflaut:
                        break;

                    }  
                }
                
                
                tables_y = cur_y;
                i=0;
                

                
                
            } while ((mode == 1) && (exit == false));
        
            break;
        }
        
        case 2:
        {
            std::string mode2_header;
            mode2_header = "TABLE " + my_db.get_current_table().get_name() + " FROM DATABASE <XXX>";           
            
            int cur_row=0;
            int cur_col=0;
            
            
            do 
            {   
                getmaxyx(stdscr, yMax, xMax);  
                move(6, xMax/2-11);
                printw("%s", mode2_header.c_str());
                w_start_x = 2;
                WINDOW * database_window = newwin(yMax-10, xMax-10, w_start_y, w_start_x);
                box(database_window,0,0);
                
                refresh();
                wrefresh(database_window);
                keypad(database_window, true);
                int cur_y = w_start_y+1;
                int y = 0; int x = 0;

                move(w_start_y+1,w_start_x+1);
                attron(COLOR_PAIR(1));
                for (auto &c : my_db.get_current_table().get_colnames())
                {
                    if (y == cur_row && x == cur_col) {attron(A_STANDOUT);}
                    if (y == cur_row && x == cur_col && edit == true)
                    {
                        printw("%s\t\t\t", buffer.c_str());
                    }
                    else
                    {
                        printw("%s\t\t\t",c.c_str());
                    }
                    attroff(A_STANDOUT);
                    x++;
                }
                attroff(COLOR_PAIR(1));
                y++;
                x=0;

                move(++cur_y, w_start_x+1);

                for(auto &r : my_db.get_current_table().get_rows())
                {
                    for (auto &f : r.get_fields())
                    {   
                        if (y == cur_row && x == cur_col) {attron(A_STANDOUT);}
                        if (y == cur_row && x == cur_col && edit == true)
                        {
                            printw("%s\t\t\t", buffer.c_str());
                        }
                        else
                        {
                            printw("%s\t\t\t", f.get_value().c_str());                   
                        }
                        attroff(A_STANDOUT);
                        x++;
                    }

                    move(++cur_y, w_start_x+1);
                    y++;
                    x=0;
                }



                refresh();
                key = wgetch(database_window);

                if (edit==true)
                {
                    switch (key)
                    {
                    case KEY_DC:
                        edit = false;
                        buffer.clear();
                        break;
                    case KEY_BACKSPACE:
			            if (buffer.length() > 0) {
                        buffer = buffer.substr(0, buffer.size() - 1);}
                        break;
                    case 10:
                        if (cur_row == 0)
                        {
                            editor.edit_colname(my_db.get_current_table().get_name(), my_db.get_current_table().get_colnames()[cur_col], buffer);
                        }
                        else
                        {
                            editor.edit_item(my_db.get_current_table().get_name(), my_db.get_current_table().get_colnames()[cur_col], my_db.get_current_table().get_rows()[cur_row-1].get_fields()[0].get_colname(), my_db.get_current_table().get_rows()[cur_row-1].get_fields()[0].get_value(), buffer);

                        }
                        
                        my_db.reset();
                        editor.open(db_filename.c_str());
                        my_db.set_current_table(cur_table);
                        edit = false;
                        buffer.clear();
                        break;
                    default:
                        buffer+=key;
                        break;
                    }
                }
                else
                {
                    switch (key)
                    {
                    case KEY_UP:
                        cur_row--;
                        if (cur_row < 0) {cur_row++; }
                        break;
                    case KEY_DOWN:
                        cur_row++;
                        if (cur_row > my_db.get_current_table().get_rows().size()) {cur_row--; }
                        break;
                    case KEY_LEFT:
                        cur_col--;
                        if (cur_col < 0) {cur_col++; }
                        break;
                    case KEY_RIGHT:
                        cur_col++;
                        if (cur_col >= my_db.get_current_table().get_colnames().size()) {cur_col--; }
                        break;
                    case '\t':
                        mode = 3;
                        break;
                    case 'Q':
                        mode = 1;
                        break;
                    case 'e':
                        edit = true;
                        break;   
                    default:
                        break;
                    }
                }

            } while ((mode == 2) && (exit == false));

            

            break;
        }

        case 3:
        {
            getmaxyx(stdscr, yMax, xMax);  
            move(6, xMax/2-11);
            printw("RELATIONS FROM DATABASE <XXX>");
            WINDOW * database_window = newwin(yMax-10, xMax-10, w_start_y, w_start_x);
            box(database_window,0,0);
            
            refresh();
            wrefresh(database_window);
            keypad(database_window, true);
            
            do
            {   
                int curpos = w_start_y+1;
                move(curpos, w_start_x+1);
                printw("%d RELATIONS", my_db.get_relations().size());
                move(++curpos, w_start_x+1);
                printw("-----------");
                for (auto &r : my_db.get_relations())
                {   
                    attron(COLOR_PAIR(1));
                    move(++curpos, w_start_x+1);
                    printw("%s", r.get_table1().c_str());
                    move(curpos, w_start_x+1 + r.get_table1().size()+7);
                    printw("%s", r.get_table2().c_str());
                    move(++curpos, w_start_x+1);
                    attroff(COLOR_PAIR(1));
                    printw("%s-------%s", r.get_col1().c_str(), r.get_col2().c_str());
                    
                    ++curpos;
                }
                refresh();
                wrefresh(database_window);
                key = getch();
                switch (key)
                {
                case 'Q':
                    exit = true;
                    break;
                case '\t':
                    mode = 1;
                default:
                    break;
                }
            
            } while ((mode == 3) && (exit == false));
        }
    
    }
    clear();
    
    
    
    }while (exit==false);
    
    endwin();
    
    return 0;
}

