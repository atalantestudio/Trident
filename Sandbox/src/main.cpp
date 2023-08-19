namespace ProjectA {
	__declspec(dllimport) void test();
}

int main() {
	ProjectA::test();
}