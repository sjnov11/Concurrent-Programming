# Scalable Lock Manager 

### Concurrenct Programming Project #4



## Analyze Innobase Lock Manager

Read only workload 는 lock_clust_rec_read_check_and_lock 함수를 호출하여 읽으려는 record에 lock을 잡을지 확인한다. 먼저, lock_mutex_enter() 를 호출하여, innobase 의 lock_sys 에 mutex를 잡는다. 그리고나선, record hash table에 아래 그림처럼 lock object를 연결한다. lock_sys의 mutex를 잡고 하나씩 hash table에 lock object 를 연결하게 되는데 workload 가 이부분에 의해서 병목현상이 발생한다. 따라서, 이 부분을 lock free 로 구현함으로써 병목현상을 해결하고자 하였다.



## Lock Free Record Hash Table

Record Hash Table에 lock object 를 추가할 때, lock free 로 구현하기 위해서, lock free linked list 의 아이디어를 차용하여 구현하였다. 해당 cell 에서 찾고자하는 lock object가 나올 때까지 scan하면서, 만약 logical delete(bit masking으로 확인) lock object가 있다면 이를 physical delete 해준다. 



#### HASH_FIND

: Find **DATA** structure which are part of linked list in a bucket. Also, It performs physical deletion when it traverse list.
Return **PRED** (previous node), **CURR** (current node = **DATA**), **PRED_TYPE** (*hash_cell_t* or *lock_t*)

```c
while (marked) {
  snip = __sync_bool_compare_and_swap( &(((TYPE*)_pred)->NAME), _curr, _succ );\
  if (!snip) {\
    retry = true;\
    break;\
  }\
  _curr = _succ;\
  if (_curr == DATA) {\
    PRED = _pred;\
    PRED_TYPE = 1;\
    CURR = _curr;\
    is_done = true;\
    break;\
  }\
  _succ = (TYPE*) REFERENCE(_curr->NAME);\
  marked = ISMARKED(_curr->NAME);\
}\
```



#### HASH_INSERT

: Find location of linked list to insert new data. Change **curr->hash** to pointer of new data using atomic operation *__sync_bool_compare_and_swap*.

```c
while (1) {\
  HASH_FIND(TYPE, NAME, cell3333, node, NULL, pred, pred_type, curr);\
  if (pred_type == 0) {\
    if (__sync_bool_compare_and_swap( &(((hash_cell_t*)pred)->node), curr, DATA )) {\
      break;\
    }\
  } else {\
    if (__sync_bool_compare_and_swap( &(((TYPE*)pred)->NAME), curr, DATA )) {\
      break;\
    }\
  }\
}\
```



#### HASH_DELETE

: Find location of data which will be removed. Change **prev->hash(node)** to pointer of **succ**. It performs physical deletion just once. If it failed, it will be done when **HASH_FIND** is called.

```c
while (1) {\
  HASH_FIND(TYPE, NAME, cell3333, node, DATA, pred, pred_type, curr);\
  succ = (TYPE*)REFERENCE(curr->NAME);\
  if (__sync_bool_compare_and_swap(&curr->NAME, succ, MARK(succ))) {\
    if(pred_type == 0) {\
      __sync_bool_compare_and_swap( &(((hash_cell_t*)pred)->node), curr, succ);\
    } else {\
      __sync_bool_compare_and_swap( &(((TYPE*)pred)->NAME), curr, succ);\
    }\
    break;\
  }\
}\
```



