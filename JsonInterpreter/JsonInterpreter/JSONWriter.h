#ifndef _JSONWRITER_H
#define _JSONWRITER_H

#include <iostream>
#include <string>
#include <stack>
#include <stdint.h>
class JsonWriter{

public:
	//定义两个枚举类型表示不同的 JSON 数据类型，比如说有 JSON 数组和 JSON 对象
	enum ContainerType{
		CONTAINER_TYPE_ARRAY,
		CONTAINER_TYPE_OBJECT
	};
	enum ContainerLayout{
		CONTAINER_LAYOUT_INHERIT,
		CONTAINER_LAYOUT_MULTI_LINE,
		CONTAINER_LAYOUT_SINGLE_LINE
	};

	//构造函数，用关键字 explicit 修饰，不允许隐式类型转换
	//初始化列表
	explicit JsonWriter() :
		writer(NULL),
		initialIndentDepth(0),
		indent("  "),
		containerPadding(" "),
		keyPaddingLeft(""),
		keyPaddingRight(" "),
		defaultContainerLayout(CONTAINER_LAYOUT_MULTI_LINE),
		forceDefaultContainerLayout(false){

	}

	//定义一个输出压缩格式设置的函数，可以设置输出的格式
	void ConfigureCompressedOutput(){
		SetIndent("");
		SetContainerPadding("");
		SetKeyPaddingLeft("");
		SetKeyPaddingRight("");
		SetDefaultContainerLayout(CONTAINER_LAYOUT_SINGLE_LINE);
		SetForceDefaultContainrLayout(true);
	}

	//下面是设置输出格式的一系列函数，包含 get 和 set 函数
	int GetInitialDepth(){
		return initialIndentDepth;
	}
	void SetInitialDepth(int depth){
		initialIndentDepth = depth;
	}

	const char* GetIndent(){
		return indent;
	}
	void SetIndent(const char* indent){
		this->indent = indent;
	}

	const char* GetContainerPadding(){
		return containerPadding;
	}
	void SetContainerPadding(const char* padding){
		containerPadding = padding;
	}

	const char* GetKeyPaddingLeft(){
		return keyPaddingLeft;
	}
	void SetKeyPaddingLeft(const char* paddingLeft){
		keyPaddingLeft = paddingLeft;
	}

	const char* GetKeyPaddingRight(){
		return keyPaddingRight;
	}

	void SetKeyPaddingRight(const char* paddingRight){
		keyPaddingRight = paddingRight;
	}

	ContainerLayout GetDefaultContainerLayout(){
		return defaultContainerLayout;
	}
	void SetDefaultContainerLayout(ContainerLayout containerLayout){
		defaultContainerLayout = containerLayout;
	}

	bool GetForceDefaultContainerLayout(){
		return forceDefaultContainerLayout;
	}
	void SetForceDefaultContainrLayout(bool force){
		forceDefaultContainerLayout = force;
	}

	//重载 write 函数，确定输出是屏幕输出还是文件
	std::ostream& Write(){
		if (writer == NULL){
			return std::cout;
		}
		return *writer;
	}

	void WriteEscapedChar(char c);
	void WriteString(const char* str);

	void StartChild(bool isKey);
	void StartChild(){
		StartChild(false);
	}

	void StartContainer(ContainerType type, ContainerLayout layout);
	void EndContainer();

	void StartArray() { StartContainer(CONTAINER_TYPE_ARRAY, CONTAINER_LAYOUT_INHERIT); }
	void StartArray(ContainerLayout layout) { StartContainer(CONTAINER_TYPE_ARRAY, layout); }
	void StartShortArray() { StartContainer(CONTAINER_TYPE_ARRAY, CONTAINER_LAYOUT_SINGLE_LINE); }
	void EndArray() { EndContainer(); }

	void StartObject() { StartContainer(CONTAINER_TYPE_OBJECT, CONTAINER_LAYOUT_INHERIT); }
	void StartObject(ContainerLayout layout) { StartContainer(CONTAINER_TYPE_OBJECT, layout); }
	void StartShortObject() { StartContainer(CONTAINER_TYPE_OBJECT, CONTAINER_LAYOUT_SINGLE_LINE); }
	void EndObject() { EndContainer(); }

	void Key(const char* key);
	void NullValue();
	void Value(const char* value);
	void Value(std::string value);
	void Value(bool value);

	//使用泛型编程，使得程序可以处理多个跟平台无关的数据类型
#define VALUE_DEF(t) void Value(t value){ StartChild();Write() << value; }
#define KEYVALUE_DEF(t) void KeyValue(const char * key, t value){Key(key); Value(value);}
	VALUE_DEF(int8_t)
	VALUE_DEF(uint8_t)
	VALUE_DEF(int16_t)
	VALUE_DEF(uint16_t)
	VALUE_DEF(int32_t)
	VALUE_DEF(uint32_t)
	VALUE_DEF(int64_t)
	VALUE_DEF(uint64_t)
	VALUE_DEF(float)
	VALUE_DEF(double)

	void KeyNullValue(const char* key){
		Key(key);
		NullValue();
	}

	KEYVALUE_DEF(const char *)
	KEYVALUE_DEF(std::string)
	KEYVALUE_DEF(bool)
	KEYVALUE_DEF(int8_t)
	KEYVALUE_DEF(uint8_t)
	KEYVALUE_DEF(int16_t)
	KEYVALUE_DEF(uint16_t)
	KEYVALUE_DEF(int32_t)
	KEYVALUE_DEF(uint32_t)
	KEYVALUE_DEF(int64_t)
	KEYVALUE_DEF(uint64_t)
	KEYVALUE_DEF(float)
	KEYVALUE_DEF(double)
	//定义数据成员
private:

	std::ostream *writer;
	//初始的缩进深度
	int initialIndentDepth;
	//输出格式的定义
	const char *indent;
	const char *keyPaddingLeft;
	const char *keyPaddingRight;
	const char *containerPadding;
	ContainerLayout defaultContainerLayout;
	//是否强制使用默认 layout
	bool forceDefaultContainerLayout;

	//定义一个结构，代表一个容器
	struct Container{
		ContainerType type;
		ContainerLayout layout;
		bool isKey;
		int childcount;

		//结构的构造函数
		Container(ContainerType type, ContainerLayout layout) :type(type), layout(layout), isKey(false), childcount(0){

		}
	};

	//定义一个存储 JSON 容器的栈，存储的指向结构体的指针
	std::stack<Container *> depth;

};
#endif
//_JSONWRITER_H