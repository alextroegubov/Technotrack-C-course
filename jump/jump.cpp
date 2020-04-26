#include <setjmp.h>
#include <cstdio>
#include <string>
#include <vector>

int GlobalLevel = 0;

#define $				printf(">>> [%3d]%*s%s\n\n", __LINE__, GlobalLevel, " ", __PRETTY_FUNCTION__);

#define $DO(smth)		printf(">>> [%3d]%*s%s: %s\n\n", __LINE__, GlobalLevel, " ", __PRETTY_FUNCTION__, #smth); smth;

#define $FUNCTION		Indent indent; $

#define $SPACES			static_cast<int>(sizeof(">>> [000]") - 1 + Indent::indent)

#define $PRINT_SPACES	printf("%*s", GlobalLevel + $SPACES, " ");

struct Indent{
	static const int indent = 3;
	 Indent(){ GlobalLevel += indent;}
	~Indent(){ GlobalLevel -= indent;}
};


struct Test{
	//value constructor
	explicit Test(std::string new_name, int new_size = 5, int new_value = 0);

	void* operator new(size_t, void* place);

	//copy constructor
	Test(const Test& test);
  
	//copy operator
	Test& operator=(const Test& test);

	//move contructor
	Test(Test&& test);

	//move operator
	Test& operator=(Test&& test);

	//d-r
	~Test();
		
	size_t size_;
	int* data_;
	std::string name_;
};

struct Jump{
	static void Throw(int val){
		//destroy objects
		for(Test* item: Gtable){ 
			item->~Test(); 
		}

		ret_val = val;
		printf("Exception returned %d \n", ret_val);

		longjmp(Genv, val);
	}

	static void Add(Test* p){
		Gtable.push_back(p);
	}

	static std::vector<Test*> Gtable;
	static int ret_val;
	static jmp_buf Genv;
};

int Jump::ret_val = 0;
std::vector<Test*> Jump::Gtable;
jmp_buf Jump::Genv;


#define SAVE 		Jump::ret_val = setjmp(Jump::Genv); if(Jump::ret_val != 0) return 0;
#define THROW(n) 	Jump::Throw(n);							
#define ADD(p) 		Jump::Add(p);

template<typename T>
void MySwap(T& a, T& b){

	T tmp = std::move(a);
	a = std::move(b);
	b = std::move(tmp);
}

//value constructor
Test::Test(std::string new_name, int new_size, int new_value):
		size_(new_size),
		data_(new int[new_size]),
		name_(new_name){   
	$FUNCTION 
	
	std::fill(data_, data_ + size_, new_value);

	ADD(this);

	$PRINT_SPACES
	printf("value c-r: %s[%p]\n\n", name_.c_str(), this);
}

void* Test::operator new(size_t, void* place){
	return place;
}

//copy constructor
Test::Test(const Test& obj):
		size_(obj.size_),
		data_(new int[obj.size_]),
		name_("(copy of " + obj.name_ + ")"){   		
	$FUNCTION
		
	std::copy(obj.data_, obj.data_ + obj.size_, data_);

	ADD(this);

	$PRINT_SPACES
	printf("copy c-r: %s[%p]\n\n", name_.c_str(), this);
}
  
//copy operator
Test& Test::operator=(const Test& obj){			
	$FUNCTION

	delete[] data_;
	new(this) Test(obj);

	$PRINT_SPACES
	printf("copy c-r: %s[%p]\n\n", name_.c_str(), this);

	return *this;
}

//move contructor
Test::Test(Test&& test):
		size_(0),
		data_(nullptr),
		name_("(steal of " + test.name_ + ")"){   
	$FUNCTION

	MySwap(test.data_, this->data_);
	MySwap(test.size_, this->size_);
	
	ADD(this);

	$PRINT_SPACES
	printf("move c-r: %s[%p]\n\n", name_.c_str(), this);
}

//move operator
Test& Test::operator=(Test&& test){   
	$FUNCTION

	name_ = "(steal of " + test.name_ + ")";

	MySwap(test.data_, this->data_);
	MySwap(test.size_, this->size_);
		
	$PRINT_SPACES
	printf("move op-r: %s[%p]\n\n", name_.c_str(), this);

	return *this;
}

//d-r
Test::~Test(){
	$FUNCTION

	$PRINT_SPACES
	printf("d-r: %s[%p]\n\n", name_.c_str(), this);
	fflush(stdout);
	delete[] data_;
}

void Func2();
void Func1();
void Func3();

void Func2(){
	$FUNCTION
	Test x("x", 3);
	Test y("y", 3);

	Func3();
}

void Func1(){
	$FUNCTION
	Test t("t", 3);
	Test p("p", 3);

	Func2();
}

void Func3(){
	$FUNCTION

	Test h("h", 3);
	THROW(10);
	Test g("g");
}

int main(){

	SAVE;

	Func1();
	
	return 0;
}
