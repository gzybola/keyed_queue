# keyed_queue
Template that gives the programmer the functionality of FIFO. Strong exception safety, copy-on-write. 

`template <class K, class V>` class keyed_queue</br>
`K` must have no-argument default constructor, copy and move constructor and assignment operator, linear order  
`V` must have copy constructor

## Command list with time cost
* `default constructor` - creates empty queue `O(1)`
* `copy constructor` - copy-on-write `O(1)` or `O(n logn)` when copy is necessary
* `move constructor` - no-throw `O(1)`
* `desctructor` - no throw
* `assignment operator` - `O(1)`
* `push(K const &k, V const &v)` - adds value to the end with specific key `O(log n)` in some cases time of copy
* `pop()` - removes first element, if empty throws `lookup_error` `O(log n)` in some cases time of copy
* `pop(K const &)` - removes first element with given key `K` `O(log n)` in some cases time of copy
* `move_to_back(K const &k)` - moves elements with `K` key to the end of queue, preserves the order `O(log n)` in some cases time of copy where m is count(k)
* `std::pair<K const &, V &> front()`- returns first element, pair: key and value, it's possible to change value but not key `O(1)` n some cases time of copy 
* `std::pair<K const &, V &> back()` - returns last element, pair: key and value, it's possible to change value but not key `O(1)` n some cases time of copy 
* `std::pair<K const &, V const &> front() const` - as above with cost `O(1)`
* `std::pair<K const &, V const &> back() const` - as above with cost `O(1)`
* `std::pair<K const &, V &> first(K const &key)` - returns first element with given key, pair: key and value, it's possible to change value but not key `O(1)` n some cases time of copy, if there is no such key, throws `lookup_error` 
* `std::pair<K const &, V &> last(K const &key)`- returns last element with given key, pair: key and value, it's possible to change value but not key `O(1)` n some cases time of copy, if there is no such key, throws `lookup_error` 
* `std::pair<K const &, V const &> first(K const &key) const` -  as above with cost `O(1)`
* `std::pair<K const &, V const &> last(K const &key) const` - as above with cost `O(1)`
* `size_t  size() const` - returns number of elements in queue `O(1)`
* `bool empty() const` - return true if queue is empty, otherwise false `O(1)`
* `clear()` - removes all elements from queue `O(n)`
* `size_t count(K const &) const` - returns number of elements with given key `O(log n)`
* `k_iterator k_begin()` - return iterator to the beginning of queue 
* `k_iterator k_end()` - returns iterator to the end of queue 

##### Iterator operations:
* `default no-argument` and `copy constructor`
* `++ operator`
* `!=` and `== operators`
* `operator*`
</br>Created thanks to `Boost.Operators`. Iterators are null when queue is successfully modified. Those objects provides read-only access in increasing order.


Project was made for classes at University of Warsaw.
