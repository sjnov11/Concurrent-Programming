#include <iostream>
#include <string>
#include <set>
#include <map>

using namespace std;

const int ALPHABETS = 26;

struct Trie {
    bool fin_state;
    Trie* next[ALPHABETS];
    Trie() : fin_state(false) {
        for (int i = 0; i < ALPHABETS; i++) {
            next[i] = NULL;
        }
    }
    ~Trie() {
        for (int i = 0; i < ALPHABETS; i++) {
            if (next[i])
                delete next[i];
        }
    }

    void insert(const char* key) {
        if(*key == '\0')
            fin_state = true;
        else {
            int curr = *key - 'a';
            if (next[curr] == NULL) {
                next[curr] = new Trie();
            }
            next[curr]->insert(key + 1);
        }
    }

    bool find(const char* key, string trace, multimap<size_t, string>* result, int index) {
        if (this->fin_state) {
            result->insert(make_pair(index, trace));
        }
        if (*key == '\0') {
            if(result->size() != 0)
                return true;
            return false;
        }
        int curr = *key - 'a';
        if (next[curr] == NULL) 
            return false;
        return next[curr]->find(key + 1, trace + *key, result, index);
    }

    bool remove(const char* key) {
        if (*key == '\0') {
            if (this->fin_state) {
                this->fin_state = false;
                for (int i = 0; i < ALPHABETS; i++) {
                    if(next[i] != NULL) {
                        return false;
                    }
                }
                return true;
            }
            return false;
        }
        int curr = *key - 'a';
        if (next[curr] == NULL)
            return false;

        bool isConnected = false;
        if(next[curr]->remove(key + 1) == true) {
            next[curr] = NULL;
            for (int i = 0; i < ALPHABETS; i++) {               
                if (next[i] != NULL) {
                    isConnected = true;
                    break;
                }
            }
            if (!isConnected && this->fin_state == false) {
                return true;
            }
        }
        return false;
    }
};


int main(){
    int N;
    set<string> word_list;
    char cmd;
    string buf;
    const char *c_buf;
    Trie t;

    std::ios::sync_with_stdio(false);

    cin >> N;
    for (int i = 0; i < N; i++){
        cin >> buf;
        t.insert(buf.c_str());
        //word_list.insert(buf);
    }
    cout << "R" << std::endl;

    while(cin >> cmd){
        cin.get();
        getline(cin, buf);
        c_buf = buf.c_str();
        switch(cmd){
            case 'Q':
                {

                    //set<string> result;
                    multimap<size_t, string> result;
                    for (int i = 0; ; i++) {                        
                        if (*c_buf == '\0') break;
                        t.find(c_buf, "", &result, i);
                        c_buf++;
                    }
                    /*
                    multimap<size_t, string> result;
                    for (set<string>::iterator it = word_list.begin();
                            it != word_list.end(); it++){
                        size_t pos = buf.find(*it);
                        if (pos != string::npos){
                            result.insert(make_pair(pos, *it));
                        }
                    }
                    */

                    /*
                    set<string>::iterator it = result.begin();
                    int cnt = result.size();
                    if (cnt == 0) {
                        cout << -1 << endl;
                        break;
                    }

                    for (; cnt != 0; cnt--, it++) {
                        cout << *it;
                        if (cnt != 1) {
                            cout << "|";
                        }
                    }
                    */
                    

                    
                    set<string> overlapSet;
                    multimap<size_t, string>::iterator it = result.begin();
                    int cnt = result.size();
                    if (cnt == 0){
                        cout << -1 << endl;
                        break;
                    }
                    for (; cnt != 0; cnt--, it++){
                        if (overlapSet.find(it->second) != overlapSet.end())
                            continue;

                        overlapSet.insert(it->second);

                        if (cnt != result.size()){
                            cout << "|";
                        }

                        cout << it->second;
                        
                    }
                    
                    cout << endl;
                }
                break;
            case 'A':
                t.insert(buf.c_str());
                //word_list.insert(buf);
                break;
            case 'D':
                t.remove(buf.c_str());
                //word_list.erase(buf);
                break;
        }
    }
    return 0;
}
