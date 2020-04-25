#pragma once

#include<assert.h>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <string>

#define POISON_VALUE -999
typedef int type_t;

class Vector{
public:
    explicit Vector(size_t size = 1);

	explicit Vector(size_t size, type_t value);
	//copy constructor  
	explicit Vector(const Vector& v);
	//copy operator
	Vector& operator=(const Vector& v);

	//move constructor
	Vector(Vector&& v);
	//move operator
	Vector& operator=(Vector&& v);
	//destructor
	~Vector();
	//placement new
	void* operator new(size_t size, void* place);

	type_t& operator[](int n);
	size_t GetSize() const{ return size_;}
	void Dump(std::ostream& output = std::cout) const; 
	bool Ok();
	void Resize(const size_t& n);
	void Clear();
    void Assert();
	void PushBack(const type_t& elem);
	void PopBack(); 

private:	
	size_t max_size_;
	size_t size_;
	type_t* buf_;
	
	std::string ErrorToString() const;

	enum errors{
		no_error,
		size_is_neg,
		max_size_is_neg,
		buf_is_null,
        memory_alloc_failed, 
		size_is_greater_than_max_size,
        destructor_invoked_twice,
		pop_back_from_empty_vector
	};

	enum errors error_ = no_error;
};
