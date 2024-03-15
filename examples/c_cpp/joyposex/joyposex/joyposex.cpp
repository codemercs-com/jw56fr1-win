#include <Windows.h>
#include <iostream>

int main()
{
    MMRESULT res = 0;
    JOYINFOEX joyinfoex;
    joyinfoex.dwSize = sizeof(joyinfoex);
    joyinfoex.dwFlags = JOY_RETURNALL;

    for (int i = 0; i < 1000; i++)
    {
        res = joyGetPosEx(JOYSTICKID1, &joyinfoex); //select first jostick found
        if (res == JOYERR_NOERROR)
        {
            std::cout << "X: " << joyinfoex.dwXpos << "  |  ";
            std::cout << "Y: " << joyinfoex.dwYpos << "  |  ";
            std::cout << "Z: " << joyinfoex.dwZpos << "  |  ";
            std::cout << "R: " << joyinfoex.dwRpos << "  |  ";
            std::cout << "U: " << joyinfoex.dwUpos << "  |  ";
            std::cout << "V: " << joyinfoex.dwVpos << "  |  ";
            std::cout << "Btn: " << joyinfoex.dwButtons << std::endl;
        }
    }
}
