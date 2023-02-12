// Copyright (c) [2022] [Raab Johannes]. All rights reserved.
//
// This file is part of [Algorithms and Datastructures Course].
//
// [ADS_set] is free Software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// [ADS_Set.h] is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//


#ifndef ADS_SET_H
#define ADS_SET_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>


enum class Mode {Free, Used};
template <typename Key, size_t N = 7>
class ADS_set {
public:
    class Iterator;
    using value_type = Key;
    using key_type = Key;
    using reference = value_type &;
    using const_reference = const value_type &;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using const_iterator = Iterator;
    using iterator = const_iterator;
    using key_compare = std::less<key_type>;                         // B+-Tree
    using key_equal = std::equal_to<key_type>;                       // Hashing
    using hasher = std::hash<key_type>;                              // Hashing

private:
    struct List {
        Key value;
        List* next {nullptr};
        bool last_val {false};
        List*  prev {nullptr};
    };

    struct Block{
        Mode mode {Mode::Free};
        bool last_block_in_table {false};
        List* element_in_block {nullptr};
        size_type elements_in_block {0};
        bool first_block_in_table {false};

        void del_el(const Key& key){
            List* help = element_in_block;

            if(elements_in_block==1){
                element_in_block = nullptr;
                delete help;
                elements_in_block = 0;
                mode=Mode::Free;
                return;
            }

            if(key_equal{}(key,help->value)){
                --elements_in_block;
                element_in_block = help->next;
                element_in_block->prev = nullptr;
                delete help;
                return;
            }

            while(help!=nullptr){

                if(key_equal{}(key,help->value)){

                    if(help->last_val) {
                        help->prev->last_val = true;
                        help->prev->next = help->next;
                        --elements_in_block;
                        delete help;
                        return;
                    }
                    --elements_in_block;
                    help->prev->next  = help->next;
                    help->next->prev = help->prev;
                    delete help;
                    return;
                }
                help = help->next;
            }
        }

        ~Block(){
            List* del;
            while(element_in_block!=nullptr){
                del = element_in_block;
                element_in_block = element_in_block->next;
                delete del;
            }

        }

        bool find_in_block(const Key& key)const{

            if(!elements_in_block) return false;
            else {
                List *help = element_in_block;
                while (help != nullptr) {
                    if (key_equal{}(help->value, key)){
                        return true;
                    }
                    help = help->next;
                }
                delete help;
            }
            return false;

        }

        void insert_into_block(const Key& key) {

            ++elements_in_block;
            mode = Mode::Used;

            if (element_in_block == nullptr) {
                List *new_list = new List;
                new_list->value = key;
                new_list->last_val = true;
                new_list->next =nullptr;
                element_in_block = new_list;
            }
            else {
                List *cpy = element_in_block;
                List *new_list = new List;
                new_list->value = key;
                new_list->next = cpy;
                element_in_block = new_list;
                cpy->prev = new_list;
            }
        }

    };

    size_type hash(const key_type& key, const bool& b = false)const {
        if(b) return hasher{}(key)% (table_size*2);
        else return hasher{}(key)% table_size;
    }

    void resize(){

        Block* new_table = new Block[table_size*2];

        for(const auto& val: *this){
            new_table[hash(val,true)].insert_into_block(val);
        }

        delete[] table;
        table = new_table;
        table_size*=2;
        table[table_size-1].last_block_in_table = true;
        table[0].first_block_in_table = true;
    }

    Block* table {new Block[N]};
    const double lf {0.7};
    size_type elements_in_table{0};
    size_type table_size {N};



public:


    //-------Konstruktor
    ADS_set(){
    }

    // PH1



    ADS_set(std::initializer_list<key_type> ilist){
        insert(ilist.begin(),ilist.end());
    }

    template<typename InputIt> ADS_set(InputIt first, InputIt last){
        insert(first,last);
    }


    ADS_set(const ADS_set &other){
        delete []table;
        table = new Block [N];
        insert(other.begin(),other.end());
        table[table_size-1].last_block_in_table = true;
        table[0].first_block_in_table = true;


    }

    ~ADS_set(){delete[] table;}

    ADS_set &operator=(const ADS_set &other){
        ADS_set cpy{other};
        swap(cpy);

        return *this;
    }

    ADS_set &operator=(std::initializer_list<key_type> ilist){
        ADS_set a{ilist};
        swap(a);


        return *this;
    }

    size_type size() const { return elements_in_table;}                                             // PH1
    bool empty() const{ return !elements_in_table;}                                                  // PH1



    void insert(std::initializer_list<key_type> ilist){ insert(ilist.begin(), ilist.end());}
    // PH1
    std::pair<iterator,bool> insert(const key_type &key){

        if (!table[hash(key)].find_in_block(key)) {
            table[hash(key)].insert_into_block(key);
            elements_in_table++;
            if (static_cast<float>(elements_in_table) / table_size >= lf) resize();
            table[table_size - 1].last_block_in_table = true;
            std::pair<Iterator, bool> ret{ find(key),true };

            return ret;
        }
        else {
            std::pair<Iterator, bool> ret{find(key), false};
            return ret;
        }
    }

    template<typename InputIt> void insert(InputIt first, InputIt last){

        for(auto it{first}; it != last; ++it){
            if(!table[hash(*it)].find_in_block(*it)){
                table[hash(*it)].insert_into_block(*it);
                ++elements_in_table;
                if (static_cast<float>(elements_in_table) / table_size >= lf) resize();


            }
        }


        table[0].first_block_in_table = true;
        table[table_size-1].last_block_in_table = true;

    } // PH1

    void clear(){

        ADS_set s;
        swap(s);

    }

    size_type erase(const key_type &key){
        if(!table[hash(key)].find_in_block(key)) return 0;
        else {
            table[hash(key)].del_el(key);
            --elements_in_table;

            return 1;
        }
    }
    size_type count(const key_type &key) const{
        return table[hash(key)].find_in_block(key);
    }
    // PH1
    iterator find(const key_type &key) const{

        if(table[hash(key)].find_in_block(key)) {
            List *help = table[hash(key)].element_in_block;
            while (!key_equal{}(help->value, key)) {
                help = help->next;
            }
            return Iterator(table+hash(key),help);
        }
        else{
            return Iterator{table+table_size,nullptr};
        }

    }

    void swap(ADS_set &other){
        std::swap(table, other.table);
        std::swap(table_size, other.table_size);
        std::swap(elements_in_table, other.elements_in_table);
    }
    //


    const_iterator begin() const{
        return elements_in_table? Iterator{table}: Iterator{table+table_size, nullptr};
    }

    const_iterator end() const{
        return Iterator(table+table_size,nullptr);
    }


    void dump(std::ostream &o = std::cerr) const{
        o << "table size[" << table_size<<"]\n";
        o<< "elements in table[" << elements_in_table <<"]\n";
        for(size_type i = 0; i < table_size; ++i){
            if(table[i].element_in_block !=nullptr){
                o << "Table[" << i << "] has elements [" << table[i].elements_in_block << "]:";
                List* help = table[i].element_in_block;
                while(help!=nullptr) {
                    o << "[last val:" << help->last_val << "]" <<" Val: [" <<  help->value<<"]";
                    help = help->next;
                }
                o <<"::last Block in table: " << table[i].last_block_in_table << ", first_block_in_table:"<< table[i].first_block_in_table<<  "\n";
            }
            if(table[i].element_in_block==nullptr){
                o << "Table[" << i << "] has elements [" << table[i].elements_in_block << "]:";
                o <<"last Block in table: " << table[i].last_block_in_table << ", first_block_in_table:"<< table[i].first_block_in_table<<  "\n";
            }
        }
    }

    friend bool operator==(const ADS_set &lhs, const ADS_set &rhs) {
        if (lhs.elements_in_table != rhs.elements_in_table)
            return false;

        for(auto it = lhs.begin();it!=lhs.end();++it){
            if(rhs.find(*it)==rhs.end()) return false;
        }
        return true;
    }

    friend bool operator!=(const ADS_set &lhs, const ADS_set &rhs) {
        return !(lhs == rhs);
    }
};


template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator{
public:
    using value_type = Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type &;
    using pointer = const value_type *;
    using iterator_category = std::forward_iterator_tag;

private:
    Block* block_pos = nullptr;
    List* list_pos = nullptr;


    void skip(){
        while(block_pos->mode==Mode::Free&&!block_pos->last_block_in_table){
            ++block_pos;
            list_pos = block_pos->element_in_block;
        }

        if(block_pos->last_block_in_table)
            if(!block_pos->elements_in_block){
                block_pos++;
                list_pos = nullptr;
                return;
            }
    }

    void skip2() {
        if(list_pos->last_val){
            if(block_pos->last_block_in_table) {
                block_pos++;
                list_pos = nullptr;
                return;
            }
            else {
                block_pos++;
                list_pos = block_pos->element_in_block;
                skip();
                return;
            }
        }
        list_pos = list_pos->next;
    }

public:




    explicit Iterator(Block* b):block_pos{b},list_pos{b->element_in_block} {
        skip();
    }

    Iterator(Block* b, List* l):block_pos{b},list_pos{l}{
    }

    Iterator():block_pos{nullptr},list_pos{nullptr}{}

    reference operator*() const{return list_pos->value;}
    pointer operator->() const{return &list_pos->value;}

    Iterator &operator++() {


        skip2();
        return *this;
    }

    Iterator operator++(int){
        auto ret{ *this };
        ++*this;
        return ret;
    }

    friend bool operator==(const Iterator &lhs, const Iterator &rhs){
        return (lhs.block_pos == rhs.block_pos && lhs.list_pos == rhs.list_pos);
    }

    friend bool operator!=(const Iterator &lhs, const Iterator &rhs){
        return !(lhs == rhs);
    }
};

template <typename Key, size_t N>
void swap(ADS_set<Key,N> &lhs, ADS_set<Key,N> &rhs) { lhs.swap(rhs); }

#endif // ADS_SET_H