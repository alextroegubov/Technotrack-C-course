#include <stdio.h>
#include <string>
#include <stdio.h>
#include <unistd.h>

int GlobalLevel = 0;

#define $				printf(">>> [%3d]%*s%s\n\n", __LINE__, GlobalLevel, " ", __PRETTY_FUNCTION__);

#define $DO(smth)		printf(">>> [%3d]%*s%s: %s\n\n", __LINE__, GlobalLevel, " ", __PRETTY_FUNCTION__, #smth); smth;

#define $FUNCTION 		Indent indent; $

#define $SPACES 		static_cast<int>(sizeof(">>> [000]") - 1 + Indent::indent)

#define $PRINT_SPACES	printf("%*s", GlobalLevel + $SPACES, " ");

struct Indent{
	static const int indent = 3;
	 Indent(){ GlobalLevel += indent;}
	~Indent(){ GlobalLevel -= indent;}
};

template<typename T>
void MySwap(T& a, T& b){

	T tmp = std::move(a);
	a = std::move(b);
	b = std::move(tmp);
}

struct TestObj{
	//value constructor
	explicit TestObj(std::string new_name, int new_data = 0):
			data_(new_data),
			name_(new_name){   
		$FUNCTION 

		$PRINT_SPACES
		printf("value c-r: %s[%p] = %d\n\n", name_.c_str(), this, data_);
	}

	//copy constructor
	TestObj(const TestObj& test):
			data_(test.data_),
			name_("(copy of " + test.name_ + ")"){   
		$FUNCTION
	/*	
		$PRINT_SPACES
		printf("_____________sleep______________\n");
		sleep(5u);
	*/
		$PRINT_SPACES
		printf("copy c-r: %s[%p] = %d\n\n", name_.c_str(), this, data_);
	}
  
	//copy operator
	TestObj& operator=(const TestObj& test){
		$FUNCTION

		name_ = "(" + name_ + " = " + "copy of " + test.name_ + ")"; 
		data_ = test.data_;
	/*	
		$PRINT_SPACES
		printf("_____________sleep______________\n");
		sleep(5u);
	*/	
		$PRINT_SPACES
		printf("copy op-r: %s[%p] = %d\n\n", name_.c_str(), this, data_);

		return *this;
	}


	TestObj(TestObj&& test):
			data_(0),
			name_("(steal of " + test.name_ + ")"){   
		$FUNCTION

		//actually swap
		MySwap(test.data_, this->data_);
	
		$PRINT_SPACES
		printf("move c-r: %s[%p] = %d\n\n", name_.c_str(), this, data_);
	}

	//move operator
	TestObj& operator=(TestObj&& test){   
		$FUNCTION

		name_ = "(steal of " + test.name_ + ")";

		MySwap(test.data_, this->data_);
		
		$PRINT_SPACES
		printf("move op-r: %s[%p] = %d\n\n", name_.c_str(), this, data_);

		return *this;
	}

	//d-r
	~TestObj(){	
		$FUNCTION

		$PRINT_SPACES
		printf("d-r: %s[%p] = %d\n\n", name_.c_str(), this, data_);
	}

	int data_;
	std::string name_;
};


TestObj operator+(const TestObj& x1, const TestObj& x2){	
	$FUNCTION

	TestObj result("result");
	result.data_ = x1.data_ + x2.data_;
	result.name_ = "(" + x1.name_ + " + " + x2.name_ + ")";
	
	$PRINT_SPACES
	printf("added: %s[%p] = %d\n\n", result.name_.c_str(), &result, result.data_);
	
	return std::move(result);
}

TestObj Func1(TestObj x){
	$FUNCTION

	return std::move(x);
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
	TestObj t1("t1", 1);
	TestObj t2("t2", 2);

	return TestObj(t1+t2) + x;
}

int main(){ $FUNCTION

	$DO(TestObj a("a", 1);)
	$DO(TestObj b("b", 2);)
	$DO(TestObj c("c");)

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
	printf("\n****** %s = %d ******\n", c.name_.c_str(), c.data_);
	$
	return 0;
}
//g++ -o research research.cpp -fno-elide-constructors
