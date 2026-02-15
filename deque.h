//
// Created by luk on 15-02-2026.
//

#ifndef C_DEQUE_H
#define C_DEQUE_H
#include <cstddef>
#include <initializer_list>
#include <memory>

template<typename T> class Deque{
private:
struct coord{
    size_t node_index;
    size_t offset;
    coord(size_t ni, size_t off);
};
struct Iterator{
    T *ptr_where_am_i;
    T *ptr_of_start_chunk;
    T *ptr_of_end_chunk;
    T **ptr_where_is_mmap;
    Iterator(T **node, size_t offset);
    Iterator& operator++();
    T& operator*();
    bool operator!=(const Iterator& other);
};

    static const size_t CHUNK_SIZE{512};
    T** m_map;
    size_t m_map_size;
    coord m_coord_first;
    coord m_coord_last;
    void resize_map();
    
    public:
    Deque();
    void push_back(T val);
    void push_front(T val);
    T& operator[](size_t index);
    ~Deque();   
    void pop_back();
    void pop_front();
    T& front();
    T& back();
    Iterator begin();
    Iterator end();
    Deque(std::initializer_list<T>list);
};

#include <cstddef>
#include <stdexcept>


template<typename T>
Deque<T>::coord::coord(size_t ni, size_t off) : node_index(ni), offset(off){};


static const size_t INITIAL_MAP_SIZE{8};
template<typename T>
Deque<T>::Deque() 
    : m_coord_first(INITIAL_MAP_SIZE/2, CHUNK_SIZE/2),
      m_coord_last(INITIAL_MAP_SIZE/2, CHUNK_SIZE/2)
{
    m_map = new T*[INITIAL_MAP_SIZE];
    m_map_size = INITIAL_MAP_SIZE;
    for(size_t i = 0; i < INITIAL_MAP_SIZE; i++){
        m_map[i] = nullptr;
    }
    m_map[INITIAL_MAP_SIZE/2] = new T[CHUNK_SIZE];
}
template<typename T>
void Deque<T>::resize_map(){
    size_t new_map_size=2*m_map_size;
    size_t new_start_index = (new_map_size - m_map_size)/2;
    T** new_map = new T*[new_map_size];
    for(size_t i = 0; i < new_map_size; i++){
        new_map[i] = nullptr;
    }
    for(size_t i = 0; i < m_map_size; i++){
        new_map[i+new_start_index] = m_map[i];
    }
    m_coord_first.node_index+=new_start_index;
    m_coord_last.node_index+=new_start_index;
    delete[] m_map;
    m_map = new_map;
    m_map_size = new_map_size;
}

template<typename T>
T& Deque<T>::operator[](size_t index){
    size_t total_ofset = m_coord_first.offset+index;
    coord target{m_coord_first.node_index+total_ofset/CHUNK_SIZE, total_ofset%CHUNK_SIZE};
    return m_map[target.node_index][target.offset];
}

template<typename T>
void Deque<T>::push_back(T val){
    m_map[m_coord_last.node_index][m_coord_last.offset] = val;
    if(++m_coord_last.offset == CHUNK_SIZE){
        if(m_coord_last.node_index+1 == m_map_size)resize_map();
        if(m_map[m_coord_last.node_index+1] == nullptr){
            m_map[m_coord_last.node_index+1] = new T[CHUNK_SIZE];
        }
        m_coord_last = coord(m_coord_last.node_index+1, 0);
    }
}

template<typename T>
void Deque<T>::push_front(T val){
    if(m_coord_first.offset == 0){
        if(m_coord_first.node_index == 0) resize_map();
        m_coord_first.node_index--;
        m_coord_first.offset = CHUNK_SIZE-1;
        if(m_map[m_coord_first.node_index] == nullptr) m_map[m_coord_first.node_index] = new T[CHUNK_SIZE];
    }else m_coord_first.offset--;
    m_map[m_coord_first.node_index][m_coord_first.offset] = val;
}

template<typename T>
Deque<T>::~Deque(){
    for(size_t i = m_coord_first.node_index; i <= m_coord_last.node_index; i++){
        delete[] m_map[i]; 
    }
    delete[] m_map; 
}
template<typename T>
Deque<T>::Iterator::Iterator(T **node, size_t offset){
    ptr_where_is_mmap = node;
    ptr_of_start_chunk = *node;
    ptr_of_end_chunk = ptr_of_start_chunk+CHUNK_SIZE;
    ptr_where_am_i = ptr_of_start_chunk + offset;
}

template<typename T>
Deque<T>::Iterator& Deque<T>::Iterator::operator++(){
    ptr_where_am_i++;
    if(ptr_where_am_i == ptr_of_end_chunk){
        ptr_where_is_mmap++;
        ptr_of_start_chunk = *ptr_where_is_mmap;
        ptr_of_end_chunk = ptr_of_start_chunk+CHUNK_SIZE;
        ptr_where_am_i = ptr_of_start_chunk;
    }
    return *this;
}
template<typename T>
T& Deque<T>::Iterator::operator*(){
    return *ptr_where_am_i;
}

template<typename T>
bool Deque<T>::Iterator::operator!=(const Iterator& other){
    return(this->ptr_where_am_i != other.ptr_where_am_i);
}

template<typename T>
void Deque<T>::pop_back(){
    if(m_coord_first.node_index == m_coord_last.node_index && m_coord_first.offset == m_coord_last.offset)throw std::runtime_error("Popping of empty deque is forbidden!");
    if(m_coord_last.offset == 0){
        m_coord_last.node_index--;
        m_coord_last.offset = CHUNK_SIZE-1;
    }else m_coord_last.offset--;
}

template<typename T>
void Deque<T>::pop_front(){
    if(m_coord_first.node_index == m_coord_last.node_index && m_coord_first.offset == m_coord_last.offset)throw std::runtime_error("Popping of empty deque is forbidden!");
    if(m_coord_first.offset == CHUNK_SIZE-1){
        delete []m_map[m_coord_first.node_index];
        m_map[m_coord_first.node_index] = nullptr;
        m_coord_first.node_index++;
        m_coord_first.offset = 0;
    }else m_coord_first.offset++;
}

template<typename T>
T& Deque<T>::front(){
    if(m_coord_first.node_index == m_coord_last.node_index && m_coord_first.offset == m_coord_last.offset)throw std::runtime_error("Popping of empty deque is forbidden!");    
    return m_map[m_coord_first.node_index][m_coord_first.offset];
}

template<typename T>
T& Deque<T>::back(){
    if(m_coord_first.node_index == m_coord_last.node_index && m_coord_first.offset == m_coord_last.offset)throw std::runtime_error("Popping of empty deque is forbidden!");
    if(m_coord_last.offset == 0){
        return m_map[m_coord_last.node_index-1][CHUNK_SIZE-1];
    }
    else return m_map[m_coord_last.node_index][m_coord_last.offset-1];
}

template<typename T>
Deque<T>::Iterator Deque<T>::begin(){
    return Iterator(&m_map[m_coord_first.node_index], m_coord_first.offset);
}

template<typename T>
Deque<T>::Iterator Deque<T>::end(){
    return Iterator(&m_map[m_coord_last.node_index], m_coord_last.offset);
}

template<typename T>
Deque<T>::Deque(std::initializer_list<T>list) : Deque() {
    for(auto &x:list){
        push_back(x);
    }
}

#endif //C_DEQUE_H