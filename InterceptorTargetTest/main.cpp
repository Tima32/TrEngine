#include <windows.h>
#include <conio.h>

#include <iostream>

using namespace std;

typedef void(*MYFUNC)();
HMODULE module = nullptr;
void Load()
{
	module = LoadLibraryW(L"ScarletNexusTrainerDLL.dll");
	MYFUNC TestCall = (MYFUNC)GetProcAddress(module, "TestCall");
	//TestCall();
}
void Unload()
{
	FreeLibrary(module);
}
int main()
{
	size_t* counter = new size_t;
	*counter = 0;

	cout << "Increment Load Free" << endl;
	while (1)
	{
		char ch = _getch();
		cout << ch << endl;

		if (ch == 'i')
		{
			*counter += 1;
			cout << *counter << " " << counter << endl;
		}
		if (ch == 'l')
			Load();
		if (ch == 'f')
			Unload();
	}

	return 0;
}