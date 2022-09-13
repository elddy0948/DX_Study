# Nonlinear Data Structure

## C++ STL map, set

균형잡힌 이진 검색 트리로 삽입, 검색, 삭제가 모두 O(log n)에 가능합니다.

### map

키 -> 데이터 페어를 저장

### set

키만 저장

## C++ STL priority_queue

힙(Heap) 은 자료를 트리로 조직화하는 또 다른 방법입니다. 이진트리이며 완전(complete) 트리입니다.

보통 배열로 나타낼 수 있는데, 이때 부모, 왼쪽 자식, 오른쪽 자식을 각각

- index / 2 (index >> 1)
- 2 * index (index << 1)
- 2 * index + 1 ((index << 1) + 1)

로 나타낼 수 있습니다. MST (프림 or 크루스칼), 다익스트라, A* 탐색 등에 사용됩니다.

## C++ STL unordered_map

해시 테이블
