#include <iostream>
#include <string>
#include <set>
#include <map>

#define NUM_THREAD   34
#define ALPHABETS    26

using namespace std;


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


int numberOfWork[NUM_THREAD];
multimap<size_t, string> thread_ret[NUM_THREAD];
string buf;
Trie t;
pthread_t threads[NUM_THREAD];
int thread_state[NUM_THREAD];
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* ThreadFunc(void* arg);


int main(){
    int N;
    char cmd;

    multimap<size_t, string> result;


    std::ios::sync_with_stdio(false);

	for (long tid = 0; tid < NUM_THREAD; tid++) {
		if (pthread_create(&threads[tid], 0, ThreadFunc, (void*)tid) < 0) {
			cerr << "pthread_create error" << endl;
		}
		
		while (thread_state[tid] != -1) {
			pthread_yield();
		}
	}

    cin >> N;
    for (int i = 0; i < N; i++){
        cin >> buf;
        t.insert(buf.c_str());
    }
    cout << "R" << std::endl;

    while(cin >> cmd){
        cin.get();
        getline(cin, buf);
        switch(cmd){
            case 'Q':
                {
                    int queryLength = buf.size();
                    int jobSize = queryLength / NUM_THREAD;
                    int reside = queryLength % NUM_THREAD;					


                    for (int i = 0; i < NUM_THREAD; i++) {
                        numberOfWork[i] = jobSize;
                    }

                    for (int i = 0; i < reside; i++) {
                        numberOfWork[i]++;
                    }

					for (int i = 0; i < NUM_THREAD; i++) {
						thread_state[i] = -1;
					}

/*
                    for (long i = 0; i < NUM_THREAD; i++) {
                        if (pthread_create(&threads[i], 0, ThreadFunc, (void*)i) < 0) {
                            printf("pthread_create error!\n");
                            return 0;
                        }
                    }

                    for (int i = 0; i < NUM_THREAD; i++) {
                        pthread_join(threads[i], NULL);
                    }
*/

					pthread_mutex_lock(&mutex);
					pthread_cond_broadcast(&cond);
					pthread_mutex_unlock(&mutex);

					while(1) {
						bool all_thread_done = true;
						for (int i = 0; i < NUM_THREAD; i++) {
							if (thread_state[i] != 1) {
								all_thread_done = false;
								break;
							}
						}

						if (all_thread_done) {
							break;
						}
						pthread_yield();
					}
					
                    //multimap<size_t, string> temp;
                    for (int i = 0; i < NUM_THREAD; i++) {
                        //temp =  thread_ret[i];
                        for (multimap<size_t, string>::iterator it = thread_ret[i].begin();
                                it != thread_ret[i].end(); it++) {
                            result.insert(make_pair(it->first, it->second));
                        }
                    }

                    
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
                    result.clear();
                }
                break;
            case 'A':
                t.insert(buf.c_str());
                break;
            case 'D':
                t.remove(buf.c_str());
                break;
        }
    }
    return 0;
}


void* ThreadFunc(void* arg) {
    long tid = (long)arg;
    string t_buf;
    const char *c_buf;
    multimap<size_t, string> result;


	// initialize thread to sleep
	pthread_mutex_lock(&mutex);
	thread_state[tid] = -1;
	pthread_cond_wait(&cond, &mutex);	// wake up point
	pthread_mutex_unlock(&mutex);


	while(1) {
		result.clear();
		int start = -1;
		int end = 0;
		t_buf = buf;

		for (int i = 0; i < (int)tid; i++) {
            start += numberOfWork[i];
        }
    
        for (int i = 0; i <= (int)tid; i++) {
            end += numberOfWork[i];
        }
    
        start ++;
            
        t_buf = t_buf.substr(start, -1);
        c_buf = t_buf.c_str();
    
    	int index = 0;
        for (int i = start; i < end; i++) {                        
            if (*c_buf == '\0') break;
            t.find(c_buf, "", &result, start + index);
    		index++;
            c_buf++;
        }
    
        thread_ret[tid] = result;		

		pthread_mutex_lock(&mutex);
		thread_state[tid] = 1;
		pthread_cond_wait(&cond, &mutex);
		pthread_mutex_unlock(&mutex);
	}

}


