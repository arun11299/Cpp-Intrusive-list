#include "intrusive_list.h"

class Test: public ListNode<Test> {
public:
	Test(int val):test_data(val) {}
	int test_data;
};

int main() {
	in_list<Test> l;
	l.push_back(Test(1));

	in_list<Test>::iterator it = l.begin();
	std::cout << (*it).test_data << std::endl;
	l.push_back(Test(2));

	while (it != l.end()) {
		std::cout << (*it).test_data << std::endl;
		++it;
	}
	
	//std::cout << "Size: " << l.size() << std::endl;
	
	return 0;
}
