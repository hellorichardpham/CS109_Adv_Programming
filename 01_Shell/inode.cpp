// Mustapha Hamade - mhamade
// asg1

#include <cassert>
#include <iostream>
#include <sstream>

using namespace std;

#include "trace.h"
#include "inode.h"

int inode::next_inode_nr = 1;

inode::inode(inode_t init_type):
   inode_nr (next_inode_nr++), type (init_type)
{
   switch (type) {
      case DIR_INODE:
           contents.dirents = new directory();
           break;
      case FILE_INODE:
           contents.data = new wordvec();
           break;
   }
   TRACE ('i', "inode " << inode_nr << ", type = " << type);
}

//
// copy ctor -
//    Make a copy of a given inode.  This should not be used in
//    your program if you can avoid it, since it is expensive.
//    Here, we can leverage operator=.
//
inode::inode (const inode &that) {
   *this = that;
}

//
// operator= -
//    Assignment operator.  Copy an inode.  Make a copy of a
//    given inode.  This should not be used in your program if
//    you can avoid it, since it is expensive.
//
inode &inode::operator= (const inode &that) {
   if (this != &that) {
      inode_nr = that.inode_nr;
      type = that.type;
      contents = that.contents;
   }
   TRACE ('i', "inode " << inode_nr << ", type = " << type);
   return *this;
}


int inode::get_inode_nr() const {
   TRACE ('i', "inode = " << inode_nr);
   return inode_nr;
}

int inode::size()  {
   int size = 1;
   switch (type) {
      case DIR_INODE:
         size = contents.dirents->size();
         break;
      case FILE_INODE:
         wordvec data = *contents.data;
         for (unsigned int i = 0; i < data.size(); i++){
            size += data[i].size();
         }
         if (data.size() > 1) {
            size += data.size()-1;
         }
         break;
   }
   
   TRACE ('i', "size = " << size);
   return size;
}

const wordvec &inode::readfile() const {
   TRACE ('i', *contents.data);
   assert (type == FILE_INODE);
   return *contents.data;
}

void inode::writefile (const wordvec &words) {
   TRACE ('i', words);
   assert (type == FILE_INODE);
   *contents.data = words;
}

void inode::remove (const string &filename) {
   TRACE ('i', filename);

   directory::iterator search;
   search = contents.dirents->find(filename);
   assert (search != contents.dirents->end());
   inode *removeNode = search->second;
   switch(removeNode->type) {
      case DIR_INODE:
         assert (removeNode->size() <=2);
         contents.dirents->erase(search);
         break;
      case FILE_INODE:
         contents.dirents->erase(search);
         break;
   }
   
}

void inode::printList() {
   directory::iterator itor;
   for (itor = contents.dirents->begin(); 
      itor != contents.dirents->end(); itor++) {
      inode *curr = itor->second;
      cout << "      " << curr->get_inode_nr() << "      " 
         << curr->size() << "  " <<  (*itor).first << endl;
   }
}

int inode::get_type() {
   int type_int = 2;
   switch(type) {
      case DIR_INODE:
         type_int = 0;
         break;
      case FILE_INODE:
         type_int = 1;
         break;
   }
   return type_int;
}

inode &inode::locate(const string &filename) {
   directory::iterator search;
   search = contents.dirents->find(filename);
   assert (search != contents.dirents->end());
   inode *found = search->second;
   cout<<found->get_type()<<endl;
   return *found;
}

void inode::mkroot (const inode &theRoot) {
   inode *dot = this;
   theRoot.contents.dirents->insert(pair<string, inode *>(".", dot));
   theRoot.contents.dirents->insert(pair<string, inode *>("..", this));
}

void inode::mkdir (const string &dirname) {
   inode *newDir = new inode (DIR_INODE);
   contents.dirents->insert(pair<string, inode *>(dirname, newDir));
   inode *dot = newDir;
   newDir->contents.dirents->insert(pair<string, inode *>(".", dot));
   
   newDir->contents.dirents->insert(pair<string, inode *>("..", this));
   
}

inode inode::mkfile(const string &filename) {
   directory::iterator search;
   search = contents.dirents->find(filename);
   
   assert (search == contents.dirents->end());
   
   inode *newFile = new inode(FILE_INODE);
   contents.dirents->insert(pair<string, inode *>(filename, newFile));
   
   return *newFile;
}

inode_state::inode_state(): root (NULL), cwd (NULL), 
   prompt ("%"), cwdString ("/") {
   TRACE ('i', "root = " << (void*) root << ", cwd = " << (void*) cwd
          << ", prompt = " << prompt);
}

ostream &operator<< (ostream &out, const inode_state &state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
	   
   return out;
}

inode *inode_state::get_cwd (){
   return cwd;
}

inode *inode_state::get_root (){
   return root;
}

inode *inode_state::get_temp() {
   return temp;
}

inode *inode_state::locateInode(const string &filename) {
   directory::iterator search;
   inode *cwd = get_cwd();
   directory cwd_dirents = cwd->get_directory();
   
   for (search = cwd_dirents.begin(); 
      search != cwd_dirents.end(); search++) {
      if(search->first == filename) {
         return search->second;
      }
   }
   return NULL;
}

string inode_state::get_prompt() {
   return prompt;
}

string inode_state::get_cwdString() {
   return cwdString;
}

void inode_state::set_cwdString(const string &newCWD) {
   cwdString = newCWD;
}

void inode_state::set_prompt (const string &newPrompt){
   prompt = newPrompt;
}

void inode_state::set_cwd (inode &newCWD){
   cwd = &newCWD;
}

void inode_state::set_root(inode &newROOT) {
   root = &newROOT;
}

void inode_state::remove_dirString() {
   unsigned int i = 0;
   string temp;
   for ( i=cwdString.size();;i--) {
      temp = cwdString[i];
      if (temp == "/") {
         break;
      }
      else {
         cwdString.erase(i);
      }
   }
   if (cwdString.size() != 1) {
      cwdString.erase(i);
   }
}

void inode_state::set_temp(inode &newTemp) {
   temp = &newTemp;
}

void inode_state::append_cwdString(string add){
   string addStr;
   if(cwdString.size() != 1) {
      addStr.append("/");
   }
   addStr.append(add);
   cwdString.append(addStr);
}


directory &inode::get_directory(){
  return *contents.dirents;
}

void inode::get_dirents (directory &getDirents) {
   assert (type == DIR_INODE);
   directory::iterator itor;
   for (itor = contents.dirents->begin(); 
      itor != contents.dirents->end(); itor++){
      getDirents.insert(pair<string, inode *>(itor->first, 
         itor->second));
   }
}

void inode::set_dirents (directory &newDirents) {
   assert (type == DIR_INODE);
   *contents.dirents = newDirents;
}
