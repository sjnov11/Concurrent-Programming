# Signal 

## Concurrent Programming Project #1



### 1. String matching algorithm

 Signal 로 들어오는 Query string에서 주어진 검색단어를 찾기 위해서 여러 string matching algorithm을 찾아보았고, 그 중 multi processing 에 가장 적합한 PFAC algorithm을 사용하기로 하였다. PFAC algorithm은 기존 String matching algorithm 중 Sequential processing에서 가장 효율적인 Aho-Corasick algorithm을 parallel processing으로 바꾸면서 생기는 비효율 문제를 해결하기 위해 제안된 algorithm이다. 직접 구현한 Algorithm의 순서도는 다음과 같다.

##### 1.1 PFAC Algorithm

1. 주어진 문자 pattern에 대하여 DFA 생성.
2. (n-m+1) 개의 thread 생성. (n : Query string length, m : minimun pattern length)
3. i 번째 thread는 i번째 문자에서 부터 DFA 탐색을 시작.
4. 탐색 실패시 thread 종료.
5. Final state 에 도달할 경우, 
   1. 현재 탐색된 pattern 저장
   2. DFA 탐색 진행
   3. 4번 단계
6.  이 외의 경우,
   1. DFA 탐색 진행
   2. 4번 단계

Aho-Corasick 과 PFAC Algorithm 은 다음 학술자료를 참고하였다.

[참고자료]: https://goo.gl/LWhp1x	"Aho-Corasick / PFAC Algorithm"



### 2. Concurrent programming

  Project1 의 채점 환경은 다음과 같다.

| 구분            | 사양                                       |
| :------------ | ---------------------------------------- |
| CPU           | 2 x Intel(R) Xeon(R) CPU E5-2699 v3 @ 2.30GHz |
| Configuration | 36 cores / Hyperthread disabled          |
| Main Memory   | 450GB                                    |

 Concurrent programming 은 다음과 같이 구현하였다. 

- Thread 의 갯수는 core 가 가장 효율적으로 발휘할 수 있는 34개로 제한하였다.
- Thread create, join 으로 인한 overhead 를 줄이기 위해 thread pool 방식으로 구현하였다.
- Query string 길이를 thread 갯수로 나누어 각 thread 별 workload 를 똑같이 배분하였다.
- Query string 이 충분히 길지 못한 경우, workload 를 받지 못하는 경우가 발생하게 되는데, 
  이 thread 는 그대로 wait 시킴으로 성능을 더 높일 수 있었다.