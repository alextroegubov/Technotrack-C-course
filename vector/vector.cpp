#include<assert.h>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <string>
#include "vector.h"

void* Vector::operator new(size_t size, void* place){
	return place;
}

Vector::Vector(size_t n /*= 1*/):
		buf_(new type_t [n * sizeof(*buf_)]),
		size_(0),
		max_size_(n){
	
	Assert(); //to check new 
	Clear();
}

Vector::Vector(size_t n, type_t value):
		buf_(new type_t [n * sizeof(*buf_)]),
		size_(n),
		max_size_(n){
	
	std::fill(buf_, buf_ + size_, value);
	Assert();
}

Vector::Vector(const Vector& v):
		buf_(new type_t [v.size_ * sizeof(*buf_)]),
		size_(v.size_),
		max_size_(v.max_size_){
	
	Assert();
	std::copy(v.buf_, v.buf_ + v.size_, buf_);
	Assert();
}

Vector& Vector::operator=(const Vector& copy_v){

	Assert();
	delete[] buf_;
	new(this) Vector(copy_v);
	Assert();
}


Vector::Vector(Vector&& v):
		size_(v.size_),
		max_size_(v.max_size_),
		buf_(v.buf_),
		error_(v.error_){

	v.buf_ = nullptr;
	Assert();
}

Vector& Vector::operator=(Vector&& v){
	
	Assert();
	std::swap(buf_, v.buf_);
	std::swap(size_, v.size_);
	std::swap(max_size_, v.max_size_);
	std::swap(error_, v.error_);
	Assert();

	return *this;
}


Vector::~Vector(){

	if(buf_ == nullptr && size_ == 0 && max_size_ == 0){
		error_ = destructor_invoked_twice;
		Assert();
	}

	delete[] buf_;
	buf_ = nullptr;
	size_ = 0;
	max_size_ = 0;
}

void Vector::Assert(){

	if(Ok() == false){
		Dump();
		assert(!"OK");
	}
}

void Vector::Dump(std::ostream& output /*= std::cout*/) const{

	output << std::endl << "**********Vector dump**********" << std::endl;
	
	output << "error_ = " << ErrorToString();
	output << ((error_ == no_error) ? " (no errors detected)" 
										: " (ERROR detected !!!)") << std::endl;

	output << "size_ = " << size_;
	output << ((size_ < 0) ? " ( < 0 !!!)" : "") << std::endl;

	output << "max_size_ = " << max_size_ << std::endl;
	output << ((max_size_ < 0) ? " ( < 0 !!!)" : "") << std::endl;	

	bool all_is_poison = false;

	output << "\tbuf_[" << buf_ << "] = {" << std::endl;

	if(error_ == no_error){
		all_is_poison = true;

		for(auto i = 0; i < max_size_; i++){
			output << "\t[" << i << "] = " << buf_[i];

			if(buf_[i] == POISON_VALUE){
				output << " (POISON ?)\n"; 
			}
			else{
				all_is_poison = false;
				output << "\n";
			}
			//output << ((buf_[i] == POISON_VALUE) ? " (POISON ?)\n" : "\n");
			//check \n
		}
	}
	output << "\t}" << std::endl;
	
	output << (all_is_poison && (size_ == 0) ? "Vector has been cleared" : "") << "\n";

	output << "Vector dump is finished" << std::endl;
}

bool Vector::Ok(){

	if(error_ != no_error)
		return false;

	if(buf_ == nullptr){
		error_ = buf_is_null;
	}
	else if(size_ < 0){
		error_ = size_is_neg;
	}
	else if(max_size_ < 0){
		error_ = max_size_is_neg;
	}
	else if(size_ > max_size_){
		error_ = size_is_greater_than_max_size;
	}

	return (error_ == no_error);
}


void Vector::PushBack(const type_t& elem){

	if(size_ == max_size_){
		Resize(max_size_ * 2);
	}

	buf_[size_++] = elem;
	Assert();
}


void Vector::PopBack(){
	Assert();
	if(size_ == 0){
		error_ = Vector::errors::pop_back_from_empty_vector;
		Assert();
	}
	buf_[size_--] = POISON_VALUE;
	Assert();
}
void Vector::Resize(const size_t& new_size){
	
	max_size_ = new_size;
	size_ = (size_ > new_size)? new_size : size_;

	type_t* new_buf = new type_t[new_size];
	std::fill(new_buf, new_buf + new_size, POISON_VALUE);

	if(!buf_){
		error_ = Vector::errors::memory_alloc_failed;
		Assert();
	}

	std::copy(buf_, buf_ + size_, new_buf);

	delete[] buf_;
	buf_ = new_buf;
	new_buf = nullptr;

	if(!buf_){
		error_ = Vector::errors::memory_alloc_failed;
		Assert();
	}

	Assert();
}

type_t& Vector::operator[](int n){
	Assert();

	assert(n >= 0);
	assert(n < max_size_);

	return buf_[n];
}	

std::string Vector::ErrorToString() const{
	switch(error_){
		case no_error:
			return "No error";
		case size_is_neg:
			return "Size of Vector has negative value";
		case max_size_is_neg:
			return "Max size of Vector has negative value";
		case buf_is_null:
			return "Buf pointer is null";
		case size_is_greater_than_max_size:
			return "Size of Vector is greater than its max size";
		case memory_alloc_failed:
			return "Memory (re)allocation failed";
		case destructor_invoked_twice:
			return "Destructor has been invoked several times";
		case pop_back_from_empty_vector:
			return "Trying to pop element from empty vector";
		default:
			return "Unknown error";
	};
}

void Vector::Clear(){

	std::fill(buf_, buf_ + max_size_, POISON_VALUE);
	size_ = 0;
	Assert();
}
