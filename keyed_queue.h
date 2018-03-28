#ifndef KEYED_QUEUE_H
#define KEYED_QUEUE_H

#include <memory>
#include <map>
#include <list>
#include <utility>
#include <functional>
#include <iterator>
#include "boost/iterator/transform_iterator.hpp"

class lookup_error : public std::exception {};

template <class K, class V>
class keyed_queue {

	typedef typename std::list<typename std::list<std::pair<K, V> > :: iterator > iteratorListType;

	class queueData {

		public:

			std::list<std::pair<K, V> > myList;
			std::map<K, std::list<typename std::list<std::pair<K, V> > :: iterator > > iterators;

			queueData() {}

			queueData(queueData const & queueDataToCopy) {
				myList = std::list<std::pair<K, V> >(queueDataToCopy.myList);
				iterators.clear();
				for (typename std::list<std::pair<K, V> > :: iterator it = myList.begin() ; it != myList.end() ; ++it) {
					iterators[it->first].push_front(it);
				}
			}
	};

	bool copyMust;
	std::shared_ptr<queueData> myQueue;

	void checkUseCount() {
		if (myQueue.use_count() > 1) {
			queueData* temp = myQueue.get();
			myQueue = std::make_shared<queueData>(*temp);
		}
	}

	void checkQueueDataList() const {
		if (empty()) {
			throw lookup_error();
		}
	}

	void checkQueueDataMap(K k) const {
		if (myQueue->iterators.count(k) == 0) {
			throw lookup_error();
		}
	}

	static typename std::map<K, iteratorListType>::key_type get_key(typename std::map<K, iteratorListType>::value_type aPair) {
		return aPair.first;
	}

	typedef typename std::map<K, iteratorListType>::key_type (*get_key_t)(typename std::map<K, iteratorListType>::value_type);
	typedef typename std::map<K, iteratorListType>::iterator map_iterator;
	typedef boost::transform_iterator<get_key_t, map_iterator> k_iterator;

	public:

	keyed_queue() {
		copyMust = false;
		myQueue = std::make_shared<queueData>();
	}

	keyed_queue(keyed_queue const& queueToCopy) {
		myQueue =  queueToCopy.myQueue;		
		if (queueToCopy.copyMust) {
			queueData* temp = queueToCopy.myQueue.get();
			myQueue = std::make_shared<queueData>(*temp);
		}
		copyMust = false;
	}

	keyed_queue(keyed_queue const && queueToCopy) noexcept {
		copyMust = false;
		myQueue = queueToCopy.myQueue;
	}

	keyed_queue &operator=(keyed_queue other) noexcept {
		myQueue = other.myQueue;
		copyMust = other.copyMust;
		return *this;
	}

	void push(K const &k, V const &v) {
		std::pair<K, V> newPair = std::make_pair(k, v);
		checkUseCount();
		myQueue->myList.push_front(newPair);
		typename std::list<std::pair<K, V> > :: iterator newIterator = myQueue->myList.begin();
		try {
			myQueue->iterators[k].push_back(newIterator);
		}
		catch (...) {
			myQueue->myList.pop_front();
		}
	}

	void pop() {
		checkUseCount();
		checkQueueDataList();

		auto iter = myQueue->iterators.find(myQueue->myList.back().first);
		size_t k_size = myQueue->iterators[myQueue->myList.back().first].size();

		myQueue->iterators[myQueue->myList.back().first].pop_front();
		if(k_size == 1) {
			myQueue->iterators.erase(iter);
		}
		myQueue->myList.pop_back();
	}

	void pop(K const &k) {
		checkUseCount();
		checkQueueDataMap(k);

		size_t k_size = myQueue->iterators[k].size();
		auto it_to_iterator = myQueue->iterators.find(k);

		typename std::list<std::pair<K, V> > :: iterator myPairIterator = myQueue->iterators[k].front();
		myQueue->iterators[k].pop_front();
		if(k_size == 1) {
			myQueue->iterators.erase(it_to_iterator);
		}
		myQueue->myList.erase(myPairIterator);
	}

	void move_to_back(K const &k) {
		checkUseCount();
		checkQueueDataMap(k);

		int count = myQueue->iterators[k].size();
		auto iter = myQueue->iterators[k];
		std::list<std::pair<K, V> > tmpList;

		for (int i = 0 ; i < count ; i++) {
			tmpList.splice(tmpList.begin(), myQueue->myList, iter.front());
			iter.pop_front();
		}

		myQueue->myList.splice(myQueue->myList.begin(), tmpList);
	}

	std::pair<K const &, V &> front() {
		checkUseCount();
		checkQueueDataList();

		copyMust = true; //nastepne elementy muszą zrobić całą kopię, bo użytkownik może zmienić zwracaną parę
		return std::make_pair(std::ref(myQueue->myList.back().first), std::ref((myQueue->myList.back().second)));
	}

	std::pair<K const &, V &> back() {
		checkQueueDataList();
		checkUseCount();

		copyMust = true;
		return std::make_pair(std::ref(myQueue->myList.front().first), std::ref(myQueue->myList.front().second));
	}

	std::pair<K const &, V const &> front() const {
		checkQueueDataList();

		return std::make_pair(std::ref(myQueue->myList.back().first), std::ref(myQueue->myList.back().second));
	}

	std::pair<K const &, V const &> back() const {
		checkQueueDataList();

		return std::make_pair(std::ref(myQueue->myList.front().first), std::ref(myQueue->myList.front().second));
	}

	std::pair<K const &, V &> first(K const &key) {
		checkQueueDataMap(key);

		copyMust = true;
		return std::make_pair(std::ref(myQueue->iterators[key].front()->first), std::ref(myQueue->iterators[key].front()->second));
	}

	std::pair<K const &, V &> last(K const &key) {
		checkQueueDataMap(key);
		checkUseCount();

		copyMust = true;
		return std::make_pair(std::ref(myQueue->iterators[key].back()->first), std::ref(myQueue->iterators[key].back()->second));
	}

	std::pair<K const &, V const &> first(K const &key) const {
		checkQueueDataMap(key);

		return std::make_pair(std::ref(myQueue->iterators[key].front()->first), std::ref(myQueue->iterators[key].front()->second));
	}

	std::pair<K const &, V const &> last(K const &key) const {
		checkQueueDataMap(key);

		return std::make_pair(std::ref(myQueue->iterators[key].back()->first), std::ref(myQueue->iterators[key].back()->second));
	}

	size_t size() const noexcept {
		return myQueue->myList.size();
	}

	bool empty() const noexcept {
		return myQueue->myList.empty();
	}

	size_t count(K const &k) const noexcept {
		if (myQueue->iterators.count(k) == 0) {
			return 0;
		}
		return myQueue->iterators[k].size();
	}

	void clear() {	
		myQueue = std::make_shared<queueData>();
		copyMust = false;
	}

	k_iterator k_begin() noexcept {
		k_iterator keybegin(myQueue->iterators.begin(), get_key);

		return keybegin;
	}

	k_iterator k_end() noexcept {
		k_iterator keyend(myQueue->iterators.end(), get_key);

		return keyend;
	}
};

#endif //KEYED_QUEUE_H
