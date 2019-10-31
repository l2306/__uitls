
#ifndef _CACHE__LRU_LFU_
#define _CACHE__LRU_LFU_

#include <iostream>
#include <vector>
#include <ext/hash_map>
using namespace std;
using namespace __gnu_cxx;

template <class K, class T>
struct Node{
    K       key;
    T       data;
    short   hitCnt;      //次数
    Node *prev, *next;
};

template <class K, class T>
class cache_LRU{
    int maxSiz;		//设计容量
	int curSiz;		//当前容量
public:
    cache_LRU(size_t size){
        entries_ = new Node<K,T>[size];
        for(int i=0; i<size; ++i)
            free_entries_.push_back(entries_+i);
        head_ = new Node<K,T>;
        tail_ = new Node<K,T>;
        head_->prev = NULL;
        head_->next = tail_;
        tail_->prev = head_;
        tail_->next = NULL;
    }
    ~cache_LRU(){
        delete      head_;
        delete      tail_;
        delete[]    entries_;
    }
    //取其值 置于头部 
    T get(K key){
        Node<K,T> *node = hashmap_[key];
        if(node){
            detach(node);
            attach(node);
            return node->data;
        }else{
            return T();
        }
    }
	//添加新节点
    void put(K key, T data){
        Node<K,T> *node = hashmap_[key];	//
        if(node){
            detach(node);
            node->data = data;
            attach(node);
        }else{
            if(free_entries_.empty()){		//无可用缓存 取出已用缓存
                node = tail_->prev;
                detach(node);
                hashmap_.erase(node->key);
            }else{
                node = free_entries_.back();	//取一个可用缓存
                free_entries_.pop_back();
            }
            node->key = key;
            node->data = data;
            hashmap_[key] = node;		//添如hashmap
            attach(node);			//添加到缓存
        }
    }
private:
    //从其中删除
    void detach(Node<K,T>* node){
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    //添加到头部
    void attach(Node<K,T>* node){
        node->prev  = head_;
        node->next  = head_->next;
        head_->next = node;
        node->next->prev = node;
    }
private:
    Node<K,T>                   *head_, *tail_;     //缓存的头尾
    Node<K,T>                   *entries_;          //全部的内存 一次开辟，重复使用
    vector<Node<K,T>*>          free_entries_;      //闲置缓存地址列表
    hash_map<K, Node<K,T>* >    hashmap_;	        //利用hash实现地址快速检索
};


template <class K, class T>
class cache_LFU{
    int maxSiz;		//设计容量
	int curSiz;		//当前容量
public:
    cache_LFU(size_t size){
        entries_ = new Node<K,T>[size];
        for(int i=0; i<size; ++i)
				entries_[i].hitCnt = 0;
        for(int i=0; i<size; ++i)
            free_entries_.push_back(entries_+i);
        head_ = new Node<K,T>;
        tail_ = new Node<K,T>;
        head_->prev = NULL;
        head_->next = tail_;
        tail_->prev = head_;
        tail_->next = NULL;
        head_->hitCnt = 0x7fff;
        tail_->hitCnt = -1;
    }
    ~cache_LFU(){
        delete      head_;
        delete      tail_;
        delete[]    entries_;
    }
    //取其值 置于头部 
    T get(K key){
        Node<K,T>* node = hashmap_[key];
        if(node){
            //attach();
            incr(node->key);
            return node->data;
        }else{
            return T();
        }
    }
    //命中次数 
    short hitCnt(K key){
        Node<K,T>* node = hashmap_[key];
        if(node){
            return node->hitCnt;
        }else{
            return -1;
        }
    }
	//添加新节点
    void put(K key, T data){
        Node<K,T> *node = hashmap_[key];	//
        if(node){
            incr(node->key);
        }else{
            if(free_entries_.empty()){		//无可用缓存 取出已用缓存
                node = tail_->prev;
                detach(node);
                hashmap_.erase(node->key);
            }else{
                node = free_entries_.back();	//取一个可用缓存
                free_entries_.pop_back();
            }
            node->key = key;
            node->data = data;
            node->hitCnt = 1;
            hashmap_[key] = node;		//添如hashmap
            attach(tail_->prev, node);			//添加到缓存
        }
    }
	//增加频率 
    void incr(K key){
        Node<K,T> *node = hashmap_[key];
        if(node){
            Node<K,T>* cursor = node;
            detach(node);
            node->hitCnt += 1;
            while (cursor->hitCnt <= node->hitCnt)
                cursor = cursor->prev;
            attach(cursor, node);
            return ;
        }else{
            node = free_entries_.back();
            free_entries_.pop_back();
            hashmap_[key];
            attach(tail_->prev,node);
            return ;
        }
    }
    //减少频率
    void decr(K key){
       Node<K,T>* node = hashmap_[key];
       if(node && node>1){
            Node<K,T>* cursor = node;
            detach(node);
            node->hitCnt -= 1;
            while (cursor->hitCnt >= node->hitCnt)
                cursor = cursor->next;
            attach(cursor->prev, node);
            return node->data;
        }else{
            if(node){
                detach(node);
                node->hitCnt = 0;
                hashmap_.erase(node->key);
                free_entries_.push_back(node);
            }
            return ;
        }
    }
    
    void expire(){
        //  过期时  不允许 其他操作
        //应该定时清理不被使用的缓存     /*从后往前，这样调整中偏移最少*/
        // 方法：计数右移 指数级递减，以减少最大数的影响程度
        if(head_->next == tail_)
            return ;
        Node<K,T>* cursor = tail—>prev;
        while (cursor != head_) {
            cursor->hitCnt >>= 1;
            if( 0 == cursor->hitCnt ) {
                Node<K,T>* node = cursor;
                cursor = cursor ->prev;
                detach(node);
                hashmap_.erase(node->key);
                free_entries_.push_back(node);
            } 
            cursor = cursor ->prev;
        }
    }
    
private:
    //从其中删除
    void detach(Node<K,T>* node){
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    //添加2到1的节点后，1节点一直在内部
    void attach(Node<K,T>* node1, Node<K,T>* node2){      
        node1->next->prev = node2;
        node2->next = node1->next;
        node1->next = node2;                  
        node2->prev = node1;
    }
private:
    Node<K,T>                   *head_, *tail_;     //缓存的头尾
    Node<K,T>                   *entries_;          //全部的内存 一次开辟，重复使用
    vector<Node<K,T>*>          free_entries_;      //闲置缓存地址列表
    hash_map<K, Node<K,T>* >    hashmap_;	        //利用hash实现地址快速检索
};

using namespace __gnu_cxx;
namespace __gnu_cxx
{
    template<> struct hash<const string> {
    size_t operator()(const string& s) const 
        { return hash<const char*>()( s.c_str() ); } //__stl_hash_string
	};
	template<> struct hash<string> {
        size_t operator()(const string& s) const 
        { return hash<const char*>()( s.c_str() ); } 
	};
}

/*

//应用场景: 需要频繁加载到内存的数据，避免全部驻留内存浪费空间

int main()
{
    cache_LRU<int, string> lru_cache(100);
    lru_cache.put(1, "one");
    cout<<lru_cache.get(1)<<endl;
    if(lru_cache.get(2) == "")
        lru_cache.put(2, "two");
    cout<<lru_cache.get(2)<<endl;

	string  str1("one");
	string  str2("two");
    cache_LFU<string, string> lfu_cache(100);
    lfu_cache.put(str1, "one");
    cout<<lfu_cache.get(str1)<<endl;
    if(lfu_cache.get(str2) == "")
        lfu_cache.put(str2, "two");
	cout<<lfu_cache.hitCnt(str2)<<endl;
    lfu_cache.incr(str2);
    cout<<lfu_cache.get(str2)<<" "<<endl;
	cout<<lfu_cache.hitCnt(str2)<<endl;
   
    return 0;
}
*/

#endif
