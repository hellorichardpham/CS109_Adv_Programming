// Mustapha Hamade - mhamade
// asg1

#include "commands.h"
#include "trace.h"
#include "inode.h"
#include <sstream>

commands::commands(): map (commandmap()) {
   map["cat"    ] = fn_cat    ;
   map["cd"     ] = fn_cd     ;
   map["echo"   ] = fn_echo   ;
   map["exit"   ] = fn_exit   ;
   map["ls"     ] = fn_ls     ;
   map["lsr"    ] = fn_lsr    ;
   map["make"   ] = fn_make   ;
   map["mkdir"  ] = fn_mkdir  ;
   map["prompt" ] = fn_prompt ;
   map["pwd"    ] = fn_pwd    ;
   map["rm"     ] = fn_rm     ;
}

function commands::operator[] (const string& cmd) {
   return map[cmd];
}

void fn_cat (inode_state &state, const wordvec &words){
   TRACE ('c', state);
   TRACE ('c', words);

   
   for(unsigned int i = 1; i<words.size(); i++){
  
     string filename = words[i];
     inode *found = state.locateInode(words[i]);

     if (found != NULL) {
        wordvec fileContents = found->readfile();
        for(unsigned int i = 0; i < fileContents.size(); i++) {
           cout << fileContents[i] << " ";
        }
        cout << endl;
     }
     else {
        cout << filename << " does not exist" << endl;
     }
   }
}

void fn_cd (inode_state &state, const wordvec &words){
   TRACE ('c', state);
   TRACE ('c', words);
   string theroot = "/";
   
   wordvec cdVec = words;
   cdVec.erase(cdVec.begin());
   string filename = cdVec.front();
   if (words.size() >1){
     if(words[1]==".."){
         state.remove_dirString();
     }
      if(words[1]!=".."&&words[1]!="."){
       state.append_cwdString(filename);
      }
     inode *found = state.locateInode(filename);
     state.set_cwd(*found);
   }
   if(words.size() == 1){
      inode *temp = state.get_root();
      state.set_cwdString(theroot);
      state.set_cwd(*temp);
   }
}

void fn_echo (inode_state &state, const wordvec &words){
   TRACE ('c', state);
   TRACE ('c', words);
   
   wordvec echoVec = words;
   echoVec.erase(echoVec.begin());
   cout << echoVec << endl;
}

void fn_exit (inode_state &state, const wordvec &words){
   TRACE ('c', state);
   TRACE ('c', words);
   wordvec exit = words;
   int status;
   exit.erase(exit.begin());
   stringstream statusInt(exit.front());
   statusInt >> status;
   exit_status::set (status);

   throw ysh_exit_exn ();
}

void fn_ls (inode_state &state, const wordvec &words){
   TRACE ('c', state);
   TRACE ('c', words);
   
   inode *cwd = state.get_cwd();
   cwd->printList();

}

void fn_lsr (inode_state &state, const wordvec &words){
   TRACE ('c', state);
   TRACE ('c', words);
   
}


void fn_make (inode_state &state, const wordvec &words){
   wordvec makeVec = words;
   makeVec.erase(makeVec.begin());
   string filename = makeVec.front();
   makeVec.erase(makeVec.begin());
   
   inode *cwd = state.get_cwd();
   inode newFile = cwd->mkfile(filename);
   
   newFile.writefile(makeVec);
   
   TRACE ('c', state);
   TRACE ('c', words);
}

void fn_mkdir (inode_state &state, const wordvec &words){
   TRACE ('c', state);
   TRACE ('c', words);
   
   string dirName = words[1];
   inode *cwd = state.get_cwd();
   cwd->mkdir(dirName);
}

void fn_prompt (inode_state &state, const wordvec &words){
   TRACE ('c', state);
   TRACE ('c', words);
   std::stringstream ss;
   
   for (size_t i = 1; i < words.size(); ++i) {
    ss << words[i];
      if (i<words.size()-1){
         ss<<" ";
      }
   }
   std::string newString = ss.str();
   state.set_prompt(newString);
}

void fn_pwd (inode_state &state, const wordvec &words){
   TRACE ('c', state);
   TRACE ('c', words);
   
   string pwdString = state.get_cwdString();
   cout<<pwdString<<endl;
}

void fn_rm (inode_state &state, const wordvec &words){
   TRACE ('c', state);
   TRACE ('c', words);
   
   wordvec removeVec = words;
   removeVec.erase(removeVec.begin());
   string filename = removeVec.front();
   inode *cwd = state.get_cwd();
   cwd->remove(filename);
}

void fn_rmr (inode_state &state, const wordvec &words){
   TRACE ('c', state);
   TRACE ('c', words);
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}
