#include <stdio.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <algorithm>

/*
	Laboratory work about constructors operator=:
		catching value/copy/move constructors and copy/move operator=
*/
int GlobalLevel = 0;

#define $          		printf(">>> [%3d]%*s%s\n\n", __LINE__, GlobalLevel, " ", __PRETTY_FUNCTION__);

#define $DO(smth)  		printf(">>> [%3d]%*s%s: %s\n\n", __LINE__, GlobalLevel, " ", __PRETTY_FUNCTION__, #smth); smth;

#define $FUNCTION 		Indent indent; $

#define $SPACES 		static_cast<int>(sizeof(">>> [000]") - 1 + Indent::indent)

#define $PRINT_SPACES 	printf("%*s", GlobalLevel + $SPACES, " ");

//to make indents automatically
struct Indent{
	static const int indent = 3;
	 Indent(){ GlobalLevel += indent;}
	~Indent(){ GlobalLevel -= indent;}
};

void PrintArray(int* a, size_t sz){
	std::cout << "\t\t: ";

	for(size_t i = 0; i < sz; i++){
		std::cout << a[i] << " ";
	}
}

template<typename T>
void MySwap(T& a, T& b){

	T tmp = std::move(a);
	a = std::move(b);
	b = std::move(tmp);
}

struct TestObj{
	//value constructor
	explicit TestObj(std::string new_name, int new_size = 5, int new_value = 0):
			size_(new_size),
			data_(new int[new_size]),
			name_(new_name){   

		$FUNCTION 
		
		std::fill(data_, data_ + size_, new_value);

		$PRINT_SPACES
		printf("value c-r: %s[%p]\n\n", name_.c_str(), this);
	}

	//placement new
	void* operator new(size_t, void* place){
		return place;
	}

	//copy constructor
	TestObj(const TestObj& obj):
			size_(obj.size_),
			data_(new int[obj.size_]),
			name_("(copy of " + obj.name_ + ")"){   		
		$FUNCTION
		
		std::copy(obj.data_, obj.data_ + obj.size_, data_);

	/*
		$PRINT_SPACES
		printf("_____________sleep______________\n");
		sleep(5u);
	*/
		$PRINT_SPACES
		printf("copy c-r: %s[%p]\n\n", name_.c_str(), this);
	}
  
	//copy operator
	TestObj& operator=(const TestObj& obj){				
		$FUNCTION

		delete[] data_;
		new(this) TestObj(obj);

		$PRINT_SPACES
		printf("copy op-r: %s[%p]\n\n", name_.c_str(), this);

		return *this;
	}

	//move contructor
	TestObj(TestObj&& test):
			size_(0),
			data_(nullptr),
			name_("(steal of " + test.name_ + ")"){   
		$FUNCTION

		MySwap(test.data_, this->data_);
		MySwap(test.size_, this->size_);
	
		$PRINT_SPACES
		printf("move c-r: %s[%p]\n\n", name_.c_str(), this);
	}

	//move operator
	TestObj& operator=(TestObj&& test){   
		$FUNCTION

		name_ = "(steal of " + test.name_ + ")";

		MySwap(test.data_, this->data_);
		MySwap(test.size_, this->size_);
		
		$PRINT_SPACES
		printf("move op-r: %s[%p]\n\n", name_.c_str(), this);

		return *this;
	}

	//destructor
	~TestObj(){		
		$FUNCTION

		$PRINT_SPACES
		printf("d-r: %s[%p]\n\n", name_.c_str(), this);
		delete[] data_;
	}

	size_t size_;
	int* data_;
	std::string name_;
};


TestObj operator+(const TestObj& x1, const TestObj& x2){	
	$FUNCTION

	TestObj result("result", std::min(x1.size_, x2.size_));

	for(size_t i = 0; i < std::min(x1.size_, x2.size_); i++)
		result.data_[i] = x1.data_[i] + x2.data_[i];

	result.name_ = "(" + x1.name_ + " + " + x2.name_ + ")";
	
	$PRINT_SPACES
	printf("added: %s[%p]\n\n", result.name_.c_str(), &result);
	
	return std::move(result);
}


TestObj Func1(TestObj x){
	$FUNCTION

	return std::move(x);
	//return x;
}

TestObj Func2(TestObj& x){
	$FUNCTION

	return x;
}

TestObj Func3(TestObj& x){
	$FUNCTION

	return std::move(x);
}

TestObj& Func4(TestObj& x){
	$FUNCTION

	return x;
}

TestObj Func5(const TestObj& x){
	TestObj t1("t1", 5, 1);
	TestObj t2("t2", 5, 1);

	return TestObj(t1+t2) + x;
}

int main(){ $FUNCTION

	$DO(TestObj a("a", 5, 2);)
	$DO(TestObj b("b", 10, 1);)
	$DO(TestObj c("c", 20);)
{
	//	(steal of (steal of (copy of b)))
	//	$DO(c = Func1(b);)

	//	(steal of (copy of b))
	//	$DO(c = Func2(b);)

	//	(steal of (steal of b))
	//	$DO(c = Func3(b);)
	
	//  (copy of b)	
	//	$DO(c = Func4(b);)

	//	(steal of (steal of (steal of ((steal of (steal of (t1 + t2))) + a))))
	//	$DO(c = Func5(a));
}
{
	//	steal of(steal of((copy of a) + (copy of b)))
	//	$DO(c = Func2(a) + Func2(b));

	//	(steal of(steal of(a + (copy of b))))
	//	$DO(c = Func4(a) + Func2(b);)

	//	(steal of (copy of a))
	//	$DO(c = Func2(Func4(a)));
}
{
	
//		$DO(a = b;)
//		$DO(b = a;)
//		$DO(a = b;)
//		$DO(b = a;)
//		$DO(c = b;)
	
}
	printf("\n****** %s ******\n", c.name_.c_str());
	PrintArray(c.data_, c.size_);
	std::cout << std::endl;

	return 0;
}
//g++ -o research research.cpp -fno-elide-constructors
