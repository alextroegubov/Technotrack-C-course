#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <ctime>


//it was: Obj* o = new Obj();
//it became: Obj* o = new(line, func, file) Obj();

struct MemoryInfo{
	MemoryInfo():
		line(0),
		file(""),
		func(""),
		f(nullptr){
		
		f = fopen("memory_log_file.txt", "w");

		if(!f){
			std::cerr << "Can't open memory_log_file \n";
		}
		std::time_t my_time = std::time(nullptr);
		fprintf(f, "\t\t\t\t MEMORY_LOG_FILE  %s\n", std::asctime(std::localtime(&my_time)));
	}

	~MemoryInfo(){
		fclose(f);
	}

	int line;
	std::string file;
	std::string func;
	FILE* f;
};

MemoryInfo GL_mem_info = {};

void* operator new(size_t size, std::string file, const int line, std::string func){
	void* ptr = operator new(size);

	fprintf(GL_mem_info.f, 	"[%p] new in file: %s, %s, #%d allocated %lu bytes\n", 
							ptr, file.c_str(), func.c_str(), line, size);

	return ptr;
}

void* operator new[](size_t size, std::string file, const int line, std::string func){
	void* ptr = operator new[](size);

	fprintf(GL_mem_info.f, "[%p] new in file: %s, %s, #%d allocated %lu bytes\n", 
							ptr, file.c_str(), func.c_str(), line, size);

	return ptr;
}

#define NEW new(__FILE__, __LINE__ , __PRETTY_FUNCTION__)

void operator delete(void *ptr){	

	fprintf(GL_mem_info.f, 	"[%p] delete in file: %s, %s, #%d\n", 
							ptr, GL_mem_info.file.c_str(), GL_mem_info.func.c_str(), GL_mem_info.line);

	free(ptr);
}

void operator delete[](void *ptr){

	fprintf(GL_mem_info.f, 	"[%p] delete in file: %s, %s, #%d\n",
							ptr, GL_mem_info.file.c_str(), GL_mem_info.func.c_str(), GL_mem_info.line);
	
	free(ptr);
}

#define DELETE 	GL_mem_info.file = __FILE__; GL_mem_info.func = __PRETTY_FUNCTION__;\
				GL_mem_info.line = __LINE__; delete

class Obj{	
	int a;
	double d;
	char c;

public:
	Obj() = default;
/*
	If destructor isn't empty, the number of elements in array are stored in extra memory*/

//	~Obj() = default;
//	~Obj(){ printf("[%p] destroyed\n", this);}
	~Obj(){ ;}
};


int main(){

	Obj *obj = NEW Obj();
	printf("size of Obj = %lu\n", sizeof(Obj));

	Obj *arr = NEW Obj[10];
	printf("size of Obj[10] = %lu\n", sizeof(Obj) * 10);
	printf("arr[%p], *[%p] = %lu - number of objects in array\n", 
			arr, ((unsigned long int*)arr) - 1, *(((unsigned long int*)arr) - 1));

	DELETE obj;
	DELETE[](arr);

	return 0;
}
