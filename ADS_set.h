#include <iostream>
#include <exception>
#include <cmath>
#include <functional>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <assert.h>
#include <utility>
using namespace std;
class Person;
template <typename Key,size_t N = 5> 
class ADS_set {
public:
	class Forward_it;
	using value_type = Key;
	using key_type = Key;
	using reference = value_type&;
	using pointer = value_type*;
	using const_reference = const value_type&;
	using size_type = size_t;
	using difference_type = std::ptrdiff_t;
	using const_iterator = Forward_it;
	using iterator = const_iterator;
	using key_equal = std::equal_to<key_type>; // Hashing
	using hasher = std::hash<key_type>;        // Hashing
	struct pai {
		value_type fi;
		bool se{false};
		pai() : se (false) {}
		pai(value_type _fi) : fi(_fi), se(true){}
		value_type& first(){return fi;}
		bool second(){return se;}
		void set(value_type f){fi = f; se = true;}
		void del(){se = false;}
        value_type& operator * () { return fi; }
	};
	class Forward_it {
	public:
		using value_type = ADS_set::value_type;
		using difference_type = ptrdiff_t;
		using reference = ADS_set::reference;
		using pointer = ADS_set::pointer;
		using iterator_category = std::forward_iterator_tag;
	private:
		pai* ptr;	
		pai* end;
	public:
		Forward_it() : ptr(NULL), end(NULL) {}
		Forward_it (pai* _ptr, pai* _end) : ptr(_ptr), end(_end) {}
		Forward_it (pai& _ptr, pai& _end) : ptr(&_ptr), end(&_end) {}
		Forward_it (const iterator& _it) : ptr(_it.ptr), end(_it.end) {}
		friend difference_type operator-( iterator& lop, iterator& rop) { return lop.ptr-rop.ptr; }
		friend difference_type operator+( iterator& lop, iterator& rop) { return lop.ptr+rop.ptr; }
		friend iterator operator-( iterator& lop, difference_type rop) { return lop.ptr-rop; }
		friend iterator operator+( iterator& lop, difference_type rop) { return lop.ptr+rop; }
		friend pai& operator+(const iterator& lop, int rop) { return *(lop.ptr+rop); }
		friend pai& operator-(const iterator& lop, int rop) { return *(lop.ptr-rop); }
		friend pai& operator+( iterator& lop, int rop) { return *(lop.ptr+rop); }
		friend pai& operator-( iterator& lop, int rop) { return *(lop.ptr-rop); }
		friend difference_type operator-( const Forward_it& lop, pointer rop)  { return lop.ptr-rop; }
		friend difference_type operator+( const iterator& lop, pointer rop)  { return lop.ptr+rop; }
		friend bool operator<( iterator& lop, const iterator& rop) { return lop<rop; }
		friend bool operator>( iterator& lop, const iterator& rop) { return lop>rop; }
		friend bool operator<=( iterator& lop, const iterator& rop) { return lop<=rop; }
		friend bool operator>=( iterator& lop, const iterator& rop) { return lop>=rop; }
		friend bool operator ==( const iterator & lop, const iterator & rop) { return lop.ptr == rop.ptr; }
		friend bool operator ==(  pai lop,  pai rop) { return key_equal{}(lop->first(), rop->first()); }
		friend bool operator !=( const iterator & lop, const iterator & rop) { return !(lop == rop); }
		iterator & operator += (difference_type rop) const { *this += rop; return *this; }
        iterator & operator -= (difference_type rop) const { return this += -rop; }
        iterator & operator ++ () { 
			if (ptr != end) ptr++;
			while (ptr != end && ptr->second()==false) ++ptr;
			return *this; 
			}
        iterator operator ++ (int) { //postfix
			iterator copy(*this); 
			if (ptr != end) ptr++; 
			while (ptr != end && ptr->second()==false) ++ptr;
			return copy; 
			}
        iterator & operator -- () { 
			ptr--; 
			while (ptr != end && ptr->second()==false)  ptr--;
			return *this;
			}
        iterator operator -- (int)  { 
			iterator copy(*this);
			ptr--;
			while (ptr != end && ptr->second()==false) ptr--;
			return copy;
			}
        reference operator * () const { return (ptr->first()); }
		pointer operator->() const { return &(operator*()); }
	};
private:
	class Block {
		int d;
		pai* values;
		int sz{0};
	public:
		Block() : sz(0) {}
		Block(pai* val, int _d) : d(_d), values(val), sz(0)  {}
		Block(Block& other) : d(other.d), values(other.values), sz(other.sz)  {}
		~Block() {}
		value_type& operator [] (int nr) { return (values+nr)->first(); }
		value_type get (int nr){ return (values+nr)->first(); }
		void set(pai* ptr) {values=ptr;}
		void set_sz(int _sz){sz = _sz;}
		void clear () { 
			for (int i=0; i<sz; ++i) values[i].del();
			sz = 0;
			}
		int size(){ return sz; }
		int get_d(){ return d; }
		void set_d(int _d){ d = _d; }
		int add (const value_type val) {
			if (sz != 0 && contains(val)) return 1;
			if (sz==N) return 0;
			(values+sz)->set(val);
			++sz;
			return 2;
		}
		size_type remove (const value_type& val){ //recoursion
			if (!contains(val))return 0;
			size_type ret_val = 0;
			if (N>1){
				int nr = 0;
				for (int i=0; i<sz; ++i) {
					if (key_equal{}((values+i)->first(), val)) { nr = i; ret_val = 1; break; }
				}
				for (; nr < sz-1; ++nr) (values+nr)->set((values+nr+1)->first());
				(values+sz-1)->del();
			}
			else if (N ==1) {
				values->del();
				ret_val = 1;

			}
			--sz;
			return ret_val;
		}
		bool contains (const value_type& val){
			if (sz == 0) return 0;
			bool b = false;
			for (int i=0; i<sz; ++i) if (key_equal{}(val,(values+i)->first())) { b = true; break; }
			return b;
		}
		pai* contains_ptr (const value_type& val) const {
			pai* b = NULL;
			for (int i=0; i<sz; ++i) if (key_equal{}((values+i)->first(), val)) { b = (values+i); break; }
			return b;
		}
	};
	Block** hash;
	pai* values;
	int t_sz{1}, b_sz{1}, max_b_sz{1};
	int cntr{0};
	int t{0};
/*
	int bucket_max_sz{1};
	Block** bucket_ptr;
*/
public:
	ADS_set (){
		hash = new Block*[1];
		values = new pai[N];
		hash[0] = new Block(&values[0], 0);
		//bucket_ptr = new Block*[1];
		//bucket_ptr[0] = &*hash[0];
	}
	ADS_set(std::initializer_list<value_type> ilist) {
		hash = new Block*[1];
		values = new pai[N];
		hash[0] = new Block(&values[0], 0);
		//bucket_ptr = new Block*[1];
		//bucket_ptr[0] = &*hash[0];
		insert(ilist);
	}
	template<typename InputIt> 
	ADS_set(InputIt first, InputIt last){
		hash = new Block*[1];
		values = new pai[N];
		hash[0] = new Block(&values[0], 0);
		//bucket_ptr = new Block*[1];
		//bucket_ptr[0] = &*hash[0];
		insert(first, last);
	}
	ADS_set(const ADS_set& other){
		hash = new Block*[1];
		values = new pai[N];
		hash[0] = new Block(&values[0], 0);
		//bucket_ptr =NULL;
		operator=(other);
	}
	~ADS_set(){
		delete[] values;
		int n = 0;
		if (b_sz != 1) { 
			while(b_sz){
				if (hash[n] != NULL){
					for(int i=n+1; i<t_sz; ++i){
						if (hash[i] == hash[n]) hash[i] = NULL;
					}
					delete hash[n];
					--b_sz;
				}
				++n;
			}
		}
		else {
			delete hash[0];
		}
		delete[] hash;
	}
private:
	void index_expansion(){
		Block** tmp_hash = new Block*[t_sz*10];
		for (int i=0; i<t_sz; ++i) {
			tmp_hash[(int)(i+0*pow(10,t))] = 
			tmp_hash[(int)(i+1*pow(10,t))] = 
			tmp_hash[(int)(i+2*pow(10,t))] = 
			tmp_hash[(int)(i+3*pow(10,t))] = 
			tmp_hash[(int)(i+4*pow(10,t))] =
			tmp_hash[(int)(i+5*pow(10,t))] = 
			tmp_hash[(int)(i+6*pow(10,t))] = 
			tmp_hash[(int)(i+7*pow(10,t))] = 
			tmp_hash[(int)(i+8*pow(10,t))] = 
			tmp_hash[(int)(i+9*pow(10,t))] = hash[i];
		}
		delete[] hash;
		hash=tmp_hash;
		t_sz = t_sz*10;
		++t;
	}
	int hash_getter ( const value_type& val){
		hasher hash_val;
		size_type _hasher = hash_val(val);
		int divisor = t>1?pow(10,t):10;
		return t==0?0:_hasher%divisor;
	}
	void split(size_type num){
		value_type copy[N];
		for (int i=0; i<(signed)N; ++i) copy[i] = hash[num]->get(i);
		if (max_b_sz == b_sz) {
			int sum = b_sz==1?
						b_sz*N*10:
							b_sz<10?
								b_sz*N*2:b_sz<100?
									(b_sz*N*1.8)-fmod((b_sz*N*1.8),N):b_sz<1000?
											(b_sz*N*1.23)-fmod((b_sz*N*1.23),N):b_sz<10000?
												(b_sz*N*1.15)-fmod((b_sz*N*1.15),N):(b_sz*N*1.07)-fmod((b_sz*N*1.07),N);
			pai* tmp_values = new pai[sum];
			bool check = false;
			int sizer = 0;
			int b_fill = 0;
			for(int i=0; i<(signed)(b_sz*N); ++i) {
				if (i%N == 0 && values[i].second()) {
					check = true;
					++b_fill;
				}
				if (check) {
					tmp_values[sizer++] = values[i];
					if (i%N == 0) hash[hash_getter(tmp_values[sizer-1].first())]->set(&tmp_values[sizer-1]);
					if (i%N == N-1) check = false;
				}
			}
			sizer = 0;
			for (int i=0; i<t_sz; ++i){
				if (hash[i]->size() == 0) {
					hash[i]->set(&tmp_values[b_fill*N]);
					hash[i]->set_sz(-1);
					++b_fill;
				}
				if (b_fill == b_sz) break;
			}
			for (int i=0; i<t_sz; ++i){
				if (hash[i]->size() == -1) 
					hash[i]->set_sz(0);
			}
			max_b_sz = sum/N;
			delete[] values;
			values = tmp_values;
			//if (t==3) {dump();abort();}
		}
		hash[num]->clear();
		cntr -= N;
		hash[num] = new Block(&values[b_sz*N], t);
		++b_sz;
		for (int i=0; i<(signed)N; ++i) push(copy[i]);
	}
	void push(const value_type& _val){
		int finder = hash_getter(_val);
		int test = hash[finder]->add(_val);
		if (test == 2){++cntr;}
		else if (test == 0) {
			if (hash[finder]->get_d()==t){
				index_expansion();
				push(_val);
			} else if (hash[finder]->get_d() < t) {
				split(finder);
				push(_val);
			} else { abort(); }
		}
		else if (test == 1){}
		else abort();

	}
	bool push_b(const value_type& _val){
		int finder = hash_getter(_val);
		int test = hash[finder]->add(_val);
		if (test == 2){++cntr; return true;}
		else if (test == 0) {
			if (hash[finder]->get_d()==t){
				index_expansion();
				push(_val);
				return true;
			} else if (hash[finder]->get_d() < t) {
				split(finder);
				push(_val);
				return true;
			} else { abort(); }
		}
		else if (test == 1){return false;}
		else abort();

	}
	bool finder(const value_type& _val) const {
		hasher hash_val;
		return hash[hash_val(_val)%(int)pow(10,t)]->contains(_val);
	}
	pai* finder_ptr (const value_type& _val) const {
		hasher hash_val;
		return hash[hash_val(_val)%(int)pow(10,t)]->contains_ptr(_val);
	}
public:
	void insert(std::initializer_list<value_type> ilist){
		for (value_type it : ilist) {
			push(it);
		}
	}
	template<typename InputIt> void insert(InputIt first, InputIt last){
		for (auto it = first; it != last; ++it) {
			push(*it);
		}	
	}
	std::pair<iterator,bool> insert(const value_type& key){
		bool b = push_b(key);
		std::pair <iterator,bool> p;
		p = std::make_pair(find(key), b);	
		return p;
	}
	ADS_set& operator=(const ADS_set& other){
		clear();
		for (int i=0; i<(int)(other.b_sz*N); ++i){
			if (other.values[i].second()){
				push(other.values[i].first());
			}
		}	
		return *this;
	}
	ADS_set& operator=(std::initializer_list<value_type> ilist){
		clear();
		insert(ilist);
		return *this;
	}
	size_type size() const{ return cntr; }
	bool empty() const{ return cntr==0; }
	void clear(){
		delete[] values;
		//if (bucket_ptr!=NULL) for (int i=0; i<b_sz; ++i) delete bucket_ptr[i];
		//delete[] bucket_ptr;
		int n = 0;
		while(b_sz){
			if (hash[n] != NULL){
				for(int i=n+1; i<t_sz; ++i){
					if (hash[i] == hash[n]) hash[i] = NULL;
				}
				delete hash[n];
				--b_sz;
			}
			++n;
		}
		delete[] hash;
		b_sz = 1;
		t_sz = 1;
		max_b_sz = 1;
		cntr = 0;
		t = 0;
		//bucket_max_sz = 1;
		hash = new Block*[1];
		values = new pai[N];
		hash[0] = new Block(&values[0], 0);
		//bucket_ptr = new Block*[1];
		//bucket_ptr[0] = &*hash[0];
	}
	size_type erase(const value_type& key){
		if (!finder(key)) return 0;
		hash[hash_getter(key)]->remove(key);
		--cntr;
		//if ((cntr/(b_sz*N))<0.7) cerr<<"HALT "<<cntr<<"-"<<b_sz*N<<"-"<<max_b_sz<<endl;
		return 1;
	}
	void swap(ADS_set& other){
		/*ADS_set tmp = other;
		other = *this;
		*this = tmp;*/

	Block** tmp_hash = other.hash;
	pai* tmp_values = other.values;
	//Block** tmp_bucket_ptr = other.bucket_ptr;
	other.hash = hash;
	other.values = values;
	//other.bucket_ptr = bucket_ptr;
	hash = tmp_hash;
	values = tmp_values;
	//bucket_ptr = tmp_bucket_ptr;

	int tmp_t_sz = other.t_sz; 
	int tmp_b_sz = other.b_sz; 
	int tmp_max_b_sz = other.max_b_sz;
	int tmp_cntr = other.cntr;
	int tmpt = other.t;
	//int tmp_bucket_max_sz = other.bucket_max_sz;

	other.t_sz = t_sz; 
	other.b_sz = b_sz; 
	other.max_b_sz = max_b_sz;
	other.cntr = cntr;
	other.t = t;
	//other.bucket_max_sz = bucket_max_sz;

	t_sz = tmp_t_sz; 
	b_sz = tmp_b_sz; 
	max_b_sz = tmp_max_b_sz;
	cntr = tmp_cntr;
	t = tmpt;
	//bucket_max_sz = tmp_bucket_max_sz;
	}
	size_type count(const value_type& key) const{
		return finder(key)?1:0;
	}
	iterator find(const key_type& key) const{
		if (size()==0)return end();
		pai* ptr = finder_ptr(key);
		iterator it((ptr==NULL?values[b_sz*N]:*ptr), values[b_sz*N]);
		return it;
	}
	iterator begin() const{
		if (cntr == 0) {
			iterator it(values[0], values[0]);
			return it;
		}
		else {
			int i = 0;
			for(;i<(int)(b_sz*N);++i) if (values[i].second()==true) break;
			iterator it(values[i], values[b_sz*N]);
			return it;
		}
	}
	iterator end() const{
		if (cntr == 0) {
			iterator it(values[0], values[0]);
			return it;
		}
		else {
			iterator it(values[b_sz*N], values[b_sz*N]);
			return it;
		}
	}
	void dump(std::ostream& o = std::cerr) const{
		o<<"tsz: "<<t_sz<<" bsz: "<<b_sz<<" max_b_sz: "<<max_b_sz<<" t: "<<t<<" counter is: "<<cntr<<endl;
		//for (auto it = begin(); it != end(); ++it) o<<*it<<", ";
		
		for (auto it = begin(); it != end(); ++it) o<<*it<<" ";
		//o<<endl;
		auto it = begin();
		for (int i = 0; i<(int)(max_b_sz * N); ++i){
			if (i%10 == 0) o<<endl;
			o<<*(it+i)<<"("<<(it+i).second()<<") ";
		}
		o<<endl;
		//o<<N<<endl<<*begin()<<endl<<*(end()-1)<<endl<<std::distance(begin(), end())<<endl;
		for (int i=0; i<t_sz; ++i) {
			o<<i<<" – "<<"sz: "<<hash[i]->size()<< " [";
			for (int n=0; n<hash[i]->size(); ++n) o<<hash[i]->get(n)<<" ";
			//for (int n=0; n<hash[i]->size(); ++n) o<<hash[i]->get(n)<<" ";
			o<<" ]"<<endl;
		}
	}
	value_type* get_values() const {return values;}
};

template <typename Key, size_t N>
bool operator==(const ADS_set<Key,N>& lhs, const ADS_set<Key,N>& rhs){
	if (lhs.size()!=rhs.size()) return false;
	bool b = false;
	for (auto it_1 = lhs.begin(); it_1 != lhs.end() ; ++it_1){
		for (auto it_2 = rhs.begin(); it_2 != rhs.end(); ++it_2) {
			if (equal_to<Key>{}(*it_1, *it_2)) {
				b = true;
			}
		}
		if (b==false) return false; 
		b = false;
	} 
	return true;
}
template <typename Key, size_t N>
bool operator!=(const ADS_set<Key,N>& lhs, const ADS_set<Key,N>& rhs){
	return !(lhs==rhs);
}

template <typename Key, size_t N>
void swap(ADS_set<Key,N>& lhs, ADS_set<Key,N>& rhs){
	lhs.swap(rhs);
}